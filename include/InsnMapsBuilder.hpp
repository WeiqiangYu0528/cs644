#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "IRVisitor.hpp"

class InsnMapsBuilder : public IRVisitor {
public:
    InsnMapsBuilder();

    std::unordered_map<std::string, int>& nameToIndex();
    std::unordered_map<int, std::shared_ptr<Node>>& indexToInsn();

    virtual std::shared_ptr<IRVisitor> enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) override;
    virtual std::shared_ptr<Node> leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, std::shared_ptr<Node> n_, std::shared_ptr<IRVisitor> v_) override;

    void addInsn(std::shared_ptr<Node> n);
    void addNameToCurrentIndex(const std::string& name);

private:
    std::unordered_map<std::string, int> nameToIndexMap;
    std::unordered_map<int, std::shared_ptr<Node>> indexToInsnMap;
    int index;
};
