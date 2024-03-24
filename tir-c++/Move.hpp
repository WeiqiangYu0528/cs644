#pragma once
#include "Stmt.hpp"
#include "Expr.hpp"

class Move : public Stmt, std::enable_shared_from_this<Move>{
private:
    std::shared_ptr<Expr> target;
    std::shared_ptr<Expr> src;

public:
    Move(std::shared_ptr<Expr> target, std::shared_ptr<Expr> src, bool trash = false);
    virtual ~Move();

    std::shared_ptr<Expr> getTarget() const;
    std::shared_ptr<Expr> getSource() const;

    virtual std::string getLabel() const override;

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
    template<typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) override;
};