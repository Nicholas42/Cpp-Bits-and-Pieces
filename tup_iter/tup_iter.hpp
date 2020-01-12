#ifndef TUP_ITER_HPP
#define TUP_ITER_HPP

#include <type_traits>
#include <iterator>
#include <tuple>
#include <variant>

namespace tuple_iter {

// Tag objects to enable deduction of index
template<size_t Index>
inline constexpr std::integral_constant<size_t, Index> index_tag;

template<class Tup, size_t Index>
struct TupleIter {
    using tuple_t = Tup;

    TupleIter(tuple_t &t) : tup(t) {}

    TupleIter(tuple_t &t, std::integral_constant<size_t, Index>) : tup(t) {}

    // BEGIN - Static Method Section

    static constexpr auto size() noexcept -> size_t {
        return std::tuple_size_v<std::decay_t<tuple_t>>;
    }

    static constexpr auto index() noexcept -> size_t {
        return Index;
    }

    template<class Inp>
    static constexpr decltype(auto) get(Inp &&inp) {
        if constexpr (Index < size()) {
            return std::get<Index>(inp);
        } else {
            // Improves error messages.
            static_assert(Index < size(), "Enditerator is not dereferencable.");
            return 0;
        }
    }

    // END - Static Method Section

  private:
    tuple_t &tup;

    // Helper for value_t since we cannot specialize a type alias directly

    // value_t for the past-the-end iterator
    template<size_t I = Index, class = void>
    struct value_t_struct {
        struct incomplete;
        using type = incomplete;   // Wanted to take void, but then references do not compile even if in
                                   // SFINAE-deactivated functions.
    };

    // value_t for all dereferencable iterators
    template<size_t I>
    struct value_t_struct<I, std::enable_if_t<(I < size())>> {
        using type = std::tuple_element_t<I, tuple_t>;
    };

  public:
    // Return value of dereferencing operator
    using value_t = typename value_t_struct<>::type;

    // Comparison methods. Compare equal to objects of the same type (i.e. have same index are over same
    // tuple type)

    constexpr bool operator==([[maybe_unused]] const TupleIter<tuple_t, Index> &other) const noexcept {
        return true;
    }

    template<size_t I, class = std::enable_if_t<I != Index>>
    constexpr bool operator==([[maybe_unused]] const TupleIter<tuple_t, I> &other) const noexcept {
        return false;
    }

    template<size_t I>
    constexpr bool operator!=(const TupleIter<tuple_t, I> &other) const noexcept {
        return !(*this == other);
    }

    // Canonical way to convert to bool, false iff past-the-end-iterator.

    constexpr explicit operator bool() const noexcept {
        return Index < size();
    }

    // These seem a bit weird since they are const de-/increment operators. But we cannot implement
    // operator+(int inc) as one would normally do it, since inc had to be a constant expression. So
    // this seems like the best way to do this. Furthermore it is actually similar to normal iterators,
    // since for them the following would be equivalent:
    //      ++it;       AND       it = ++it;
    // So reassigning the return value is not that weird.

    template<size_t I = Index, class = std::enable_if_t<(0 < I)>>
    [[nodiscard]] constexpr auto operator--() const noexcept -> TupleIter<tuple_t, Index - 1> {
        return {tup};
    }

    template<size_t I = Index, class = std::enable_if_t<(I < size())>>
    [[nodiscard]] constexpr auto operator++() const noexcept -> TupleIter<tuple_t, Index + 1> {
        return {tup};
    }

    template<std::ptrdiff_t N>
    [[nodiscard]] constexpr auto advance() const noexcept -> TupleIter<tuple_t, Index + N> {
        return {tup};
    }

    template<size_t I = Index, class = std::enable_if_t<(I < size())>>
    constexpr auto operator*() noexcept -> value_t & {
        return std::get<Index>(tup);
    }

    template<size_t I = Index, class = std::enable_if_t<(I < size())>>
    constexpr auto operator*() const noexcept -> const value_t & {
        return std::get<Index>(tup);
    }
};

template<size_t Index, class Tuple>
TupleIter(Tuple, std::integral_constant<size_t, Index>) -> TupleIter<Tuple, Index>;

template<std::ptrdiff_t N, std::size_t Index, class Tup>
constexpr auto advance(const TupleIter<Tup, Index> &it) -> TupleIter<Tup, Index + N> {
    return it.template advance<N>();
}

// Easier to use in constant expressions
template<class TupIter1, class TupIter2>
constexpr size_t distance_v = TupIter2::index() - TupIter1::index();

// Performs template argument deduction
template<class TupIter1, class TupIter2>
constexpr auto distance([[maybe_unused]] const TupIter1 &it1,
                        [[maybe_unused]] const TupIter2 &it2) {
    return distance_v<TupIter1, TupIter2>;
}

template<class T>
using begin_t = TupleIter<T, 0>;

template<class T>
using end_t = TupleIter<T, std::tuple_size_v<std::decay_t<T>>>;

template<class T>
constexpr auto begin([[maybe_unused]] T &tup) noexcept -> begin_t<T> {
    return {tup};
}

template<class T>
constexpr auto end([[maybe_unused]] T &tup) noexcept -> end_t<T> {
    return {tup};
}
}   // namespace tuple_iter

#endif   // TUP_ITER_HPP
