#pragma once;
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.IRVisitor;
#include "Expr.hpp"
/**
 * An intermediate representation for a transfer of control
 */
class Jump : public Stmt {
private:
    Expr target;

public:
    /**
     *
     * @param expr the destination of the jump
     */
    Jump(Expr& expr);

    Expr target() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) {
        T result = v.unit();
        result = v.bind(result, v.visit(target));
        return result;
    }
}
