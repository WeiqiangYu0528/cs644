// package joosc.ir.ast;
#pragma once
#include <string>
#include "Expr.hpp"
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.CheckCanonicalIRVisitor;
// import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for a conditional transfer of control
 * CJUMP(expr, trueLabel, falseLabel)
 */
class CJump : public Stmt {
private:
    Expr cond;
    std::string trueLabel, falseLabel;

public:
    /**
     * Construct a CJUMP instruction with fall-through on false.
     * @param cond the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     */
    CJump(Expr& cond, const std::string& trueLabel);
    /**
     *
     * @param cond the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     * @param falseLabel the destination of the jump if {@code expr} evaluates
     *          to false
     */
    CJump(Expr& cond, std::string& trueLabel, std::string& falseLabel);

    Expr cond() const;

    std::string trueLabel() const;

    std::string falseLabel() const;

    bool hasFalseLabel() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(cond));
        return result;
    }

    bool isCanonical(CheckCanonicalIRVisitor& v) const override;
}
