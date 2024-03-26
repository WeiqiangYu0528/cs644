#include "Label.hpp"

Label::Label(const std::string& name) : name(name) {
}

std::string Label::getName() const {
    return name;
}

std::string Label::getLabel() const {
    return "LABEL(" + name + ")";
}

std::shared_ptr<InsnMapsBuilder> Label::buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) {
    v->addNameToCurrentIndex(name);
    return v;
}
