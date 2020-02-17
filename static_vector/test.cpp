#include <exception>
#include "static_vector.hpp"
using namespace static_vector;

struct test {
    test() = delete;
    test(int) {}
};

struct throwing {
    throwing() {
        throw std::runtime_error("Testing");
    }
};

int main() {
    vector<int> v(10, 0);
    vector<int> w(std::begin(v), std::end(v));

    int *i = new int[10];

    vector<int, delete_allocator<int>> z(std::move(i), 10);

    try {
        // Should not leak
        vector<throwing> t(10);
    } catch (...) {
    }
}
