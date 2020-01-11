#ifndef STRUCT_ITER_HPP
#define STRUCT_ITER_HPP

#include <map>
template<class C, typename T, T C::*... members>
class struct_it;

template<class C, typename T, T C::*... members>
class struct_it {
  public:
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::random_access_iterator_tag;

    constexpr struct_it(C &c) : _index{0}, _c(std::addressof(c)) {}

    constexpr struct_it(size_t index, C &c) : _index{index}, _c(std::addressof(c)) {}

    constexpr static struct_it<C, T, members...> make_end(C &c) {
        return struct_it<C, T, members...>(sizeof...(members), c);
    }
    // Does not check for other._c == _c, since that is not always
    // possible. Maybe do &other._c == &_c?
    constexpr bool operator==(const struct_it &other) const {
        return other._index == _index;
    }

    constexpr bool operator!=(const struct_it &other) const {
        return !(other == *this);
    }

    constexpr const T &operator*() const {
        return _c->*_members[_index];
    }

    constexpr const T *operator->() const {
        return &(_c->*_members[_index]);
    }

    constexpr const T &operator[](difference_type n) const {
        return &(_c->*_members[_index + n]);
    }

    constexpr T &operator*() {
        return _c->*_members[_index];
    }

    constexpr T *operator->() {
        return &(_c->*_members[_index]);
    }

    constexpr T &operator[](difference_type n) {
        return &(_c->*_members[_index + n]);
    }

    constexpr struct_it &operator--() {
        --_index;
        return *this;
    }

    constexpr struct_it &operator--(int) {
        auto copy = *this;
        --_index;
        return copy;
    }
    constexpr struct_it &operator++() {
        ++_index;
        return *this;
    }

    constexpr struct_it &operator++(int) {
        auto copy = *this;
        ++_index;
        return copy;
    }

    constexpr struct_it &operator+=(difference_type n) {
        _index += n;
        return *this;
    }

    constexpr struct_it &operator-=(difference_type n) {
        _index -= n;
        return *this;
    }

    friend struct_it<C, T, members...>
        operator+(const struct_it<C, T, members...> &s,
                  typename struct_it<C, T, members...>::difference_type n) {
        auto ret = s;
        return ret += n;
    }

    friend struct_it<C, T, members...>
        operator-(const struct_it<C, T, members...> &s,
                  typename struct_it<C, T, members...>::difference_type n) {
        auto ret = s;
        return ret -= n;
    }

    friend bool operator<(const struct_it<C, T, members...> &s, const struct_it<C, T, members...> &t) {
        return s._index < t._index;
    }

    friend bool operator>(const struct_it<C, T, members...> &s, const struct_it<C, T, members...> &t) {
        return s._index < t._index;
    }

    friend bool operator<=(const struct_it<C, T, members...> &s, const struct_it<C, T, members...> &t) {
        return s._index <= t._index;
    }

    friend bool operator>=(const struct_it<C, T, members...> &s, const struct_it<C, T, members...> &t) {
        return s._index >= t._index;
    }

    friend typename struct_it<C, T, members...>::difference_type
        operator-(const struct_it<C, T, members...> &s, const struct_it<C, T, members...> &t) {
        return s._index - t._index;
    }

  private:
    size_type _index;
    C *_c;
    constexpr static std::array<T C::*, sizeof...(members)> _members = {members...};
};
template<class C, typename T, T C::*... members>
using cstruct_it = struct_it<const C, T, members...>;

#endif   // STRUCT_ITER_HPP
