#include "Stmt.hpp"
#include "Expr.hpp"

class Move : public Stmt {
private:
    Expr* target;
    Expr* src;

public:
    Move(Expr* target, Expr* src, bool trash = false);
    virtual ~Move();

    Expr* target() const;
    Expr* source() const;

    virtual std::string label() const override;

    virtual Node* visitChildren(IRVisitor* v) override;
    template<typename T>
    T aggregateChildren(AggregateVisitor<T>* v) override;
};