#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <memory>
#include "optional_inserter.hpp"

int main()
{
    std::vector<int> vec = {1, 2, 3, 4, 5, 6};
    std::vector<int> vec2;

    std::transform(cbegin(vec), cend(vec), optional_inserter(vec2, end(vec2)), [](int i) {
        return i % 2 == 0 ? i : std::optional<int>{};
    });

    for (auto i: vec2)
    {
        std::cout << i << ' ';
    }

    std::cout << '\n';

    // Testing a move-only type
    using ptr = std::unique_ptr<unsigned long, std::default_delete<unsigned long>>;
    std::vector<ptr> ptr_vec;
    std::vector<ptr> ptr_out;

    for (auto i = 0ul; i < 10; ++i)
    {
        ptr_vec.emplace_back(new unsigned long{i});
    }

    std::transform(std::move_iterator(begin(ptr_vec)),
                   std::move_iterator(end(ptr_vec)),
                   optional_inserter(ptr_out, end(ptr_out)),
                   [](ptr &&i) {
                       return *i % 3 == 0 ? std::optional{std::move(i)}
                                          : std::optional<ptr>{};
                   });

    for (auto &i: ptr_out)
    {
        std::cout << *i << " ";
    }

    std::cout << '\n';
    return 0;
}
