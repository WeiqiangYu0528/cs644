#pragma once
#include "IRVisitor.hpp"
#include "InsnMapsBuilder.hpp"
#include "CheckCanonicalIRVisitor.hpp"

class Node_c : public Node {
public:
    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override {
        return shared_from_this();
    }

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        return v->unit();
    }

    virtual std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override {
        return v;
    }

    virtual std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) override {
        v->addInsn(shared_from_this());
        return shared_from_this();
    }

    virtual std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) override {
        return v;
    }

    virtual bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override {
        return true;
    }

    virtual std::string getLabel() const override = 0;
};
