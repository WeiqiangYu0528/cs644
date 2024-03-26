#pragma once
#include "Stmt.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class Seq : public Stmt, public std::enable_shared_from_this<Seq> {
private:
    std::vector<std::shared_ptr<Stmt>> stmts;
    bool replaceParent;
    std::vector<std::shared_ptr<Stmt>> filterNulls(std::vector<std::shared_ptr<Stmt>>& list);

public:
    Seq(std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent = false);

    std::vector<std::shared_ptr<Stmt>> getStmts() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template<typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
        T result = v->unit();
        for (auto stmt : stmts) {
            result = v->bind(result, v->visit(stmt));
        }
        return result;
    }

    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) override;

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) override;
};
