#include "CJump.hpp"

CJump::CJump(Expr& cond, const std::string& trueLabel) : CJump(cond, trueLabel, nullptr) {
}

CJump::CJump(Expr& cond, const std::string& trueLabel, const std::string& falseLabel) : cond(cond), trueLabel(trueLabel), falseLabel(falseLabel) {
}

Expr CJump::cond() const {
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

Node CJump::visitChildren(IRVisitor& v) {
    Expr expr = (Expr) v.visit(this, this.cond);

    if (expr != this.cond)
        return v.nodeFactory().IRCJump(expr, trueLabel, falseLabel);

    return this;
}

bool CJump::isCanonical(CheckCanonicalIRVisitor& v) const {
    return !hasFalseLabel();
}
