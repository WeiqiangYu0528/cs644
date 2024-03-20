#include "Exp.hpp"

Exp::Exp(Expr& expr) : expr(expr){
}

Expr Exp::expr() const {
    return expr;
}

std::string Exp::label() const {
    return "EXP";
}

Node Exp::visitChildren(IRVisitor& v) {
    Expr expr = (Expr) v.visit(this, this.expr);

    if (expr != this.expr) return v.nodeFactory().IRExp(expr);

    return this;
}

CheckCanonicalIRVisitor Exp::checkCanonicalEnter(
        CheckCanonicalIRVisitor& v) {
    return v.enterExp();
}

