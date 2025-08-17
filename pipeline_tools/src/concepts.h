#pragma once
#include <type_traits>

template<typename T>
concept ImplicitlyDefaultConstructible = std::__is_implicitly_default_constructible<T>::value;
