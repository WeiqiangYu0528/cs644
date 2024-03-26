// package joosc.ir.ast;
#pragma once
#include <memory>
#include "Expr_c.hpp"
#include "Stmt.hpp"

/**
 * An intermediate representation for an expression evaluated under side effects
 * ESEQ(stmt, expr)
 */
class ESeq : public Expr_c {
private:
    std::shared_ptr<Stmt> stmt;
    std::shared_ptr<Expr> expr;

public:
    /**
     * @param stmt IR statement to be evaluated for side effects
     * @param expr IR expression to be evaluated after {@code stmt}
     */
    ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr);

    ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr, bool replaceParent);

    std::shared_ptr<Stmt> getStmt() const;

    std::shared_ptr<Expr> getExpr() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(stmt));
        result = v->bind(result, v->visit(expr));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
};
