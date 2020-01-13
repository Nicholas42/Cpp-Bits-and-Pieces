#include "lambda.hpp"
#include "higher_order.hpp"
#include <iostream>

int main() {
    fibonacci l;

    std::cout << l() << '\n';
    std::cout << l() << '\n';
    std::cout << l() << '\n';
    std::cout << l() << '\n';
    std::cout << l() << '\n';

    lazy_evaluation<int, int, int> lazy(add, 5, 6);

    std::cout << lazy() << '\n';

    return 0;
}
