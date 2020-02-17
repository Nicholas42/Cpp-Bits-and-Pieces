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

// Allocating old-style function
int* create_array(size_t s) {
    return new int[s];
}

void use_array(int *, size_t ) {
    // Do something...
}

void destroy_array(int * ptr) {
    delete[] ptr;
}

int main() {
    vector<int> v(10, 0);
    vector<int> w(std::begin(v), std::end(v));

    int *i = new int[10];

    vector<int, delete_allocator<int>> z(std::move(i), 10);

    v = std::move(w);

    vector<int, delete_allocator<int>> a(create_array(20), 20);
    use_array(a.data(), a.size());
    destroy_array(a.release());


    try {
        // Should not leak
        vector<throwing> t(10);
    } catch (...) {
    }
}
