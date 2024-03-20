#include "Stmt.hpp"
#include <vector>
#include <memory>
#include <algorithm>

class Seq : public Stmt {
private:
    std::vector<std::shared_ptr<Stmt>> stmts;
    bool replaceParent;
    template <typename T>
    static std::vector<std::shared_ptr<T>> filterNulls(const std::vector<std::shared_ptr<T>>& list);

public:
    Seq(const std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent = false);
    const std::vector<std::shared_ptr<Stmt>>& getStmts() const;
    std::string label() const override;
    Node* visitChildren(IRVisitor* v) override;
    template<typename T>
    T aggregateChildren(AggregateVisitor<T>* v);
    CheckCanonicalIRVisitor* checkCanonicalEnter(CheckCanonicalIRVisitor* v) override;
    bool isCanonical(CheckCanonicalIRVisitor* v) override;
};