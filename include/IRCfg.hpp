#pragma once
#include<unordered_set>
#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <sstream>
#include "IRAst.hpp"

namespace TIR {
    class Edge;

    class BasicBlock {
    public:
        std::vector<std::shared_ptr<Stmt>> statements;
        std::vector<std::shared_ptr<Edge>> incoming;
        std::vector<std::shared_ptr<Edge>> outgoing;
        bool marked = false; 
        BasicBlock();
        int index;
        std::unordered_set<std::string> defs;
        std::unordered_set<std::string> uses;

    };

    class Edge {
    public:
        std::shared_ptr<BasicBlock> from;
        std::shared_ptr<BasicBlock> to;
        Edge(std::shared_ptr<BasicBlock> f, std::shared_ptr<BasicBlock> t) : from(f), to(t) {}
    };

    class CFG {
        public:
            std::shared_ptr<BasicBlock> start; 
            std::vector<std::shared_ptr<BasicBlock>> blocks;
            std::vector<std::shared_ptr<Edge>> edges;        
            
            std::unordered_map<std::shared_ptr<BasicBlock>, std::unordered_set<std::string>> liveIn;
            std::unordered_map<std::shared_ptr<BasicBlock>, std::unordered_set<std::string>> liveOut;

            std::unordered_map<std::string, std::shared_ptr<BasicBlock>> labelToBlock;
            std::vector<std::pair<std::shared_ptr<BasicBlock>, std::string>> pendingJumps;
            std::vector<std::pair<std::shared_ptr<BasicBlock>, std::pair<std::string, std::string>>> pendingCJumps;

            CFG();             
            std::shared_ptr<BasicBlock> newBlock();
            void addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to);
            void Print(const std::string filename="cfg");
            void collectTrace(std::shared_ptr<BasicBlock> block, std::vector<std::shared_ptr<BasicBlock>>& blocks);
            void optimizeJumps(std::vector<std::shared_ptr<BasicBlock>>& trace);
            std::vector<std::shared_ptr<Stmt>> collectTraces();            
            void computeLiveVariables();
            int block_id;                        
    };

}
