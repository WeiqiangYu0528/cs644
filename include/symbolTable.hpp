#pragma once
#include <memory>
#include <stack>
#include <unordered_map>
#include "ast.hpp"
class SymbolTable {
    std::shared_ptr<Program> ast;
    // field table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ftable;
    // method table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> mtable;
    // constructor table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ctable;
    // local variable table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ltable;
    std::stack<std::string> stack_t;

    public:
        void putField(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getField(const std::string& key) const;
        void putMethod(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getMethod(const std::string& key) const;
        void putConstuctor(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getConstructor(const std::string& key) const;
        void putVar(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getVar(const std::string& key) const;
        void setAst(std::shared_ptr<Program> a);
        void beginScope();
        void endScope();
};
