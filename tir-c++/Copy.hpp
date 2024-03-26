#pragma once

//package joosc.util;

/**
 * Interface used to copy objects.  Similar to {@code Cloneable}, except that
 * {@code copy()} must be public, not protected as {@code clone()} is.
 */

//abstract virtual class
#include <type_traits>
template <class T>
class Copy : public Cloneable {
public:
    T copy();
    class Util {
    public:        
        template<typename A> 
        typename std::enable_if<std::is_base_of<A, T>::value && std::is_base_of<Copy<A>, T>::value, T>::type
        copy(T o) {
            return static_cast<T>(o.copy());
        }
    };
};
