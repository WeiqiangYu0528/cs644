#pragma once
#include <memory>
#include <stdexcept>

namespace TIR {

class Node;

class AggregateVisitor : public std::enable_shared_from_this<AggregateVisitor> {
public:
    virtual bool unit() = 0;
    virtual bool bind(bool r1, bool r2) = 0;

    virtual bool visit(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);

    virtual bool visit(std::shared_ptr<Node> node);

protected:
    virtual bool override(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);

    virtual std::shared_ptr<AggregateVisitor> enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);

    virtual bool leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, bool r, std::shared_ptr<AggregateVisitor> v_);

    template <typename V>
    std::shared_ptr<V> copyIfNeeded(std::shared_ptr<V> v) {
        return (v.get() == this) ? std::make_shared<V>(*v) : v;
    }
};

};
