#pragma once
#include <memory>
#include "Expr.hpp"
#include "Stmt.hpp"
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

    std::shared_ptr<Expr> getExpr() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(expr));
        return result;
    }

    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) override;
};
