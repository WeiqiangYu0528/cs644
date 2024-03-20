#include "Mem.hpp"

Mem::Mem(Expr_c* expr) : expr(expr) {}

Mem::~Mem() {
}

Expr_c* Mem::expr() const {
    return expr;
}

std::string Mem::label() const {
    return "MEM";
}

Node* Mem::visitChildren(IRVisitor* v) {
    Expr_c* newExpr = dynamic_cast<Expr_c*>(v->visit(this, expr));
    if (newExpr != expr) {
        return v->nodeFactory().IRMem(newExpr);
    }
    return this;
}

template<typename T>
T Mem::aggregateChildren(AggregateVisitor<T>* v) {
    T result = v->unit();
    result = v->bind(result, v->visit(expr));
    return result;
}
