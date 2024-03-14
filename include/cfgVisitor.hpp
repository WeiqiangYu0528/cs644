#pragma once
#include <iostream>
#include <stack>
#include "visitor.hpp"
#include "cfg.hpp"

class CFGVisitor : public Visitor {
    private:
        ControlFlowGraph cfg;
        std::shared_ptr<BasicBlock> block;
        std::vector<std::shared_ptr<BasicBlock>> from;
        std::stack<std::string> stack_t;
        std::unordered_set<std::string> uninitLocal;
        bool error;
    public:
        std::shared_ptr<Scope> scope;
        std::vector<ControlFlowGraph> cfgs;
        CFGVisitor(std::shared_ptr<Scope> s);
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
        void visit(std::shared_ptr<IdentifierExp> n) override;
        void visit(std::shared_ptr<FormalParameter> n) override;
        void visit(std::shared_ptr<Assignment> n) override;
        void visit(std::shared_ptr<ConditionalOrExp> n) override;
        void createBasicBlock();
        void printCFG(const ControlFlowGraph& cfg);
        void beginScope();
        bool endScope();
        bool isError() const;
};
