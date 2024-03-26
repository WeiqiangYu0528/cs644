#include "ESeq.hpp"
#include "NodeFactory.hpp"

ESeq::ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) : stmt(stmt), expr(expr) {
}

ESeq::ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr, bool replaceParent) : stmt(stmt), expr(expr) {
}

std::shared_ptr<Stmt> ESeq::getStmt() const {
    return stmt;
}

std::shared_ptr<Expr> ESeq::getExpr() const {
    return expr;
}

std::string ESeq::getLabel() const {
    return "ESEQ";
}

std::shared_ptr<Node> ESeq::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Stmt> s = std::dynamic_pointer_cast<Stmt>(v->visit(shared_from_this(), stmt));
    std::shared_ptr<Expr> e = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), expr));

    if (e != expr || s != stmt)
        return v->nodeFactory()->IRESeq(s, e);

    return shared_from_this();
}

bool ESeq::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return false;
}
