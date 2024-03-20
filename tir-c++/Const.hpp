#pragma once

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

    int value() const;

    std::string label() const override;

    bool isConstant() const override;

    int constant() const override;
}
