#pragma once
#include <memory>
#include <stdexcept>

namespace TIR {

class NodeFactory;
class Node;

class IRVisitor : public std::enable_shared_from_this<IRVisitor> {
public:
    explicit IRVisitor(std::shared_ptr<NodeFactory> inf);

    std::shared_ptr<NodeFactory> nodeFactory();

    std::shared_ptr<Node> visit(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);
    std::shared_ptr<Node> visit(std::shared_ptr<Node> node);

protected:
    std::shared_ptr<NodeFactory> inf;
    virtual std::shared_ptr<Node> override(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);
    virtual std::shared_ptr<IRVisitor> enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);
    virtual std::shared_ptr<Node> leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, std::shared_ptr<Node> n_, std::shared_ptr<IRVisitor> v_);
    
    template<typename V>
    std::shared_ptr<V> copyIfNeeded(const std::shared_ptr<V> v) {
        if (v.get() == this) {
            return std::make_shared<V>(*v);
        }
        return v;
    }
};

};
