// package joosc.ir.ast;
#pragma once
#include <memory>
#include <string>
#include "Expr.hpp"

/**
 * An intermediate representation for a conditional transfer of control
 * CJUMP(expr, trueLabel, falseLabel)
 */
class CJump : public Stmt, public std::enable_shared_from_this<CJump> {
private:
    std::shared_ptr<Expr> cond;
    std::string trueLabel, falseLabel;

public:
    /**
     * Construct a CJUMP instruction with fall-through on false.
     * @param cond the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     */
    CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel);
    /**
     *
     * @param cond the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     * @param falseLabel the destination of the jump if {@code expr} evaluates
     *          to false
     */
    CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel, const std::string& falseLabel);

    std::shared_ptr<Expr> getCond() const;

    std::string getTrueLabel() const;

    std::string getFalseLabel() const;

    bool hasFalseLabel() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) override {
        T result = v->unit();
        result = v->bind(result, v->visit(cond));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
}
