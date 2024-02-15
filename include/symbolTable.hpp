#pragma once
#include <memory>
#include <stack>
#include <unordered_map>
#include "ast.hpp"
class SymbolTable {
    // field table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ftable;
    // local variable table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ltable;
    std::stack<std::string> stack_t;

    public:
        void putField(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getField(const std::string& key) const;
        void putVar(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getVar(const std::string& key) const;
        void beginScope();
        void endScope();
        
};
