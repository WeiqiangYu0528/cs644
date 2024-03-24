#pragma once
#include "Stmt.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class Seq : public Stmt, public std::enable_shared_from_this<Seq>{
private:
    std::vector<std::shared_ptr<Stmt>> stmts;
    bool replaceParent;
    template <typename T>
    static std::vector<std::shared_ptr<T>> filterNulls(const std::vector<std::shared_ptr<T>>& list) {
        std::vector<std::shared_ptr<T>> filtered;
        std::copy_if(list.begin(), list.end(), std::back_inserter(filtered),
                    [](const std::shared_ptr<T>& item) { return bool(item); });
        return filtered;
    }

public:
    Seq(const std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent = false);
    const std::vector<std::shared_ptr<Stmt>>& getStmts() const;
    std::string getLabel() const override;
    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
    template<typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
        T result = v->unit();
        for (const auto& stmt : stmts) {
            result = v->bind(result, v->visit(stmt.get()));
        }
        return result;
    }
    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) override;
    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) override;
};