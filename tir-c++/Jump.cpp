#include "Jump.hpp"

Jump::Jump(Expr& expr) : target(expr) {
}

Expr Jump::target() const {
    return target;
}

std::string Jump::label() const {
    return "JUMP";
}

Node Jump::visitChildren(IRVisitor& v) {
    Expr expr = (Expr) v.visit(this, target);

    if (expr != target) return v.nodeFactory().IRJump(expr);

    return this;
}
