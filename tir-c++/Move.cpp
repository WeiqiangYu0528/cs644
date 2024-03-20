#include "Move.hpp"

Move::Move(Expr* target, Expr* src, bool trash) : target(target), src(src) {}

Move::~Move() {
}

Expr* Move::target() const {
    return target;
}

Expr* Move::source() const {
    return src;
}

std::string Move::label() const {
    return "MOVE";
}

Node* Move::visitChildren(IRVisitor* v) {
    Expr* newTarget = dynamic_cast<Expr*>(v->visit(this, target));
    Expr* newSrc = dynamic_cast<Expr*>(v->visit(this, src));

    if (newTarget != target || newSrc != src)
        return v->nodeFactory()->IRMove(newTarget, newSrc);

    return this;
}

template<typename T>
T Move::aggregateChildren(AggregateVisitor<T>* v) {
    T result = v->unit();
    result = v->bind(result, v->visit(target));
    result = v->bind(result, v->visit(src));
    return result;
}
