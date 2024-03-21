#include "Call.hpp"

Call::Call(std::shared_ptr<Expr> target, std::vector<std::shared_ptr<Expr>> args) : target(target), args(args) {
}

Expr Call::target() const {
    return target;
}

std::vector<std::shared_ptr<Expr>> Call::args() const {
    return args;
}

int Call::getNumArgs() const {
    return args.size();
}

std::string Call::label() const {
    return "CALL";
}

std::shared_ptr<Node> Call::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;

    std::shared_ptr<Expr> target = (Expr) v.visit(this, this.target);
    if (target != this.target) modified = true;

    std::vector<std::shared_ptr<Expr>> results = new ArrayList<>(args.size());
    for (std::shared_ptr<Expr> arg : args) {
        std::shared_ptr<Expr> newExpr = (Expr) v.visit(this, arg);
        if (newExpr != arg) modified = true;
        results.add(newExpr);
    }

    if (modified) return v.nodeFactory().IRCall(target, results);

    return this;
}

bool Call::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !v->inExpr();
}
