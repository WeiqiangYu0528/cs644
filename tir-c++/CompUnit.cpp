#include "CompUnit.hpp"

CompUnit::CompUnit(const std::string& name) : name(name) {
}

CompUnit::CompUnit(const std::string& name, std::unordered_map<std::string, FuncDecl> functions) : name(name), functions(functions) {
}

void CompUnit::appendFunc(FuncDecl& func) {
    functions[func.name()] = func;
}

std::string CompUnit::name() const {
    return name;
}

std::unordered_map<std::string, FuncDecl> CompUnit::functions() const {
    return functions;
}

FuncDecl CompUnit::getFunction(const std::string& name) const {
    return functions.get(name);
}

std::string CompUnit::label() const {
    return "COMPUNIT";
}

Node CompUnit::visitChildren(IRVisitor& v) {
    bool modified = false;

    std::unordered_map<std::string, FuncDecl> results;
    for (FuncDecl func : functions.values()) {
        FuncDecl newFunc = (FuncDecl) v.visit(this, func);
        if (newFunc != func) modified = true;
        results.put(newFunc.name(), newFunc);
    }

    if (modified) return v.nodeFactory().IRCompUnit(name, results);

    return this;
}
