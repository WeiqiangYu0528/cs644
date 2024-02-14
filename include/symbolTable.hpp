#pragma once
#include <memory>
#include <stack>
#include <unordered_map>
#include "ast.hpp"
class SymbolTable {
    std::unordered_map<std::string, std::shared_ptr<Exp>> table;
    std::stack<std::string> stack_t;

    public:
        void put(std::string& key, std::shared_ptr<Exp> value);
        std::shared_ptr<Exp> get(std::string& key);
        void beginScope();
        void endScope();
};
