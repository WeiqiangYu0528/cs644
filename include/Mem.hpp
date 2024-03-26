#pragma once
#include "Expr_c.hpp"

class Mem : public Expr_c {
private:
    std::shared_ptr<Expr> expr;

public:
    explicit Mem(std::shared_ptr<Expr> expr);

    std::shared_ptr<Expr_c> getExpr() const;

    virtual std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(expr));
        return result;
    }

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};
