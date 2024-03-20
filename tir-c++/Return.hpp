#include "Stmt.hpp"
#include "Expr.hpp"
#include "IRVisitor.hpp"
#include "AggregateVisitor.hpp"

class Return : public Stmt {
protected:
    Expr* ret;

public:
    explicit Return(Expr* ret);

    virtual ~Return();

    Expr* ret() const;

    virtual std::string label() const override;

    virtual Node* visitChildren(IRVisitor* v);
    template<typename T>
    T aggregateChildren(AggregateVisitor<T>* v);
};