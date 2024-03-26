#include "Call.hpp"

Call::Call(std::shared_ptr<Expr> target, std::vector<std::shared_ptr<Expr>> args) : target(target), args(args) {
}

Expr Call::getTarget() const {
    return target;
}

std::vector<std::shared_ptr<Expr>> Call::getArgs() const {
    return args;
}

int Call::getNumArgs() const {
    return args.size();
}

std::string Call::getLabel() const {
    return "CALL";
}

std::shared_ptr<Node> Call::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;

    std::shared_ptr<Expr> t = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));
    if (t != target) modified = true;

    std::vector<std::shared_ptr<Expr>> results(args.size());
    for (std::shared_ptr<Expr> arg : args) {
        std::shared_ptr<Expr> newExpr = std::dynamic_pointer_cast<Expr>(v.visit(shared_from_this(), arg));
        if (newExpr != arg) modified = true;
        results.add(newExpr);
    }

    if (modified) return v.nodeFactory().IRCall(t, results);

    return shared_from_this();
}

bool Call::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !v->inExpr();
}
