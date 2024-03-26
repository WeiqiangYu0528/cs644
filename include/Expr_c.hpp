#pragma once
#include <memory>
#include "Expr.hpp"

/**
 * An intermediate representation for expressions
 */
class Expr_c : public Expr {
public:
    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) override;

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;

    bool isConstant() const override;

    int getConstant() const override;
};
