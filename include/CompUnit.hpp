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

    CompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions);

    void appendFunc(std::shared_ptr<FuncDecl> func);

    std::string getName() const;

    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> getFunctions() const;

    std::shared_ptr<FuncDecl> getFunction(const std::string& name) const;

    std::string getLabel() const override;

    template <typename T>
    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        for (auto [key, func] : functions)
            result = v->bind(result, v->visit(func));
        return result;
    }

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

};
