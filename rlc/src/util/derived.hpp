#ifndef RELOG_UTIL_DERIVED_HPP
#define RELOG_UTIL_DERIVED_HPP

#include <type_traits>

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

#endif
