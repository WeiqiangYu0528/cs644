#pragma once
#include <memory>
#include "Expr.hpp"
#include "Stmt.hpp"
/**
 * An intermediate representation for a transfer of control
 */
class Jump : public Stmt {
private:
    std::shared_ptr<Expr> target;

public:
    /**
     *
     * @param expr the destination of the jump
     */
    Jump(std::shared_ptr<Expr> expr);

    std::shared_ptr<Expr> getTarget() const;

    std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        result = v->bind(result, v->visit(target));
        return result;
    }

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};
