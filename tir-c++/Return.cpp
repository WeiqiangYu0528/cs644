#include "Return.hpp"

Return::Return(Expr* ret) : ret(ret) {}

Return::~Return() {
}

Expr* Return::ret() const {
    return ret;
}

std::string Return::label() const {
    return "RETURN";
}

Node* Return::visitChildren(IRVisitor* v) {
    bool modified = false;
    Expr* newExpr = dynamic_cast<Expr*>(v->visit(this, ret));

    if (newExpr != ret) modified = true;
    Expr* result = newExpr;

    if (modified) {
        return v->nodeFactory()->IRReturn(result);
    }

    return this;
}

template<typename T>
T Return::aggregateChildren(AggregateVisitor<T>* v) {
    T result = v->unit();
    result = v->bind(result, v->visit(ret));
    return result;
}