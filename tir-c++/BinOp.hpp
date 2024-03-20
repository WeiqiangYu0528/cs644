#pragma once
#include "Expr_c.hpp"
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for a binary operation
 * OP(left, right)
 */

enum class OpType {
    ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
    EQ, NEQ, LT, GT, LEQ, GEQ
};

/**
 * Binary operators
 */
class BinOp : public Expr_c {
private:
    OpType type;
    Expr left, right;

public:
    BinOp(OpType type, Expr& left, Expr& right);

    OpType opType() const;

    Expr left() const;

    Expr right() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(left));
        result = v.bind(result, v.visit(right));
        return result;
    }

    boolean isConstant() const override;
}
