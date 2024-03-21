#pragma once
#include <memory>
#include <vector>
#include "Expr_c.hpp"

/**
 * An intermediate representation for a function call
 * CALL(e_target, e_1, ..., e_n)
 */
class Call : public Expr_c, public std::enable_shared_from_this<Call> {
protected:
    std::shared_ptr<Expr> target;
    std::vector<Expr> args;

public:
    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    template<typename... Args>
    Call(std::shared_ptr<Expr> target, Args... args) : Call(target, std::vector<std::shared_ptr<Expr>>{args...}) {
    }
    /**
     *
     * @param target address of the code for this function call
     * @param numRets number of return values for this function call
     * @param args arguments of this function call
     */
    Call(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>> args);

    std::shared_ptr<Expr> getTarget() const;

    std::vector<std::shared_ptr<Expr>> getArgs() const;

    int getNumArgs() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) override {
        T result = v->unit();
        result = v->bind(result, v->visit(target));
        for (std::shared_ptr<Expr> arg : args)
            result = v->bind(result, v->visit(arg));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
}
