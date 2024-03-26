#include "Expr_c.hpp"

std::shared_ptr<CheckCanonicalIRVisitor> Expr_c::checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterExpr();
}

bool Expr_c::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return v->inExpr() || !v->inExp();
}

bool Expr_c::isConstant() const {
    return false;
}

int Expr_c::getConstant() const {
    throw std::runtime_error("Unsupported Operation Exception");
}
