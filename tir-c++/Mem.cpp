#include "Mem.hpp"

Mem::Mem(std::shared_ptr<Expr_c> expr) : expr(expr) {}

Mem::~Mem() {
}

std::shared_ptr<Expr_c> Mem::getExpr() const {
    return expr;
}

std::string Mem::getLabel() const {
    return "MEM";
}

std::shared_ptr<Node> Mem::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr_c> newExpr = std::dynamic_pointer_cast<Expr_c>(v->visit(shared_from_this(), expr));
    if (newExpr != expr) {
        return v->nodeFactory().IRMem(newExpr);
    }
    return this;
}

template<typename T>
T Mem::aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
    T result = v->unit();
    result = v->bind(result, v->visit(expr));
    return result;
}
