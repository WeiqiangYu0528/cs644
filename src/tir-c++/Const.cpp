#include "Const.hpp"

Const::Const(int value) : value(value) {
}

int Const::getValue() const {
    return value;
}

std::string Const::getLabel() const {
    return "CONST(" + std::to_string(value) + ")";
}

bool Const::isConstant() const {
    return true;
}

int Const::getConstant() const {
    return value;
}
