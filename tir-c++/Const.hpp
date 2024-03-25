#pragma once
#include "Expr_c.hpp"

/**
 * An intermediate representation for a 64-bit integer constant.
 * CONST(n)
 */
class Const : public Expr_c {
private:
    int value;

public:
    /**
     *
     * @param value value of this constant
     */
    Const(int value);

    int getValue() const;

    std::string getLabel() const override;

    bool isConstant() const override;

    int getConstant() const override;
}
