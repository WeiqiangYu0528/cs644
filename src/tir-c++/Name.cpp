#include "Name.hpp"

Name::Name(const std::string& name) : name(name) {}

std::string Name::getName() const {
    return name;
}

std::string Name::getLabel() const {
    return "NAME(" + name + ")";
}
