#pragma once

#include <memory>
#include <type_traits>
//package joosc.util;

/**
 * Interface used to copy objects.  Similar to {@code Cloneable}, except that
 * {@code copy()} must be public, not protected as {@code clone()} is.
 */

//abstract virtual class
template <typename T>
class Copy : public Cloneable {
public:
    virtual std::shared_ptr<T> copy() = 0;
    class Util {
    public:        
        template<typename A> 
        typename std::enable_if<std::is_base_of<A, T>::value && std::is_base_of<Copy<A>, T>::value, std::shared_ptr<T>>::type
        copy(std::shared_ptr<T> o);
    };
};