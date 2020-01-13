#include <type_traits>
#include "my_type_traits.hpp"

using namespace my_type_traits;

template<class... Args>
struct variadic_t {};

using variadic_instantiation = variadic_t<int, char>;

template<class, class>
struct two_parm_t {};

using two_parm_instantiation = two_parm_t<int, char>;

template<int>
struct non_type_t {};

using non_type_instantiation = non_type_t<5>;

int main() {
    static_assert(is_instantiation_of_v<variadic_instantiation, variadic_t>);
    static_assert(is_instantiation_of_v<two_parm_instantiation, two_parm_t>);
    static_assert(!is_instantiation_of_v<two_parm_instantiation, variadic_t>);
    static_assert(!is_instantiation_of_v<two_parm_instantiation, std::is_same>);
    // static_assert(!is_instantiation_of_v<non_type_instantiation, non_type_t>);  // Sadly, this does
    // not work.
}
