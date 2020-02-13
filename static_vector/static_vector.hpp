#ifndef STATIC_VECTOR_HPP
#define STATIC_VECTOR_HPP

#include <cstddef>            // std::size_t, std::ptrdiff_t
#include <algorithm>          // std::fill, std::copy, std::for_each
#include <utility>            // std::exchange, std::swap
#include <iterator>           // std::reverse_iterator
#include <cassert>            // assert
#include <stdexcept>          // std::out_of_range
#include <sstream>            // For constructing error message
#include <initializer_list>   // std::initializer_list
#include <memory>             // std::allocator, std::allocator_traits
#include <iterator>           // std::iterator_traits, std::input_iterator_tag

namespace static_vector {

namespace detail {
// TODO: Make a wrapper so that iterator is not a raw pointer
template<class Ptr>
using iter_wrap = Ptr;
}   // namespace detail

template<class T>
struct new_delete_allocator {
    using value_type = T;

    template<class U, class... Args>
    void construct(U * /* unused */, Args &&... /* unused */) {}

    [[nodiscard]] T *allocate(std::size_t n, const void * /* unused */ = 0) {
        return new T[n];
    }

    void deallocate(T *p, std::size_t /* unused */) {
        delete[] p;
    }

    void destroy(T * /* unused */) {}
};

template<class T, class Allocator = std::allocator<T>>
struct vector : private Allocator {   // Empty base optimization for most cases

    // BEGIN Typedefs
    using value_type = T;
    using allocator_type = Allocator;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = detail::iter_wrap<pointer>;
    using const_iterator = detail::iter_wrap<const_pointer>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    // END Typedefs

    vector(const allocator_type &alloc, size_type size, pointer p) noexcept :
            Allocator(alloc), m_size(size), m_data(p) {}

    struct vector_factory {
        allocator_type m_alloc;
        const size_type m_size;
        pointer m_data;
        pointer m_end = m_data;

        vector_factory(allocator_type alloc, size_type size) :
                m_alloc(alloc), m_size(size), m_data(allocator_traits::allocate(alloc, m_size)) {
        }

        template<class Iter>
        vector_factory(allocator_type alloc, Iter first, Iter last) :
                vector_factory(alloc, std::distance(first, last)) {
            for (auto it = first; it != last; ++it, ++m_end) {
                allocator_traits::construct(alloc, m_end, *it);
            }
        }

        vector_factory(allocator_type alloc, size_type size, std::in_place_t /* unused */) :
                vector_factory(alloc, size) {
            for (; m_end != m_data + m_size; ++m_end) {
                allocator_traits::construct(alloc, m_end);
            }
        }

        vector_factory(allocator_type alloc, size_type size, const T &t) : vector_factory(alloc, size) {
            for (; m_end != m_data + m_size; ++m_end) {
                allocator_traits::construct(alloc, m_end, t);
            }
        }

        operator vector() && {
            return vector(m_alloc, m_size, std::exchange(m_data, nullptr));
        }

        // Will be called with non-null m_data iff allocation was successful, but construction failed
        ~vector_factory() {
            if (!m_data) {
                return;
            }

            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (; m_end != m_data; ) {
                    allocator_traits::destroy(m_alloc, --m_end);
                }
            }

            allocator_traits::deallocate(m_alloc, m_data, std::distance(m_data, m_end));
        }
    };

    template<class... Args>
    void construct(pointer loc, Args &&... args) {
        allocator_traits::construct(*this, loc, std::forward<Args>(args)...);
    }

    template<class... Args>
    void construct_all(Args &&... args) {
        for (pointer p = std::begin(*this); p != std::end(*this); ++p) {
            construct(p, std::forward<Args>(args)...);
        }
    }

  public:
    // BEGIN Constructors
    vector(size_type size, const T &value, const allocator_type &alloc = {}) :
            vector(vector_factory(alloc, size, value)) {}

    // Construction in-place without copy.
    explicit vector(size_type size, const allocator_type &alloc = {}) :
            vector(vector_factory(alloc, size, std::in_place_t{})) {}

    template<class Iter,
             class = std::enable_if_t<std::is_base_of_v<
                 std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>>>
    vector(Iter begin, Iter end, const allocator_type &alloc = {}) :
            vector(vector_factory(alloc, begin, end)) {}

    template<class _alloc = allocator_type,
             class = std::enable_if_t<std::is_same_v<std::decay_t<_alloc>, new_delete_allocator<T>>>>
    vector(pointer &&ptr, size_type len, const allocator_type &alloc = {}) :
            Allocator(alloc), m_size(len), m_data(ptr) {}

    // END Constructors

    // BEGIN Rule of five
    vector(const vector &other) : vector(begin(other), end(other), other) {}

    vector(vector &&other) :
            vector(std::exchange(other.m_data, nullptr), other.m_size, std::move(other)) {}

    ~vector() {
        if (!m_data) {
            return;
        }

        if constexpr (!std::is_trivially_destructible_v<T>) {
            // Do not need to do this for trivially destructible types.
            for (pointer end = m_data + m_size; end != m_data;) {
                allocator_traits::destroy(*this, --end);
            }
        }
        allocator_traits::deallocate(*this, m_data, m_size);
    }

    auto operator=(const vector &other) -> vector & {
        if (this == std::addressof(other)) {
            return *this;
        }
        assert(m_size == other.m_size);

        std::copy(begin(other), end(other), begin(*this));
        return *this;
    }

    auto operator=(vector &&other) -> vector & {
        if (this == std::addressof(other)) {
            return *this;
        }
        assert(m_size == other.m_size);

        swap(other);
        return *this;
    }
    // END Rule of five

    // BEGIN Element Access
    auto operator[](size_type index) noexcept -> reference {
        return m_data[index];
    }

    auto operator[](size_type index) const noexcept -> const_reference {
        return m_data[index];
    }

    auto at(size_type index) -> reference {
        if (index >= m_size || empty()) {
            std::stringstream mes;
            mes << "Index " << index << " is not small than size " << m_size << '\n';
            throw std::out_of_range{mes.str()};
        }
        return (*this)[index];
    }

    auto at(size_type index) const -> const_reference {
        if (index >= m_size || empty()) {
            std::stringstream mes;
            mes << "Index " << index << " is not small than size " << m_size << '\n';
            throw std::out_of_range{mes.str()};
        }
        return (*this)[index];
    }

    auto front() -> reference {
        return (*this)[0];
    }

    auto front() const -> const_reference {
        return (*this)[0];
    }

    auto back() -> reference {
        return (*this)[m_size - 1];
    }

    auto back() const -> const_reference {
        return (*this)[m_size - 1];
    }

    auto data() noexcept -> pointer {
        return m_data;
    }

    auto data() const noexcept -> const_pointer {
        return m_data;
    }
    // END Element Access

    // BEGIN Capacity
    auto empty() const noexcept -> bool {
        return m_data == nullptr;
    }
    auto size() const noexcept -> size_type {
        return m_size;
    }

    auto get_allocator() const noexcept -> allocator_type {
        return *this;
    }
    // END Capacity

    // BEGIN Iterators
    auto begin() noexcept -> iterator {
        return iterator{m_data};
    }

    auto begin() const noexcept -> const_iterator {
        return std::cbegin(*this);
    }

    auto cbegin() const noexcept -> const_iterator {
        return iterator{m_data};
    }

    auto end() noexcept -> iterator {
        return iterator{m_data + m_size};
    }

    auto end() const noexcept -> const_iterator {
        return std::cend(*this);
    }

    auto cend() const noexcept -> const_iterator {
        return iterator{m_data + m_size};
    }

    auto rbegin() noexcept -> reverse_iterator {
        return iterator{m_data};
    }

    auto rbegin() const noexcept -> const_reverse_iterator {
        return std::crbegin(*this);
    }

    auto crbegin() const noexcept -> const_reverse_iterator {
        return iterator{m_data};
    }

    auto rend() noexcept -> reverse_iterator {
        return reverse_iterator{m_data + m_size};
    }

    auto rend() const noexcept -> const_reverse_iterator {
        return std::crend(*this);
    }

    auto crend() const noexcept -> const_reverse_iterator {
        return reverse_iterator{m_data + m_size};
    }
    // END Iterators

    // BEGIN Modifiers
    void swap(vector &other) noexcept {
        std::swap(static_cast<allocator_type &>(*this), static_cast<allocator_type &>(other));
        std::swap(m_data, other.m_data);
    }

    friend void swap(vector &lhs, vector &rhs) noexcept {
        lhs.swap(rhs);
    }

    void assign(const value_type &v) {
        std::fill(begin(*this), end(*this), v);
    }

    template<class Iter,
             class = std::enable_if_t<std::is_base_of_v<
                 std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>>>
    void assign(Iter begin, Iter end) {
        assert(std::distance(begin, end) == size());

        std::copy(begin, end, begin(*this));
    }

    void assign(std::initializer_list<T> ilist) {
        assign(begin(ilist), end(ilist));
    }

    void assign(pointer &&p) {
        vector(std::move(m_data), m_size, *this);

        m_data = p;
    }

    // Be careful with this, pointer has to be deallocated with same allocator
    [[nodiscard]] pointer release() {
        return std::exchange(m_data, nullptr);
    }

    template<class... Args>
    auto construct_at(iterator pos, Args &&... args) -> value_type & {
        allocator_traits::deallocate(*this, pos);

        construct(pos, std::forward<Args>(args)...);
        return *pos;
    }

    template<class... Args>
    auto construct_at(size_t index, Args &&... args) -> value_type & {
        return construct_at(m_data + index, std::forward<Args>(args)...);
    }
    // END Modifiers

  private:
    const size_type m_size{0};
    pointer m_data;
};

template<class T, class Alloc>
bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return std::equal(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

template<class T, class Alloc>
bool operator!=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return !(lhs == rhs);
}

template<class T, class Alloc>
bool operator<(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return std::lexicographical_compare(begin(lhs), end(lhs), begin(rhs), end(rhs));
}

// I am a bit skeptical regarding the sensibility of this
template<class T, class Alloc>
bool operator<=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return std::lexicographical_compare(
        begin(lhs), end(lhs), begin(rhs), end(rhs), std::less_equal<>{});
}

template<class T, class Alloc>
bool operator>(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return std::lexicographical_compare(begin(lhs), end(lhs), begin(rhs), end(rhs), std::greater<>{});
}

template<class T, class Alloc>
bool operator>=(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs) {
    return std::lexicographical_compare(
        begin(lhs), end(lhs), begin(rhs), end(rhs), std::greater_equal<>{});
}

}   // namespace static_vector

#endif   // STATIC_VECTOR_HPP
