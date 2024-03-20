#pragma once
#include <vector>
#include "Expr_c.hpp"
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.CheckCanonicalIRVisitor;
// import joosc.ir.visit.IRVisitor;

// import java.util.ArrayList;
// import java.util.Arrays;
// import java.util.List;

/**
 * An intermediate representation for a function call
 * CALL(e_target, e_1, ..., e_n)
 */
class Call : public Expr_c {
protected:
    Expr target;
    std::vector<Expr> args;

public:
    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    template<typename... Args>
    Call(Expr& target, Args... args) : Call(target, std::vector<Expr>{args...}) {
    }
    /**
     *
     * @param target address of the code for this function call
     * @param numRets number of return values for this function call
     * @param args arguments of this function call
     */
    Call(Expr& target, const std::vector<Expr>& args);

    Expr target() const;

    std::vector<Expr> args() const;

    int getNumArgs() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(target));
        for (Expr arg : args)
            result = v.bind(result, v.visit(arg));
        return result;
    }

    bool isCanonical(CheckCanonicalIRVisitor& v) const override;
}
