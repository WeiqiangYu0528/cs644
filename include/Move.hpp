#pragma once
#include "Stmt.hpp"
#include "Expr.hpp"

class Move : public Stmt {
private:
    std::shared_ptr<Expr> target;
    std::shared_ptr<Expr> src;

public:
    Move(std::shared_ptr<Expr> target, std::shared_ptr<Expr> src, bool trash = false);

    std::shared_ptr<Expr> getTarget() const;

    std::shared_ptr<Expr> getSource() const;

    virtual std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(target));
        result = v->bind(result, v->visit(src));
        return result;
    }


    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};
