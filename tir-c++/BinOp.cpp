#include "BinOp.hpp"

BinOp::BinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) : type(type), left(left), right(right) {
}

OpType BinOp::opType() const {
    return type;
}

std::shared_ptr<Expr> BinOp::left() const {
    return left;
}

std::shared_ptr<Expr> BinOp::right() const {
    return right;
}

std::string BinOp::label() const {
    return OpTypeToString[type];
}

std::shared_ptr<Node> BinOp::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> left = (Expr) v.visit(this, this.left);
    std::shared_ptr<Expr> right = (Expr) v.visit(this, this.right);

    if (left != this.left || right != this.right)
        return v.nodeFactory().IRBinOp(type, left, right);

    return this;
}

bool BinOp::isConstant() const {
    return left->isConstant() && right->isConstant();
}
