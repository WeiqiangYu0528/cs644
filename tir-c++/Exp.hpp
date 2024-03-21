#pragma once
#include <memory>
#include "Expr.hpp"
/**
 * An intermediate representation for evaluating an expression for side effects,
 * discarding the result
 * EXP(e)
 */
class Exp : public Stmt {
private:
    std::shared_ptr<Expr> expr;

public:
    /**
     *
     * @param expr the expression to be evaluated and result discarded
     */
    Exp(std::shared_ptr<Expr> expr);

    std::shared_ptr<Expr> expr() const;

    std::string label() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(expr));
        return result;
    }

    CheckCanonicalIRVisitor checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) override;
}
