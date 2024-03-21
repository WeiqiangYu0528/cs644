#include "Exp.hpp"

Exp::Exp(std::shared_ptr<Expr> expr) : expr(expr){
}

std::shared_ptr<Expr> Exp::expr() const {
    return expr;
}

std::string Exp::label() const {
    return "EXP";
}

std::shared_ptr<Node> Exp::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = (Expr) v.visit(this, this.expr);

    if (expr != this.expr) return v.nodeFactory().IRExp(expr);

    return this;
}

CheckCanonicalIRVisitor Exp::checkCanonicalEnter(
        std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterExp();
}
