#include "cfg.hpp"

BasicBlock::BasicBlock() {
}

ControlFlowGraph::ControlFlowGraph() {
    start = std::make_shared<BasicBlock>();
}

void ControlFlowGraph::addEdge(std::vector<shared_ptr<BasicBlock>> from&, std::shared_ptr<BasicBlock> to) {
    to->pred = from;
    for (auto f : from) {
        f->succ.push_back(to);
    }
}
