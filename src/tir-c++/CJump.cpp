#include "CJump.hpp"
#include "NodeFactory.hpp"

CJump::CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel) : CJump(cond, trueLabel, nullptr) {
}

CJump::CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel, const std::string& falseLabel) : cond(cond), trueLabel(trueLabel), falseLabel(falseLabel) {
}

std::shared_ptr<Expr> CJump::getCond() const {
    return cond;
}

std::string CJump::getTrueLabel() const {
    return trueLabel;
}

std::string CJump::getFalseLabel() const {
    return falseLabel;
}

bool CJump::hasFalseLabel() const {
    return !falseLabel.empty();
}

std::string CJump::getLabel() const {
    return "CJUMP";
}

std::shared_ptr<Node> CJump::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), cond));

    if (expr != cond)
        return v->nodeFactory()->IRCJump(expr, trueLabel, falseLabel);

    return shared_from_this();
}

bool CJump::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !hasFalseLabel();
}
