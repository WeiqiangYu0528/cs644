#pragma once
#include <string>
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.IRVisitor;
// import joosc.ir.visit.InsnMapsBuilder;

/** An IR function declaration */
class FuncDecl : public Node_c {
private:
    std::string name;
    Stmt body;
    int numParams;

public:
    FuncDecl(const std::string& name, int numParams, Stmt& body);

    std::string name() const;

    Stmt body() const;

    int getNumParams() const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) {
        T result = v.unit();
        result = v.bind(result, v.visit(body));
        return result;
    }

    InsnMapsBuilder buildInsnMapsEnter(InsnMapsBuilder& v) override;

    Node buildInsnMaps(InsnMapsBuilder& v) override;
}
