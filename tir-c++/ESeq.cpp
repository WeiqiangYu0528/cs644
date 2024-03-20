#include "ESeq.hpp"

ESeq::ESeq(Stmt& stmt, Expr& expr) : stmt(stmt), expr(expr) {
}

ESeq::ESeq(Stmt& stmt, Expr& expr, bool replaceParent) : stmt(stmt), expr(expr) {
}

Stmt ESeq::stmt() const {
    return stmt;
}

Expr ESeq::expr() const {
    return expr;
}

std::string ESeq::label() const {
    return "ESEQ";
}

Node ESeq::visitChildren(IRVisitor& v) {
    Stmt stmt = (Stmt) v.visit(this, this.stmt);
    Expr expr = (Expr) v.visit(this, this.expr);

    if (expr != this.expr || stmt != this.stmt)
        return v.nodeFactory().IRESeq(stmt, expr);

    return this;
}

bool ESeq::isCanonical(CheckCanonicalIRVisitor& v) const {
    return false;
}
