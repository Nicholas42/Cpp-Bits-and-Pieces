#include <iostream>
#include <cassert>
#include "tup_iter.hpp"
#include "any_iter.hpp"
#include "tup_algo.hpp"

using namespace tuple_iter;

// Templated class that evaluates to true iff it is templated on cv char.
template<class Found>
struct StructFinder {
    constexpr explicit operator bool() {
        return std::is_same_v<std::remove_cv_t<Found>, char>;
    }
};

// Templated class that searches for the value of its data member.
template<class Val>
struct ValueFinder {
    template<class T, class = std::enable_if_t<!std::is_same_v<Val, T>>>
    constexpr bool operator()([[maybe_unused]] T &&) {
        return false;
    }

    constexpr bool operator()(const Val &v) {
        return v == searched;
    }

    Val searched;
};

template<class Val>
ValueFinder(Val)->ValueFinder<Val>;

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

    using it_t = find_type_t<StructFinder, begin_t<tup_t>, end_t<tup_t>>;
    it_t it = find_type<StructFinder>(begin(tup), end(tup));
    std::cout << *it << '\n';

    auto any_iter = *find_type_any(begin(tup), a, ValueFinder{1});
    static_assert(std::variant_size_v<decltype(any_iter)> == 2);
    assert(any_iter.index() == 0);

    // Should be type safe
    assert(!find_type_any(begin(tup), end(tup), ValueFinder<int>{'A'}));

    std::tuple numbers{1, 1.4, 3l, -7.123f, 'A'};

    auto sum1 = for_each(begin(numbers), end(numbers), [sum=0.](auto v) mutable { std::cout << v << ", "; return sum += v;});
    auto sum2 = accumulate(begin(numbers), end(numbers));

    assert(sum1(0.) == sum2);
    std::cout << '\n' << sum2 << '\n';
}
