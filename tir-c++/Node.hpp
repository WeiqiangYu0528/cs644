#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"
#include "InsnMapsBuilder.hpp"
#include "CheckCanonicalIRVisitor.hpp"

class Node {
public:
    virtual ~Node() = default;

    virtual Node* visitChildren(IRVisitor* v) = 0;

    virtual InsnMapsBuilder* buildInsnMapsEnter(InsnMapsBuilder* v) = 0;

    virtual Node* buildInsnMaps(InsnMapsBuilder* v) = 0;

    template<typename T>
    virtual T aggregateChildren(AggregateVisitor<T>* v) = 0;

    virtual CheckCanonicalIRVisitor* checkCanonicalEnter(CheckCanonicalIRVisitor* v) = 0;

    virtual bool isCanonical(CheckCanonicalIRVisitor* v) = 0;

    virtual std::string label() const = 0;
};