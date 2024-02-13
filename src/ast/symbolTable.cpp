#include "symbolTable.hpp"

void SymbolTable::put(std::string& key, std::shared_ptr<Exp> value) {
    stack_t.push(key);
    table[key] = value;
}

std::shared_ptr<Exp> SymbolTable::get(std::string& key) {
    return table[key];
}

void SymbolTable::beginScope() {
    stack_t.push("#");
}

void SymbolTable::endScope() {
    while (stack_t.top() != "#") {
        table.erase(stack_t.top());
        stack_t.pop();
    }
    stack_t.pop();
}
