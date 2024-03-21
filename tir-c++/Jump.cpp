#include "Jump.hpp"

Jump::Jump(std::shared_ptr<Expr> expr) : target(expr) {
}

std::shared_ptr<Expr> Jump::target() const {
    return target;
}

std::string Jump::label() const {
    return "JUMP";
}

std::shared_ptr<Node> Jump::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = (Expr) v.visit(this, target);

    if (expr != target) return v->nodeFactory().IRJump(expr);

    return this;
}
