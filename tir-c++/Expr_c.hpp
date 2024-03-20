#pragma once
#include "Expr.hpp"
/**
 * An intermediate representation for expressions
 */
class Expr_c : public Node_c, public Expr {
public:
    CheckCanonicalIRVisitor checkCanonicalEnter(
            CheckCanonicalIRVisitor& v) override;

    bool isCanonical(CheckCanonicalIRVisitor& v) const override;

    bool isConstant() const override;

    int constant() const override;
}
