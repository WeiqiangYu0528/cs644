#pragma once
#include <iostream>
#include <stack>
#include "visitor.hpp"
#include "cfg.hpp"

class CFGVisitor : public Visitor {
    private:
        ControlFlowGraph cfg;
        DataType currentMethodReturnType;
        std::shared_ptr<BasicBlock> currentBlock;
        std::shared_ptr<Node> currentNode;
        bool isInsideIf;
        bool isInsideReturn;
        void addStatement(std::shared_ptr<Statement> stmt);
        std::shared_ptr<BasicBlock> newBlock();
        void createNewNode(const std::string& name);
    public:
        std::shared_ptr<Scope> scope;
        std::vector<ControlFlowGraph> cfgs;
        std::stack<std::string> stack_t;
        std::unordered_set<std::string> ltable;
        std::unordered_set<std::string> uninitLocal;

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
        bool evaluateFalse(std::shared_ptr<AstNode> n);
        void beginScope();
        void endScope();
};
