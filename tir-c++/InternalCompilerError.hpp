#pragma once
#include <string>
//package joosc.util;

/** Exception thrown when the compiler is confused. */
class InternalCompilerError : public RuntimeException {
public:
    InternalCompilerError(const std::string& msg) : RuntimeException(msg) {}
    InternalCompilerError(Throwable cause) : RuntimeException(cause.getMessage(), cause) {}
    InternalCompilerError(const std::string& msg, Throwable cause) : RuntimeException(msg, cause) {}
};