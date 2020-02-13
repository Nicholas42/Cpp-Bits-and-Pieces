#include "static_vector.hpp"
using namespace static_vector;

struct test {
    test() = delete;
    test(int) {}
};

int main() {
    vector<int> v(10,0);
    vector<int> w(std::begin(v), std::end(v));

    int* i = new int[10];


    vector<int, new_delete_allocator<int>> z(std::move(i), 10);
}
    
