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

std::string Seq::label() const {
    return "SEQ";
}

Node* Seq::visitChildren(IRVisitor* v) {
    bool modified = false;
    std::vector<std::shared_ptr<Stmt>> results;

    for (const auto& stmt : stmts) {
        auto newStmt = dynamic_cast<Stmt*>(v->visit(this, stmt.get()));
        if (newStmt != stmt.get()) modified = true;
        results.emplace_back(newStmt);
    }

    if (modified) return v->nodeFactory()->IRSeq(results, replaceParent);

    return this;
}

template<typename T>
T Seq::aggregateChildren(AggregateVisitor<T>* v) {
    T result = v->unit();
    for (const auto& stmt : stmts) {
        result = v->bind(result, v->visit(stmt.get()));
    }
    return result;
}

CheckCanonicalIRVisitor* Seq::checkCanonicalEnter(CheckCanonicalIRVisitor* v) {
    return v->enterSeq();
}

bool Seq::isCanonical(CheckCanonicalIRVisitor* v) {
    return !v->inSeq();
}
