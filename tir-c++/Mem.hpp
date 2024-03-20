#include "Expr_c.hpp"
#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"
#include "Node.hpp"

class Mem : public Expr_c {
private:
    Expr_c* expr;

public:
    explicit Mem(Expr_c* expr);

    virtual ~Mem();

    Expr_c* expr() const;

    virtual std::string label() const override;

    virtual Node* visitChildren(IRVisitor* v) override;
    template<typename T>
    T aggregateChildren(AggregateVisitor<T>* v) override;
};
