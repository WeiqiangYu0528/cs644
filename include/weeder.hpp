#pragma once

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
