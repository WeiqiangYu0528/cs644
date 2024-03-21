#include "ESeq.hpp"

ESeq::ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) : stmt(stmt), expr(expr) {
}

ESeq::ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr, bool replaceParent) : stmt(stmt), expr(expr) {
}

std::shared_ptr<Stmt> ESeq::stmt() const {
    return stmt;
}

std::shared_ptr<Expr> ESeq::expr() const {
    return expr;
}

std::string ESeq::label() const {
    return "ESEQ";
}

std::shared_ptr<Node> ESeq::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Stmt> stmt = (Stmt) v.visit(this, this.stmt);
    std::shared_ptr<Expr> expr = (Expr) v.visit(this, this.expr);

    if (expr != this.expr || stmt != this.stmt)
        return v.nodeFactory().IRESeq(stmt, expr);

    return this;
}

bool ESeq::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return false;
}
