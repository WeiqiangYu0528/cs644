#pragma once
#include<unordered_set>
#include "ast.hpp"

class Edge;

class BasicBlock {
public:
    std::vector<std::shared_ptr<Statement>> statements;
    std::vector<std::shared_ptr<Edge>> incoming;
    std::vector<std::shared_ptr<Edge>> outgoing;
    BasicBlock();
};

class Edge {
public:
    std::shared_ptr<BasicBlock> from;
    std::shared_ptr<BasicBlock> to;
    Edge(std::shared_ptr<BasicBlock> f, std::shared_ptr<BasicBlock> t) : from(f), to(t) {}
};


class ControlFlowGraph {
    public:
        std::shared_ptr<BasicBlock> start; 
        std::vector<std::shared_ptr<BasicBlock>> blocks;
        std::vector<std::shared_ptr<Edge>> edges;        
        ControlFlowGraph();
        void addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to);
        bool checkReachability();
        bool checkMissingReturn();
        bool checkPathForReturn(const std::shared_ptr<BasicBlock>& block, std::vector<std::shared_ptr<BasicBlock>>& visited);
        bool hasReturnInBlock(const std::shared_ptr<BasicBlock>& block);
        bool isTerminalBlock(const std::shared_ptr<BasicBlock>& block);
        void Print();
        void removeUnusedNodes();      
};
