#include "IRAst.hpp"
#include "CheckCanonicalIRVisitor.hpp"
#include <iostream>

using namespace TIR;

CheckCanonicalIRVisitor::CheckCanonicalIRVisitor()
    : inSeqFlag(false), inExpFlag(false), inExprFlag(false), offender(nullptr), outer(nullptr) {}

bool CheckCanonicalIRVisitor::unit() {
    return true;
}

bool CheckCanonicalIRVisitor::bind(bool r1, bool r2) {
    return r1 && r2;
}

std::shared_ptr<AggregateVisitor> CheckCanonicalIRVisitor::enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    return n->checkCanonicalEnter(std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this()));
}

bool CheckCanonicalIRVisitor::leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, bool r, std::shared_ptr<AggregateVisitor> v_) {
    if (!r) return false;
    if (!n->isCanonical(std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this()))) {
        noncanonical(parent ? parent : n);
        return false;
    }
    return true;
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterSeq() {
    std::shared_ptr<CheckCanonicalIRVisitor> v_ = std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
    if (!inSeqFlag) {
        std::shared_ptr<CheckCanonicalIRVisitor> v = copyIfNeeded(v_);
        v->outer = v_;
        v->inSeqFlag = true;
        return v;
    }
    return v_;
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterExp() {
    std::shared_ptr<CheckCanonicalIRVisitor> v_ = std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
    if (!inExpFlag) {
        std::shared_ptr<CheckCanonicalIRVisitor> v = copyIfNeeded(v_);
        v->outer = v_;
        v->inExpFlag = true;
        return v;
    }
    return v_;
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterExpr() {
    std::shared_ptr<CheckCanonicalIRVisitor> v_ = std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
    if (!inExprFlag) {
        std::shared_ptr<CheckCanonicalIRVisitor> v = copyIfNeeded(v_);
        v->outer = v_;
        v->inExprFlag = true;
        return v;
    }
    return v_;
}

bool CheckCanonicalIRVisitor::inSeq() const {
    return inSeqFlag;
}

bool CheckCanonicalIRVisitor::inExp() const {
    return inExpFlag;
}

bool CheckCanonicalIRVisitor::inExpr() const {
    return inExprFlag;
}

std::shared_ptr<Node> CheckCanonicalIRVisitor::noncanonical() {
    return offender;
}

void CheckCanonicalIRVisitor::noncanonical(std::shared_ptr<Node> offender) {
    this->offender = offender;
    if (outer) {
        outer->noncanonical(offender);
    }
}
