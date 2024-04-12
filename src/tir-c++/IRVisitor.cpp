#include "IRAst.hpp"
#include "IRVisitor.hpp"

using namespace TIR;

IRVisitor::IRVisitor(std::shared_ptr<NodeFactory> inf) : inf(inf) {}

std::shared_ptr<NodeFactory> IRVisitor::nodeFactory() {
    return inf;
}

std::shared_ptr<Node> IRVisitor::visit(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    if (!n) return nullptr;

    auto overrideValue = override(parent, n);
    if (overrideValue) return overrideValue;

    auto v_ = enter(parent, n);
    if (!v_) throw std::runtime_error("IRVisitor::enter() returned null!");

    auto n_ = n->visitChildren(v_);
    if (!n_) throw std::runtime_error("IRVisitor::visitChildren() returned null!");

    n_ = leave(parent, n, n_, v_);

    if (!n_) throw std::runtime_error("IRVisitor::leave() returned null!");

    return n_;
}

std::shared_ptr<Node> IRVisitor::visit(std::shared_ptr<Node> node) {
    return visit(nullptr, node);
}

std::shared_ptr<Node> IRVisitor::override(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    return nullptr;
}

std::shared_ptr<IRVisitor> IRVisitor::enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    return shared_from_this();
}

std::shared_ptr<Node> IRVisitor::leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, std::shared_ptr<Node> n_, std::shared_ptr<IRVisitor> v_) {
    return n_;
}
