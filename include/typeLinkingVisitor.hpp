#pragma once
#include <memory>
#include <unordered_set>
#include "symbolTable.hpp"
#include "visitor.hpp"

class TypeLinkingVisitor : public Visitor {
    private:
        std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& tables;
        std::unordered_map<std::string, std::shared_ptr<SymbolTable>> scopes;
        std::unordered_set<std::string> ambiguousNames;
        std::unordered_map<std::string, std::unordered_set<std::string>> onDemandImported;
        std::unordered_set<std::string> singleImported;        
        bool error;
    public:
        TypeLinkingVisitor(std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>& t);
        void visit(std::shared_ptr<ImportStatements> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        void visit(std::shared_ptr<Program> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        bool isError() const;

        std::string currentPackageName;
        std::string currentClassName;   

};
