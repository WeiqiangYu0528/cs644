#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "FuncDecl.hpp"

/**
 * An intermediate representation for a compilation unit
 */
class CompUnit : public Node_c {
private:
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> functions;

public:
    CompUnit(const std::string& name);

    CompUnit(const std::string& name, std::unordered_map<std::string, FuncDecl> functions);

    void appendFunc(std::shared_ptr<FuncDecl> func);

    std::string name() const;

    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> functions() const;

    std::shared_ptr<FuncDecl> getFunction(const std::string& name) const;

    std::string label() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    template <typename T>
    T aggregateChildren(std::shared_ptr<AggregateVisitor<T>> v) {
        T result = v.unit();
        for (FuncDecl func : functions.values())
            result = v.bind(result, v.visit(func));
        return result;
    }
}
