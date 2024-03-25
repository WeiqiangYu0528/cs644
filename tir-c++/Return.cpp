#include "Return.hpp"

Return::Return(std::shared_ptr<Expr> ret) : ret(ret) {}

std::shared_ptr<Expr> Return::getRet() const {
    return ret;
}

std::string Return::getLabel() const {
    return "RETURN";
}

std::shared_ptr<Node> Return::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;
    std::shared_ptr<Expr> newExpr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), ret));

    if (newExpr != ret) modified = true;
    std::shared_ptr<Expr> result = newExpr;

    if (modified) {
        return v->nodeFactory()->IRReturn(result);
    }

    return shared_from_this();
}
