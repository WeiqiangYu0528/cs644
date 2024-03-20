#include "BinOp.hpp"

BinOp::BinOp(OpType type, Expr& left, Expr& right) : type(type), left(left), right(right) {
}

OpType BinOp::opType() const {
    return type;
}

Expr BinOp::left() const {
    return left;
}

Expr BinOp::right() const {
    return right;
}

std::string BinOp::label() const {
    using enum OpType;
    switch (opType) {
        case ADD:
            return "ADD";
        case SUB:
            return "SUB";
        case MUL:
            return "MUL";
        case DIV:   
            return "DIV";
        case MOD:
            return "MOD";
        case AND:
            return "AND";
        case OR:    
            return "OR";
        case XOR:
            return "XOR";
        case LSHIFT:
            return "LSHIFT";
        case RSHIFT:
            return "RSHIFT";
        case ARSHIFT:   
            return "ARSHIFT";
        case EQ:
            return "EQ";
        case NEQ:
            return "NEQ";
        case LT:
            return "LT";
        case GT:
            return "GT";
        case LEQ:
            return "LEQ";
        case GEQ:
            return "GEQ";
        default:
            return "UNKNOWN";
    }
}

//need to update this func.
Node BinOp::visitChildren(IRVisitor& v) {
    Expr left = (Expr) v.visit(this, this.left);
    Expr right = (Expr) v.visit(this, this.right);

    if (left != this.left || right != this.right)
        return v.nodeFactory().IRBinOp(type, left, right);

    return this;
}

bool BinOp::isConstant() const {
    return left.isConstant() && right.isConstant();
}
