#pragma once
#include <memory>
#include <string>
#include "Stmt.hpp"

/** An IR function declaration */
class FuncDecl : public Node_c {
public:
    std::string name;
    std::shared_ptr<Stmt> body;
    int numParams;

    FuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> body);

    std::string getName() const;

    std::shared_ptr<Stmt> getBody() const;

    int getNumParams() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(body));
        return result;
    }

    std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) override;
};
