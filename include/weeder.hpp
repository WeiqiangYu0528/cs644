#pragma once

#include <vector>
#include <string>

const std::vector<std::string> keywords {
    "assert",
    "break",
    "case",
    "catch",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "enum",
    "finally",
    "float",
    "goto",
    "long",
    "private",
    "strictfp",
    "super",
    "switch",
    "synchronized",
    "throw",
    "throws",
    "transient",
    "try",
    "volatile",
};

enum class Modifiers {
    PUBLIC = 0,
    PROTECTED,
    FINAL,
    ABSTRACT,
    STATIC,
    NATIVE
};

enum class MemberType {
    FIELD = 0,
    METHODWITHOUTBODY,
    METHODWITHBODY,
    CONSTRUCTOR
};

enum class DataType {
    VOID = 0,
    INT,
    BOOLEAN,
    CHAR,
    BYTE,
    SHORT,
    LONG,
    FLOAT,
    DOUBLE,
    ARRAY,
    OBJECT,
    VOIDARRAY,
    INTARRAY,
    BOOLEANARRAY,
    CHARARRAY,
    BYTEARRAY,
    SHORTARRAY,
    LONGARRAY,
    FLOATARRAY,
    DOUBLEARRAY,
    OBJECTARRAY
};

inline bool validateModifier(std::vector<int>& vec, Modifiers token) {
    switch (token) {
        case Modifiers::PUBLIC: {vec[0]++; break;};
        case Modifiers::PROTECTED: {vec[1]++; break;}
        case Modifiers::ABSTRACT: {vec[2]++; break;}
        case Modifiers::FINAL: {vec[3]++; break;}
        case Modifiers::STATIC: {vec[4]++; break;}
        case Modifiers::NATIVE: {vec[5]++; break;}
        default: return false;
    }
    return true;
}

enum class Expression {
    LOCAL = 0,
    NON_STATIC_FIELD,
    STATIC_FIELD
};
