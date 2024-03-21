#include "CJump.hpp"

CJump::CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel) : CJump(cond, trueLabel, nullptr) {
}

CJump::CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel, const std::string& falseLabel) : cond(cond), trueLabel(trueLabel), falseLabel(falseLabel) {
}

std::shared_ptr<Expr> CJump::cond() const {
    return cond;
}

std::string CJump::trueLabel() const {
    return trueLabel;
}

std::string CJump::falseLabel() const {
    return falseLabel;
}

bool CJump::hasFalseLabel() const {
    return falseLabel != nullptr;
}

std::string CJump::label() const {
    return "CJUMP";
}

std::shared_ptr<Node> CJump::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = (Expr) v.visit(this, this.cond);

    if (expr != this.cond)
        return v.nodeFactory().IRCJump(expr, trueLabel, falseLabel);

    return this;
}

bool CJump::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !hasFalseLabel();
}
