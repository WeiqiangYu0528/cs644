#pragma once
#include <set>
#include "ast.hpp"

class Edge;
class Link;

class BasicBlock {
public:
    std::vector<std::shared_ptr<Statement>> statements;
    std::vector<std::shared_ptr<Edge>> incoming;
    std::vector<std::shared_ptr<Edge>> outgoing;
    BasicBlock();
};

class Node {
public:
    std::string name;
    std::string def;
    std::set<std::string> use;
    std::vector<std::shared_ptr<Link>> incoming;
    std::vector<std::shared_ptr<Link>> outgoing;
    Node(const std::string& n) : name(n) {}
};

class Edge {
public:
    std::shared_ptr<BasicBlock> from;
    std::shared_ptr<BasicBlock> to;
    Edge(std::shared_ptr<BasicBlock> f, std::shared_ptr<BasicBlock> t) : from(f), to(t) {}
};

class Link {
public:
    std::shared_ptr<Node> from;
    std::shared_ptr<Node> to;
    Link(std::shared_ptr<Node> f, std::shared_ptr<Node> t) : from(f), to(t) {}
};

class ControlFlowGraph {
    public:
        std::shared_ptr<BasicBlock> start;
        std::shared_ptr<Node> startNode;
        std::shared_ptr<Node> endNode;
        std::vector<std::shared_ptr<BasicBlock>> blocks;
        std::vector<std::shared_ptr<Node>> nodes;
        std::vector<std::shared_ptr<Edge>> edges;
        std::vector<std::shared_ptr<Link>> links;
        ControlFlowGraph();
        void addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to);
        void addLink(std::shared_ptr<Node> from, std::shared_ptr<Node> to);
        std::shared_ptr<Node> removeLastLink();
        bool checkReachability();
        bool checkMissingReturn();
        bool checkPathForReturn(const std::shared_ptr<BasicBlock>& block, std::vector<std::shared_ptr<BasicBlock>>& visited);
        bool checkDeadAssignments();
        bool hasReturnInBlock(const std::shared_ptr<BasicBlock>& block);
        bool isTerminalBlock(const std::shared_ptr<BasicBlock>& block);
        void Print();
        void PrintNodes();
        void removeUnusedNodes();      
};
