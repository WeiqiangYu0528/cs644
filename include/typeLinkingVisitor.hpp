#pragma once
#include <memory>
#include "symbolTable.hpp"
#include "visitor.hpp"

class TypeLinkingVisitor : public Visitor {
    private:
        std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& tables;
        std::unordered_map<std::string, std::shared_ptr<SymbolTable>> scopes;
        bool error;
    public:
        TypeLinkingVisitor(std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& t);
        void visit(std::shared_ptr<ImportStatements> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        void visit(std::shared_ptr<Program> n) override;
        bool isError() const;
};
