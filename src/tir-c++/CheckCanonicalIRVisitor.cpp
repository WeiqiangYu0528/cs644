#include "CheckCanonicalIRVisitor.hpp"
#include "Node.hpp"

CheckCanonicalIRVisitor::CheckCanonicalIRVisitor()
    : inSeqFlag(false), inExpFlag(false), inExprFlag(false), offender(nullptr), outer(nullptr) {}

bool CheckCanonicalIRVisitor::unit() {
    return true;
}

bool CheckCanonicalIRVisitor::bind(bool r1, bool r2) {
    return r1 && r2;
}

std::shared_ptr<AggregateVisitor> CheckCanonicalIRVisitor::enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
    return nullptr;
    
    //  n->checkCanonicalEnter(shared_from_this());
}

bool CheckCanonicalIRVisitor::leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, bool r, std::shared_ptr<AggregateVisitor> v_) {
    if (!r) return false;
    if (!n->isCanonical(shared_from_this())) {
        noncanonical(parent ? parent : n);
        return false;
    }
    return true;
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterSeq() {
    if (!inSeqFlag) {
        auto v = copyIfNeeded(shared_from_this());
        v->outer = shared_from_this();
        v->inSeqFlag = true;
        return v;
    }
    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterExp() {
    if (!inExpFlag) {
        auto v = copyIfNeeded(shared_from_this());
        v->outer = shared_from_this();
        v->inExpFlag = true;
        return v;
    }
    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> CheckCanonicalIRVisitor::enterExpr() {
    if (!inExprFlag) {
        auto v = copyIfNeeded(shared_from_this());
        v->outer = shared_from_this();
        v->inExprFlag = true;
        return v;
    }
    return shared_from_this();
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
