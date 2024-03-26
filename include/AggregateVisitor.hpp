#pragma once
#include <stdexcept>
#include "Node.hpp"

template<typename T>
class AggregateVisitor {
public:
    virtual T unit() = 0;
    virtual T bind(T r1, T r2) = 0;

    virtual T visit(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
        if (!n) return nullptr;

        T overrideValue = override(parent, n);
        if (overrideValue != nullptr) return overrideValue;

        std::shared_ptr<AggregateVisitor<T>> v_ = enter(parent, n);
        if (!v_)
            throw std::runtime_error("AggregateVisitor::enter() returned null!");

        T r = n->aggregateChildren(v_);
        if (!r)
            throw std::runtime_error("Node::aggregateChildren() returned null!");

        r = leave(parent, n, r, v_);
        if (!r)
            throw std::runtime_error("AggregateVisitor::leave() returned null!");

        return r;
    }

    virtual T visit(std::shared_ptr<Node> node) {
        return visit(nullptr, node);
    }

protected:
    virtual T override(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
        return nullptr; 
    }

    virtual std::shared_ptr<AggregateVisitor<T>> enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n) {
        return std::shared_ptr<AggregateVisitor<T>>(this);
    }

    virtual T leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, T r, std::shared_ptr<AggregateVisitor<T>> v_) {
        return r;
    }

    template <typename V>
    std::shared_ptr<V> AggregateVisitor<T>::copyIfNeeded(std::shared_ptr<V> v) {
        return (v.get() == this) ? std::make_shared<V>(*v) : v;
    }
};
