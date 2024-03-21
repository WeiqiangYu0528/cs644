#pragma once
#include <memory>
#include <unordered_map>
#include "Expr_c.hpp"

/**
 * An intermediate representation for a binary operation
 * OP(left, right)
 */

enum class OpType {
    ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
    EQ, NEQ, LT, GT, LEQ, GEQ
};

std::unordered_map<OpType, std::string> OpTypeToString = {
    {OpType::ADD, "ADD"},
    {OpType::SUB, "SUB"},
    {OpType::MUL, "MUL"},
    {OpType::DIV, "DIV"},
    {OpType::MOD, "MOD"},
    {OpType::AND, "AND"},
    {OpType::OR, "OR"},
    {OpType::XOR, "XOR"},
    {OpType::LSHIFT, "LSHIFT"},
    {OpType::RSHIFT, "RSHIFT"},
    {OpType::ARSHIFT, "ARSHIFT"},
    {OpType::EQ, "EQ"},
    {OpType::NEQ, "NEQ"},
    {OpType::LT, "LT"},
    {OpType::GT, "GT"},
    {OpType::LEQ, "LEQ"},
    {OpType::GEQ, "GEQ"}
};

/**
 * Binary operators
 */
class BinOp : public Expr_c {
private:
    OpType type;
    std::shared_ptr<Expr> left, right;

public:
    BinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);

    OpType opType() const;

    std::shared_ptr<Expr> left() const;

    std::shared_ptr<Expr> right() const;

    std::string label() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) override {
        T result = v.unit();
        result = v.bind(result, v.visit(left));
        result = v.bind(result, v.visit(right));
        return result;
    }

    bool isConstant() const override;
}
