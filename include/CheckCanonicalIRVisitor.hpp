#pragma once
#include <memory>
#include "AggregateVisitor.hpp"

class CheckCanonicalIRVisitor : public AggregateVisitor {
public:
    CheckCanonicalIRVisitor();

    virtual bool unit() override;
    virtual bool bind(bool r1, bool r2) override;
    virtual std::shared_ptr<AggregateVisitor> enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) override;
    virtual bool leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, bool r, std::shared_ptr<AggregateVisitor> v_) override;

    std::shared_ptr<CheckCanonicalIRVisitor> enterSeq();
    std::shared_ptr<CheckCanonicalIRVisitor> enterExp();
    std::shared_ptr<CheckCanonicalIRVisitor> enterExpr();

    bool inSeq() const;
    bool inExp() const;
    bool inExpr() const;

    std::shared_ptr<Node> noncanonical();
    void noncanonical(std::shared_ptr<Node> offender);

protected:
    bool inSeqFlag;
    bool inExpFlag;
    bool inExprFlag;

    std::shared_ptr<Node> offender;
    std::shared_ptr<CheckCanonicalIRVisitor> outer;
};
