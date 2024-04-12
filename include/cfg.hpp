#pragma once
#include <set>
#include "ast.hpp"

class _Edge;
class Link;

class _BasicBlock {
public:
    std::vector<std::shared_ptr<Statement>> statements;
    std::vector<std::shared_ptr<_Edge>> incoming;
    std::vector<std::shared_ptr<_Edge>> outgoing;
    _BasicBlock();
};

class _Node {
public:
    std::string name;
    std::string def;
    std::set<std::string> use;
    bool init;
    std::vector<std::shared_ptr<Link>> incoming;
    std::vector<std::shared_ptr<Link>> outgoing;
    _Node(const std::string& n) : name(n), init(false) {}
};

class _Edge {
public:
    std::shared_ptr<_BasicBlock> from;
    std::shared_ptr<_BasicBlock> to;
    _Edge(std::shared_ptr<_BasicBlock> f, std::shared_ptr<_BasicBlock> t) : from(f), to(t) {}
};

class Link {
public:
    std::shared_ptr<_Node> from;
    std::shared_ptr<_Node> to;
    Link(std::shared_ptr<_Node> f, std::shared_ptr<_Node> t) : from(f), to(t) {}
};

class ControlFlowGraph {
    public:
        std::shared_ptr<_BasicBlock> start;
        std::shared_ptr<_Node> startNode;
        std::shared_ptr<_Node> endNode;
        std::vector<std::shared_ptr<_BasicBlock>> blocks;
        std::vector<std::shared_ptr<_Node>> nodes;
        std::vector<std::shared_ptr<_Edge>> edges;
        std::vector<std::shared_ptr<Link>> links;
        ControlFlowGraph();
        void addEdge(std::shared_ptr<_BasicBlock> from, std::shared_ptr<_BasicBlock> to);
        void addLink(std::shared_ptr<_Node> from, std::shared_ptr<_Node> to);
        std::shared_ptr<_Node> removeLastLink();
        bool checkReachability();
        bool checkMissingReturn();
        bool checkPathForReturn(const std::shared_ptr<_BasicBlock>& block, std::vector<std::shared_ptr<_BasicBlock>>& visited);
        bool checkDeadAssignments();
        std::unordered_map<std::shared_ptr<_Node>, std::set<std::string>> liveVariableAnalysis();
        bool hasReturnInBlock(const std::shared_ptr<_BasicBlock>& block);
        bool isTerminalBlock(const std::shared_ptr<_BasicBlock>& block);
        void Print();
        void PrintNodes();
        void removeUnusedNodes();
        void mergeUnusedNodes();
};
