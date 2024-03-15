#pragma once
#include <iostream>
#include "visitor.hpp"
#include "cfg.hpp"

class CFGVisitor : public Visitor {
    private:
        ControlFlowGraph cfg;
        DataType currentMethodReturnType;
        std::shared_ptr<BasicBlock> currentBlock;
        std::shared_ptr<Node> node;
        void addNode(std::shared_ptr<Node> node);
        std::shared_ptr<BasicBlock> newBlock();
    public:
        CFGVisitor();
        void visit(std::shared_ptr<BlockStatement> n) override;
        void visit(std::shared_ptr<SemicolonStatement> n) override;
        void visit(std::shared_ptr<IfStatement> n) override;
        void visit(std::shared_ptr<WhileStatement> n) override;
        void visit(std::shared_ptr<ForStatement> n) override;
        void visit(std::shared_ptr<ReturnStatement> n) override;
        void visit(std::shared_ptr<ExpressionStatement> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        void visit(std::shared_ptr<Constructor> n) override;
        void visit(std::shared_ptr<Method> n) override;
};
