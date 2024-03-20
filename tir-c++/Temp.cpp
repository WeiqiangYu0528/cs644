#include "Temp.hpp"
#include <string>

Temp::Temp(const std::string& name) : name(name) {}

std::string Temp::name() const {
    return name;
}

std::string Temp::label() const {
    return "TEMP(" + name + ")";
}