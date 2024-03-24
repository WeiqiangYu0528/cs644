#pragma once
#include "Stmt.hpp"
#include "Expr.hpp"
#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"

class Return : public Stmt, std::enable_shared_from_this<Return> {
protected:
    std::shared_ptr<Expr> ret;

public:
    explicit Return(std::shared_ptr<Expr> ret);

    virtual ~Return();

    std::shared_ptr<Expr> getRet() const;

    virtual std::string getLabel() const override;

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v);
    template<typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v);
};