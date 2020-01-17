#include <iostream>
#include <cassert>
#include "tup_iter.hpp"
#include "any_iter.hpp"

using namespace tuple_iter;

int main() {
    std::tuple<int, const char, double> tup{1, 'A', 2.1};
    using tup_t = decltype(tup);

    end_t<tup_t> e{tup};
    auto a = --e;
    auto b = ++++begin(tup);
    TupleIter iter(tup, index_tag<1>);

    static_assert(e.index() == 3);
    static_assert(std::is_same_v<decltype(a), decltype(b)>);
    static_assert(std::is_same_v<decltype(a)::value_t, double>);
    static_assert(std::is_same_v<begin_t<tup_t>::value_t, std::tuple_element_t<0, tup_t>>);
    static_assert(distance_v<begin_t<tup_t>, end_t<tup_t>> == 3);
    static_assert(iter.index() == 1);
    static_assert(std::is_same_v<begin_t<std::tuple<>>, end_t<std::tuple<>>>);

    // decltype(e)::value_t i = 5; // Good: Does not compile
    assert(*a == 2.1);

    auto any = make_any_iter(iter);
    auto any2 = make_any_iter(tup, index_tag<2>, std::index_sequence<2>{});
    assert(any.index() == 1);
    assert(any2.index() == 0);

    std::visit(
        [](auto &&first, auto &&second) { std::cout << *first << ' ' << *second << '\n'; }, any, any2);

    const char value{std::get<1>(any)};
    assert(value == std::get<1>(tup));
    static_assert(span_sequence<decltype(a), end_t<tup_t>>::size() == 1);
    static_assert(span_sequence<end_t<tup_t>, end_t<tup_t>>::size() == 0);
}
