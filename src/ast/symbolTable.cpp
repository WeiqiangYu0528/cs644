#include "symbolTable.hpp"

void SymbolTable::putField(const std::string& key, const std::shared_ptr<AstNode> value) {
    ftable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getField(const std::string& key) const {
    return ftable.contains(key) ? ftable.at(key) : nullptr;
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
