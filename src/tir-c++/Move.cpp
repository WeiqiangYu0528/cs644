#include "Move.hpp"

Move::Move(std::shared_ptr<Expr> target, std::shared_ptr<Expr> src, bool trash) : target(target), src(src) {}

std::shared_ptr<Expr> Move::getTarget() const {
    return target;
}

std::shared_ptr<Expr> Move::getSource() const {
    return src;
}

std::string Move::getLabel() const {
    return "MOVE";
}

std::shared_ptr<Node> Move::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> newTarget = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));
    std::shared_ptr<Expr> newSrc = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), src));

    if (newTarget != target || newSrc != src)
        return v->nodeFactory()->IRMove(newTarget, newSrc);

    return shared_from_this();
}
