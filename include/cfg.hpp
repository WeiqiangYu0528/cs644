#pragma once
#include "ast.hpp"

class BasicBlock {
    public:
        std::vector<std::shared_ptr<Statement>> statements;
        std::vector<std::shared_ptr<BasicBlock>> pred;
        std::vector<std::shared_ptr<BasicBlock>> succ;
        BasicBlock();
};

class ControlFlowGraph {
    public:
        std::shared_ptr<BasicBlock> start;
        ControlFlowGraph();
        void addEdge(std::vector<std::shared_ptr<BasicBlock>>& from, std::shared_ptr<BasicBlock> to);
};
