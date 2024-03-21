#include "CompUnit.hpp"

CompUnit::CompUnit(const std::string& name) : name(name) {
}

CompUnit::CompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>> functions) : name(name), functions(functions) {
}

void CompUnit::appendFunc(std::shared_ptr<FuncDecl> func) {
    functions[func.name()] = func;
}

std::string CompUnit::name() const {
    return name;
}

std::unordered_map<std::string, std::shared_ptr<FuncDecl>> CompUnit::functions() const {
    return functions;
}

std::shared_ptr<FuncDecl> CompUnit::getFunction(const std::string& name) const {
    return functions.get(name);
}

std::string CompUnit::label() const {
    return "COMPUNIT";
}

std::shared_ptr<Node> CompUnit::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;

    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> results;
    for (std::shared_ptr<FuncDecl> func : functions.values()) {
        std::shared_ptr<FuncDecl> newFunc = (FuncDecl) v.visit(this, func);
        if (newFunc != func) modified = true;
        results.put(newFunc.name(), newFunc);
    }

    if (modified) return v->nodeFactory().IRCompUnit(name, results);

    return this;
}
