#ifndef ANY_ITER_HPP
#define ANY_ITER_HPP

#include "tup_iter.hpp"
#include <type_traits>
#include <variant>

namespace tuple_iter {

// I decided to directly use a variant for this type and not wrap it in another class since this makes
// the integration in existing visitor code much easier.

namespace detail {
template<class Tuple, class T>
struct helper_t;

template<class Tuple, size_t... Indices>
struct helper_t<Tuple, std::index_sequence<Indices...>> {
    using type = std::variant<TupleIter<Tuple, Indices>...>;
};

template<class Tuple>
using default_index_sequence = std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>;

template<class Begin, class End, size_t... Indices>
struct span_sequence_impl :
        span_sequence_impl<decltype(++std::declval<Begin>()), End, Indices..., Begin::index()> {};

template<class Iter, size_t... Indices>
struct span_sequence_impl<Iter, Iter, Indices...> {
    using sequence = std::index_sequence<Indices...>;
};
}   // namespace detail

// Not including the past-the-end iterator per default. It is easier to handle it with std::optional in
// application code and do not have to handle it in visitors, since contrary to the others, it does not
// have the same methods. It is possible to provide an own sequence of indices that should be considered
template<class Tuple, class index_seq = detail::default_index_sequence<Tuple>>
using AnyIter = typename detail::helper_t<Tuple, index_seq>::type;

template<class Begin, class End>
using span_sequence =
    typename detail::span_sequence_impl<std::decay_t<Begin>, std::decay_t<End>>::sequence;

template<class TupleIter, class index_seq = detail::default_index_sequence<tuple_t<TupleIter>>,
         class = std::enable_if_t<is_tuple_iter_v<TupleIter>>>
constexpr auto make_any_iter(TupleIter &&it, index_seq /*unused*/ = {})
    -> AnyIter<tuple_t<TupleIter>, index_seq> {
    return AnyIter<tuple_t<TupleIter>, index_seq>{std::forward<TupleIter>(it)};
}

template<class Tuple, size_t N, class index_seq = detail::default_index_sequence<Tuple>,
         class = std::tuple_element<N, Tuple>>
constexpr auto make_any_iter(Tuple &&tup, std::integral_constant<size_t, N> /*unused*/ = {},
                             index_seq /*unused*/ = {}) -> AnyIter<Tuple, index_seq> {
    return AnyIter<Tuple, index_seq>{TupleIter<Tuple, N>{std::forward<Tuple>(tup)}};
}
}   // namespace tuple_iter

#endif   // ANY_ITER_HPP
