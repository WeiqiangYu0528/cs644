#include "IRAst.hpp"
#include "CheckCanonicalIRVisitor.hpp"
#include <iostream>

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
    if (!inSeqFlag) {
        std::shared_ptr<CheckCanonicalIRVisitor> v = std::make_shared<CheckCanonicalIRVisitor>();
        std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(v)->outer = std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
        std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(v)->inSeqFlag = true;
        return std::static_pointer_cast<CheckCanonicalIRVisitor>(v);
    }
    return std::static_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterExp() {
    if (!inExpFlag) {
        std::shared_ptr<CheckCanonicalIRVisitor> v = std::make_shared<CheckCanonicalIRVisitor>();
        std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(v)->outer = std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
        std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(v)->inExpFlag = true;
        return v;
    }
    return std::static_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterExpr() {
    if (!inExprFlag) {
        std::shared_ptr<CheckCanonicalIRVisitor> v = std::make_shared<CheckCanonicalIRVisitor>();
        std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(v)->outer = std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
        std::dynamic_pointer_cast<CheckCanonicalIRVisitor>(v)->inExprFlag = true;
        return v;
        
    }
    return std::static_pointer_cast<CheckCanonicalIRVisitor>(shared_from_this());
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
