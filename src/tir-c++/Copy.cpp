#include "Copy.hpp"

template<typename T>
template<typename A> 
typename std::enable_if<std::is_base_of<A, T>::value && std::is_base_of<Copy<A>, T>::value, std::shared_ptr<T>>::type
Copy<T>::Util::copy(std::shared_ptr<T> o) {
    return std::dynamic_pointer_cast<T>(o->copy());
}