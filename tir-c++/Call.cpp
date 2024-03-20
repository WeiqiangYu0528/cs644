#include "Call.hpp"

Call::Call(Expr& target, std::vector<Expr>& args) : target(target), args(args) {
}

Expr Call::target() const {
    return target;
}

std::vector<Expr> Call::args() const {
    return args;
}

int Call::getNumArgs() const {
    return args.size();
}

std::string Call::label() const {
    return "CALL";
}

Node Call::visitChildren(IRVisitor& v) {
    bool modified = false;

    Expr target = (Expr) v.visit(this, this.target);
    if (target != this.target) modified = true;

    std::vector<Expr> results = new ArrayList<>(args.size());
    for (Expr arg : args) {
        Expr newExpr = (Expr) v.visit(this, arg);
        if (newExpr != arg) modified = true;
        results.add(newExpr);
    }

    if (modified) return v.nodeFactory().IRCall(target, results);

    return this;
}

bool Call::isCanonical(CheckCanonicalIRVisitor& v) const {
    return !v.inExpr();
}
