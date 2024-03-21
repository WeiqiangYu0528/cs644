#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"
#include "InsnMapsBuilder.hpp"
#include "CheckCanonicalIRVisitor.hpp"

class Node_c {
public:
    virtual ~Node_c() = default;

    virtual Node_c* visitChildren(IRVisitor* v) {
        return this;
    }

    template<typename T>
    T aggregateChildren(AggregateVisitor<T>* v) {
        return v->unit();
    }

    virtual InsnMapsBuilder* buildInsnMapsEnter(InsnMapsBuilder* v) {
        return v;
    }

    virtual Node_c* buildInsnMaps(InsnMapsBuilder* v) {
        v->addInsn(this);
        return this;
    }

    virtual CheckCanonicalIRVisitor* checkCanonicalEnter(CheckCanonicalIRVisitor* v) {
        return v;
    }

    virtual bool isCanonical(CheckCanonicalIRVisitor* v) {
        return true;
    }

    virtual std::string getLabel() const = 0;
};