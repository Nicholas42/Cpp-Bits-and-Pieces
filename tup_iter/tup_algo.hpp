#ifndef TUP_ALGO_HPP
#define TUP_ALGO_HPP

#include <tuple>
#include <type_traits>
#include <optional>
#include "any_iter.hpp"

namespace tuple_iter {

// Type based find algorithm, only works on the types. Pred should be a class template that explicitly converts to bool.
template<template<class> class Pred, class Begin, class End>
constexpr auto find_type(Begin begin, End end) noexcept {
    if constexpr (begin == end) {
        return end;
    } else {
        if constexpr (Pred<typename Begin::value_t>()) {
            return begin;
        } else {
            return find_type<Pred>(++begin, end);
        }
    }
}

template<template<class> class Pred, class Begin, class End>
using find_type_t = decltype(find_type<Pred>(std::declval<Begin>(), std::declval<End>()));

// Value based find algorithm, can use both types and values in the tuple. Pred should be an object that can be called with each of the searched types.
template<class Pred, class Begin, class End, class Sequence = span_sequence<Begin, End>>
constexpr auto find_type_any(Begin begin, End end, Pred pred, Sequence seq = {}) noexcept
    -> std::optional<AnyIter<typename Begin::tuple_t, Sequence>> {
    if constexpr (begin == end) {
        return {};
    } else {
        if (pred(*begin)) {
            return {begin};
        } else {
            return find_type_any(++begin, end, pred, seq);
        }
    }
}

// Applies f on each element in the iterated range. f should be an object that is callable on each of the types in the range.
template<class Begin, class End, class Func>
constexpr auto for_each(Begin begin, End end, Func f) noexcept -> Func {
    if constexpr (begin == end) {
        return f;
    } else {
        f(*begin);
        return for_each(++begin, end, f);
    }
}

// Accumulates the iterated range in the form:
//   Op(...Op(Op(v, begin), ++begin), ..., end)
// Op should be an object that can be called appropriately.
template<class Begin, class End, class Val = typename Begin::value_t, class Op = std::plus<>>
constexpr auto accumulate(Begin begin, End end, Val v = {}, Op op = {}) noexcept {
    if constexpr (begin == end) {
        return v;
    } else {
        return accumulate(++begin, end, op(v, *begin), op);
    }
}

}   // namespace tuple_iter
#endif   // TUP_ALGO_HPP
