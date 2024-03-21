#include "Expr_c.hpp"

CheckCanonicalIRVisitor Expr_c::checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterExpr();
}

bool Expr_c::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return v->inExpr() || !v->inExp();
}

bool Expr_c::isConstant() const {
    return false;
}

int Expr_c::constant() const {
    throw new UnsupportedOperationException();
}
