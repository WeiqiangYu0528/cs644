#include "Seq.hpp"

Seq::Seq(const std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent)
    : stmts(filterNulls(stmts)), replaceParent(replaceParent) {}

template<typename T>
std::vector<std::shared_ptr<T>> Seq::filterNulls(const std::vector<std::shared_ptr<T>>& list) {
    std::vector<std::shared_ptr<T>> filtered;
    std::copy_if(list.begin(), list.end(), std::back_inserter(filtered),
                 [](const std::shared_ptr<T>& item) { return bool(item); });
    return filtered;
}

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

template<typename T>
T Seq::aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
    T result = v->unit();
    for (const auto& stmt : stmts) {
        result = v->bind(result, v->visit(stmt.get()));
    }
    return result;
}

std::shared_ptr<CheckCanonicalIRVisitor> Seq::checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterSeq();
}

bool Seq::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return !v->inSeq();
}
