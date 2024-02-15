#pragma once
#include <memory>
#include "symbolTable.hpp"
#include "visitor.hpp"

class ContextVisitor: public Visitor {
    private:
        std::shared_ptr<SymbolTable> symbolTable;
        bool error;
    public:
        ContextVisitor();
        void visit(std::shared_ptr<FormalParameter> n) override;
        void visit(std::shared_ptr<Constructor> n) override;
        void visit(std::shared_ptr<Method> n) override;
        void visit(std::shared_ptr<Field> n) override;
        void visit(std::shared_ptr<BlockStatement> n) override;
        void visit(std::shared_ptr<IfStatement> n) override;
        void visit(std::shared_ptr<WhileStatement> n) override;
        void visit(std::shared_ptr<ForStatement> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        bool isError() const;
        std::shared_ptr<SymbolTable> getSymbolTable() const;
};
