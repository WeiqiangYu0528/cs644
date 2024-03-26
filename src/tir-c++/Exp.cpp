#include "Exp.hpp"
#include "NodeFactory.hpp"

Exp::Exp(std::shared_ptr<Expr> expr) : expr(expr) {
}

std::shared_ptr<Expr> Exp::getExpr() const {
    return expr;
}

std::string Exp::getLabel() const {
    return "EXP";
}

std::shared_ptr<Node> Exp::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> e = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), expr));

    if (e != expr) return v->nodeFactory()->IRExp(e);

    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> Exp::checkCanonicalEnter(
        std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterExp();
}
