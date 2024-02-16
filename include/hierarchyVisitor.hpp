#pragma once
#include <memory>
#include "symbolTable.hpp"
#include "visitor.hpp"

class HierarchyVisitor: public Visitor {
    private:
        std::shared_ptr<SymbolTable> symbolTable;
        bool error;
    public:
        std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> tables;

        HierarchyVisitor(std::shared_ptr<SymbolTable> st, std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> t);
        void visit(std::shared_ptr<Program> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        bool isError() const;
        std::shared_ptr<SymbolTable> getSymbolTable() const;
        std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> getTables() const;
};
