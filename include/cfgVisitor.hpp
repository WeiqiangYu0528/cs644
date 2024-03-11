#pragma once
#include "visitor.hpp"
#include "cfg.hpp"

class CFGVisitor : public Visitor {
    private:
        ControlFlowGraph cfg;
        std::shared_ptr<BasicBlock> block;
        std::vector<std::shared_ptr<BasicBlock>> from;
    public:
        std::vector<ControlFlowGraph> cfgs;
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
