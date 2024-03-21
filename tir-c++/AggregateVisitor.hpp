#pragma once
#include <stdexcept>
#include "Node.hpp"

template<typename T>
class AggregateVisitor {
public:
    virtual ~AggregateVisitor() {}

    virtual T unit() = 0;
    virtual T bind(T r1, T r2) = 0;
    virtual T visit(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);
    virtual T visit(std::shared_ptr<Node> node);

protected:
    virtual T override(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);
    virtual std::shared_ptr<AggregateVisitor<T>> enter(std::shared_ptr<Node> parent, std::shared_ptr<Node> n);
    virtual T leave(std::shared_ptr<Node> parent, std::shared_ptr<Node> n, T r, std::shared_ptr<AggregateVisitor<T>> v_);

    template <typename V>
    std::shared_ptr<V> copyIfNeeded(std::shared_ptr<V> v);
};
