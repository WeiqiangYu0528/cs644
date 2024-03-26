#pragma once
#include "Stmt.hpp"
#include "Expr.hpp"

class Return : public Stmt {
protected:
    std::shared_ptr<Expr> ret;

public:
    explicit Return(std::shared_ptr<Expr> ret);

    std::shared_ptr<Expr> getRet() const;

    virtual std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        result = v->bind(result, v->visit(ret));
        return result;
    }

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};
