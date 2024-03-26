#include "Jump.hpp"
#include "NodeFactory.hpp"

Jump::Jump(std::shared_ptr<Expr> expr) : target(expr) {
}

std::shared_ptr<Expr> Jump::getTarget() const {
    return target;
}

std::string Jump::getLabel() const {
    return "JUMP";
}

std::shared_ptr<Node> Jump::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));

    if (expr != target) return v->nodeFactory()->IRJump(expr);

    return shared_from_this();
}
