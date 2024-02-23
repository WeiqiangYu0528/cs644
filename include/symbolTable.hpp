#pragma once
#include <memory>
#include <stack>
#include <unordered_map>
#include "ast.hpp"
class SymbolTable {
    // program AST
    std::shared_ptr<Program> ast;
    // package
    std::string pkg;
    // member type declaration
    std::shared_ptr<ClassOrInterfaceDecl> cdecl;
    // field table
    std::unordered_map<std::string, std::shared_ptr<Field>> ftable;
    // method table
    std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>> mtable;
    // constructor table
    std::unordered_map<std::string, std::vector<std::shared_ptr<Constructor>>> ctable;
    // local variable table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ltable;
    std::stack<std::string> stack_t;

    public:
        void putField(const std::string& key, const std::shared_ptr<Field> value);
        std::shared_ptr<Field> getField(const std::string& key) const;
        void putMethod(const std::string& key, const std::shared_ptr<Method> value);
        std::vector<std::shared_ptr<Method>> getMethod(const std::string& key) const;
        void putConstuctor(const std::string& key, const std::shared_ptr<Constructor> value);
        std::vector<std::shared_ptr<Constructor>> getConstructor(const std::string& key) const;
        void putVar(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getVar(const std::string& key) const;
        void setAst(std::shared_ptr<Program> a);
        std::shared_ptr<Program> getAst() const;
        void setPackage(std::string& p);
        std::string getPackage() const;
        void setClassOrInterfaceDecl(std::shared_ptr<ClassOrInterfaceDecl> c);
        std::shared_ptr<ClassOrInterfaceDecl> getClassOrInterfaceDecl() const;
        void beginScope();
        void endScope();
};
