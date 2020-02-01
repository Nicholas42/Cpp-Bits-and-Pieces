#include "zip.hpp"
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>

using namespace zip_iter;

int main() {
    std::vector<int> nums{1, 2, 3, 4};
    std::set<std::string> strings{"one", "two", "three"};
    std::map<int, double> other{{1, 1.5}, {2, 2.5}, {3, 3.5}, {4, 4.5}};

    zip z(std::pair(begin(nums), begin(nums) + 1), std::pair(++begin(strings), ++++begin(strings)));

    std::cout << "Setting a value\n";
    for (auto it: z) {
        std::cout << std::get<0>(it) << ", " << std::get<1>(it) << '\n';
        std::get<0>(it) = 0;
    }

    std::cout << "Three containers\n";
    for (auto [n, s, o]: zip(nums, strings, other)) {
        std::cout << n << ", " << s << ", " << o.second << '\n';
    }

    std::cout << "Iterating with index\n";

    for (auto [i, v]: with_index(strings)) {
        std::cout << i << ": " << v << '\n';
    }
}
