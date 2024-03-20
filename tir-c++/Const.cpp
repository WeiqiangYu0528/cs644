#include "Const.hpp"

Const::Const(int value) : value(value) {
}

int Const::value() const {
    return value;
}

std::string Const::label() const {
    return "CONST(" + value + ")";
}

bool Const::isConstant() const {
    return true;
}

int Const::constant() const {
    return value;
}
