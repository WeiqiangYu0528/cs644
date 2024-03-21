#include "CompUnit.hpp"

CompUnit::CompUnit(const std::string& name) : name(name) {
}

CompUnit::CompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>> functions) : name(name), functions(functions) {
}

void CompUnit::appendFunc(std::shared_ptr<FuncDecl> func) {
    functions[func.name()] = func;
}

std::string CompUnit::getName() const {
    return name;
}

std::unordered_map<std::string, std::shared_ptr<FuncDecl>> CompUnit::getFunctions() const {
    return functions;
}

std::shared_ptr<FuncDecl> CompUnit::getFunction(const std::string& name) const {
    return functions.at(name);
}

std::string CompUnit::getLabel() const {
    return "COMPUNIT";
}

std::shared_ptr<Node> CompUnit::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;

    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> results;
    for (auto [key, func] : functions) {
        std::shared_ptr<FuncDecl> newFunc = std::dynamic_pointer_cast<FuncDecl>(v.visit(shared_from_this(), func));
        if (newFunc != func) modified = true;
        results[newFunc.name()] = newFunc;
    }

    if (modified) return v->nodeFactory()->IRCompUnit(name, results);

    return shared_from_this();
}
