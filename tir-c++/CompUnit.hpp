#pragma once
#include <string>
#include <unordered_map>
#include "FuncDecl.hpp"
// import joosc.ir.visit.AggregateVisitor;
// import joosc.ir.visit.IRVisitor;

// import java.util.LinkedHashMap;
// import java.util.Map;

/**
 * An intermediate representation for a compilation unit
 */
class CompUnit : public Node_c {
private:
    std::string name;
    std::unordered_map<std::string, FuncDecl> functions;

public:
    CompUnit(const std::string& name);

    CompUnit(const std::string& name, std::unordered_map<std::string, FuncDecl> functions);

    void appendFunc(FuncDecl& func);

    std::string name() const;

    std::unordered_map<std::string, FuncDecl> functions() const;

    FuncDecl getFunction(const std::string& name) const;

    std::string label() const override;

    Node visitChildren(IRVisitor& v) override;

    template <typename T>
    T aggregateChildren(AggregateVisitor<T>& v) {
        T result = v.unit();
        for (FuncDecl func : functions.values())
            result = v.bind(result, v.visit(func));
        return result;
    }
}
