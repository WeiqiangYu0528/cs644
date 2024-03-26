#include "Copy.hpp"

template<class T>
template<typename A>
typename std::enable_if<std::is_base_of<A, T>::value && std::is_base_of<Copy<A>, T>::value, T>::type
Copy<T>::Util::copy(T o) {
    return static_cast<T>(o.copy());
}