#include "BinOp.hpp"

BinOp::BinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) : type(type), left(left), right(right) {
}

OpType BinOp::getOpType() const {
    return type;
}

std::shared_ptr<Expr> BinOp::getLeft() const {
    return left;
}

std::shared_ptr<Expr> BinOp::getRight() const {
    return right;
}

std::string BinOp::getLabel() const {
    return OpTypeToString[type];
}

std::shared_ptr<Node> BinOp::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> l = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), left));
    std::shared_ptr<Expr> r = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), right));
    
    if (l != left || r != right)
        return v->nodeFactory()->IRBinOp(type, l, r);

    return shared_from_this();
}

bool BinOp::isConstant() const {
    return left->isConstant() && right->isConstant();
}
