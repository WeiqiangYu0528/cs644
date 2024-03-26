#pragma once

class AggregateVisitor;
class IRVisitor;
class CheckCanonicalIRVisitor;
class InsnMapsBuilder;

class Node : public std::enable_shared_from_this<Node> {
public:
    virtual ~Node() = default;

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) = 0;

    virtual std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) = 0;

    virtual std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) = 0;

    virtual bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) = 0;

    virtual std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) = 0;

    virtual bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const = 0;

    virtual std::string getLabel() const = 0;
};
