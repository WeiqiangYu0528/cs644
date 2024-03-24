#pragma once
#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"
#include "InsnMapsBuilder.hpp"
#include "CheckCanonicalIRVisitor.hpp"

class Node {
public:
    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) = 0;

    virtual std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) = 0;

    virtual std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) = 0;

    template<typename T>
    virtual T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) = 0;

    virtual std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) = 0;

    virtual bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) = 0;

    virtual std::string getLabel() const = 0;
};
