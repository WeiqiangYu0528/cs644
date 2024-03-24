#pragma once
#include "Expr_c.hpp"
#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"
#include "Node.hpp"

class Mem : public Expr_c, std::enable_shared_from_this<Mem>{
private:
    std::shared_ptr<Expr_c> expr;

public:
    explicit Mem(std::shared_ptr<Expr_c> expr);

    virtual ~Mem();

    std::shared_ptr<Expr_c> getExpr() const;

    virtual std::string getLabel() const override;

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
    template<typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) override;
};
