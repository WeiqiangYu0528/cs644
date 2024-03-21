#pragma once
#include "Expr.hpp"
/**
 * An intermediate representation for expressions
 */
class Expr_c : public Node_c, public Expr {
public:
    CheckCanonicalIRVisitor checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) override;

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;

    bool isConstant() const override;

    int constant() const override;
}
