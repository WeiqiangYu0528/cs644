#include "symbolTable.hpp"

void SymbolTable::putField(const std::string& key, const std::shared_ptr<AstNode> value) {
    ftable[key].push_back(value);
}

std::shared_ptr<AstNode> SymbolTable::getField(const std::string& key, size_t idx) const {
    return ftable.contains(key) ? ftable.at(key)[idx] : nullptr;
}

void SymbolTable::putMethod(const std::string& key, const std::shared_ptr<AstNode> value) {
    mtable[key].push_back(value);
}

std::shared_ptr<AstNode> SymbolTable::getMethod(const std::string& key, size_t idx) const {
    return mtable.contains(key) ? mtable.at(key)[idx] : nullptr;
}

void SymbolTable::putConstuctor(const std::string& key, const std::shared_ptr<AstNode> value) {
    ctable[key].push_back(value);
}

std::shared_ptr<AstNode> SymbolTable::getConstructor(const std::string& key, size_t idx) const {
    return ctable.contains(key) ? ctable.at(key)[idx] : nullptr;
}

void SymbolTable::putVar(const std::string& key, const std::shared_ptr<AstNode> value) {
    stack_t.push(key);
    ltable[key].push_back(value);
}

std::shared_ptr<AstNode> SymbolTable::getVar(const std::string& key, size_t idx) const {
    return ltable.contains(key) ? ltable.at(key)[idx] : nullptr;
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
