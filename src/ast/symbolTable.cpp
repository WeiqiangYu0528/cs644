#include "symbolTable.hpp"

void SymbolTable::putField(const std::string& key, const std::shared_ptr<AstNode> value) {
    ftable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getField(const std::string& key) const {
    return ftable.contains(key) ? ftable.at(key) : nullptr;
}

void SymbolTable::putMethod(const std::string& key, const std::shared_ptr<AstNode> value) {
    mtable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getMethod(const std::string& key) const {
    return mtable.contains(key) ? mtable.at(key) : nullptr;
}

void SymbolTable::putConstructor(const std::string& key, const std::shared_ptr<AstNode> value) {
    ctable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getConstructor(const std::string& key) const {
    return ctable.contains(key) ? ctable.at(key) : nullptr;
}

void SymbolTable::putVar(const std::string& key, const std::shared_ptr<AstNode> value) {
    stack_t.push(key);
    ltable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getVar(const std::string& key) const {
    return ltable.contains(key) ? ltable.at(key) : nullptr;
}

void SymbolTable::beginScope() {
    stack_t.push("#");
}

void SymbolTable::endScope() {
    while (stack_t.top() != "#") {
        ltable.erase(stack_t.top());
        stack_t.pop();
    }
    stack_t.pop();
}

void SymbolTable::setAst(std::shared_ptr<Program> a) {
    ast = a;
}

std::shared_ptr<Program> SymbolTable::getAst() const {
    return ast;
}
