#include "Expr_c.hpp"

CheckCanonicalIRVisitor Expr_c::checkCanonicalEnter(
            CheckCanonicalIRVisitor& v) {
    return v.enterExpr();
}

bool Expr_c::isCanonical(CheckCanonicalIRVisitor& v) const {
    return v.inExpr() || !v.inExp();
}

bool Expr_c::isConstant() const {
    return false;
}

int Expr_c::constant() const {
    throw new UnsupportedOperationException();
}
