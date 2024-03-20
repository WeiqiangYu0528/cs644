#pragma once
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.CheckCanonicalIRVisitor;
// import joosc.ir.visit.IRVisitor;
#include "Expr.hpp"
/**
 * An intermediate representation for evaluating an expression for side effects,
 * discarding the result
 * EXP(e)
 */
class Exp : public Stmt {
private:
    Expr expr;

public:
    /**
     *
     * @param expr the expression to be evaluated and result discarded
     */
    Exp(Expr& expr);

    Expr expr() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(expr));
        return result;
    }

    CheckCanonicalIRVisitor checkCanonicalEnter(
            CheckCanonicalIRVisitor& v) override;
}
