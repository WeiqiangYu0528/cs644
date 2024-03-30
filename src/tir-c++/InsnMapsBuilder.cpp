#include "IRAst.hpp"
#include "InsnMapsBuilder.hpp"
#include <stdexcept>

using namespace TIR;

InsnMapsBuilder::InsnMapsBuilder() : IRVisitor(nullptr), index(0) {
}

std::unordered_map<std::string, int>& InsnMapsBuilder::nameToIndex() {
    return nameToIndexMap;
}

std::unordered_map<int, std::shared_ptr<Node>>& InsnMapsBuilder::indexToInsn() {
    return indexToInsnMap;
}

std::shared_ptr<IRVisitor> InsnMapsBuilder::enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    std::shared_ptr<InsnMapsBuilder> v = n->buildInsnMapsEnter(std::dynamic_pointer_cast<InsnMapsBuilder>(shared_from_this()));
    return v;
}

std::shared_ptr<Node> InsnMapsBuilder::leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, std::shared_ptr<Node> n_, std::shared_ptr<IRVisitor> v_) {
    return n_->buildInsnMaps(std::dynamic_pointer_cast<InsnMapsBuilder>(v_));
}

void InsnMapsBuilder::addInsn(std::shared_ptr<Node> n) {
    indexToInsnMap[index] = n;
    ++index;
}

void InsnMapsBuilder::addNameToCurrentIndex(const std::string& name) {
    if (nameToIndexMap.find(name) != nameToIndexMap.end()) {
        throw std::runtime_error("Error - encountered duplicate name " + name + " in the IR tree -- go fix the generator.");
    }
    nameToIndexMap[name] = index;
}
