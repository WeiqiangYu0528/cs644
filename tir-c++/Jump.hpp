#pragma once;
#include <memory>
#include "Expr.hpp"
/**
 * An intermediate representation for a transfer of control
 */
class Jump : public Stmt, public std::enable_shared_from_this<Jump> {
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

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
        T result = v->unit();
        result = v->bind(result, v->visit(target));
        return result;
    }
}
