#include <iostream>
#include <algorithm>
#include <functional>
#include "struct_iter.hpp"

struct boundary {
    int top;
    int bottom;
    int left;
    int right;

    using iter =
        struct_it<boundary, int, &boundary::top, &boundary::bottom, &boundary::left, &boundary::right>;
    using citer =
        cstruct_it<boundary, int, &boundary::top, &boundary::bottom, &boundary::left, &boundary::right>;

    iter begin() {
        return iter{*this};
    }

    iter end() {
        return iter::make_end(*this);
    }

    citer cbegin() const {
        return citer{*this};
    }

    citer cend() const {
        return citer::make_end(*this);
    }
};

int main() {
    boundary b{1, 2, 3, 4};

    for (auto i: b) {
        std::cout << i << ' ';   // Prints 1 2 3 4
    }
    std::cout << '\n';

    std::sort(std::begin(b), std::end(b), std::greater<>{});

    for (auto i: b) {
        std::cout << i << ' ';   // Prints 1 2 3 4
    }
    std::cout << '\n';
}
