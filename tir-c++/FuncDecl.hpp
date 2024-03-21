#pragma once
#include <memory>
#include <string>
#include "Stmt.hpp"

/** An IR function declaration */
class FuncDecl : public Node_c, std::enable_shared_from_this<FuncDecl> {
private:
    std::string name;
    std::shared_ptr<Stmt> body;
    int numParams;

public:
    FuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> body);

    std::string getName() const;

    std::shared_ptr<Stmt> getBody() const;

    int getNumParams() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
        T result = v->unit();
        result = v->bind(result, v->visit(body));
        return result;
    }

    std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override;

    std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) override;
}
