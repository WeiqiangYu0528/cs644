// package joosc.ir.ast;
#pragma once
#include "Expr_c.hpp"
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.CheckCanonicalIRVisitor;
// import joosc.ir.visit.IRVisitor;

/**
 * An intermediate representation for an expression evaluated under side effects
 * ESEQ(stmt, expr)
 */
class ESeq : public Expr_c {
private:
    Stmt stmt;
    Expr expr;

public:
    /**
     * @param stmt IR statement to be evaluated for side effects
     * @param expr IR expression to be evaluated after {@code stmt}
     */
    ESeq(Stmt& stmt, Expr& expr);

    ESeq(Stmt& stmt, Expr& expr, bool replaceParent);

    Stmt stmt() const;

    Expr expr() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(stmt));
        result = v.bind(result, v.visit(expr));
        return result;
    }

    bool isCanonical(CheckCanonicalIRVisitor& v) const override;
}
