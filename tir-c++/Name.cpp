#include "Name.hpp"

Name::Name(const std::string& name) : name(name) {}

std::string Name::name() const {
    return name;
}

std::string Name::label() const {
    return "NAME(" + name + ")";
}
