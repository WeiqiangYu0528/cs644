#include "Seq.hpp"

Seq::Seq(const std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent)
    : stmts(filterNulls(stmts)), replaceParent(replaceParent) {}

const std::vector<std::shared_ptr<Stmt>>& Seq::getStmts() const {
    return stmts;
}

std::string Seq::getLabel() const {
    return "SEQ";
}

std::shared_ptr<Node> Seq::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;
    std::vector<std::shared_ptr<Stmt>> results;

    for (const auto& stmt : stmts) {
        auto newStmt = std::dynamic_pointer_cast<Stmt>(v->visit(share_from_this(), stmt.get()));
        if (newStmt != stmt.get()) modified = true;
        results.emplace_back(newStmt);
    }

    if (modified) return v->nodeFactory()->IRSeq(results, replaceParent);

    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> Seq::checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterSeq();
}

bool Seq::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return !v->inSeq();
}
