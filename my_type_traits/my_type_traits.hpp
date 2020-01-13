#ifndef MY_TYPE_TRAITS
#define MY_TYPE_TRAITS

#include <type_traits>

namespace my_type_traits {

// BEGIN is_instantiation_of
// Takes a type C and a template T and checks if C is an instantiation of T. If this is the case,
// is_instantiation_of<C, T>::value is true, otherwise false. There is a helper variable template
// is_instantiation_of_v<C, T> defined. The cv-qualification, references, etc. _are_ taken into account.
//
// Note: Of course one could have arbitrarily awkward types with alternating type and non-type template
// parameters. I would have no idea how to work that out, so I will concentrate on the mose useful case,
// where all parameters to the template are type template parameters.

template<class, template<class...> class>
struct is_instantiation_of : std::false_type {};

template<template<class...> class T, class... Parms>
struct is_instantiation_of<T<Parms...>, T> : std::true_type {};

template<class C, template<class...> class T>
inline constexpr bool is_instantiation_of_v = is_instantiation_of<C, T>::value;
// END is_instantiation_of

}   // namespace my_type_traits

#endif   // MY_TYPE_TRAITS
