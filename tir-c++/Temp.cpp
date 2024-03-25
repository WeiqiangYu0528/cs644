#include "Temp.hpp"

Temp::Temp(const std::string& name) : name(name) {}

std::string Temp::getName() const {
    return name;
}

std::string Temp::getLabel() const {
    return "TEMP(" + name + ")";
}
