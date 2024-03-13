#include <unordered_map>
#include "cfg.hpp"

BasicBlock::BasicBlock() {
}

ControlFlowGraph::ControlFlowGraph() {
    start = std::make_shared<BasicBlock>();
    blocks.push_back(start);
}

void ControlFlowGraph::addEdge(std::vector<std::shared_ptr<BasicBlock>>& from, std::shared_ptr<BasicBlock> to) {
    to->pred = from;
    for (auto f : from) {
        f->succ.push_back(to);
    }
}

//iterative solving for reachability
bool ControlFlowGraph::checkReachability() {
    // step1: initialize out[n] to false for all node n
    std::unordered_map<std::shared_ptr<BasicBlock>, bool> in, out;
    for (auto block : blocks) {
        in[block] = false;
        out[block] = false;
    }
    // step2: repeat until no change to out[n] are possible.
    bool changed{true};
    while (changed) {
        changed = false;
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (i == 0) in[blocks[i]] = true;
            else if (blocks[i]->pred.empty()) in[blocks[i]] = false;
            else {
                for (auto prev : blocks[i]->pred) {
                    in[blocks[i]] = in[blocks[i]] || out[prev];
                }
            }

            bool temp {out[blocks[i]]};
            bool returnStmt{false};
            size_t len = blocks[i]->statements.size();
            for (size_t j = 0; j < len; ++j) {
                auto stmt = blocks[i]->statements[j];
                if (std::dynamic_pointer_cast<ReturnStatement>(stmt)) {
                    returnStmt = true;
                    if (j == len - 1) out[blocks[i]] = false;
                    else return false;
                }
            }
            if (!returnStmt) out[blocks[i]] = in[blocks[i]];
            if (temp != out[blocks[i]]) changed = true;
        }  
    }

    for (auto block : blocks) {
        if (!in[block]) return false;
    }

    return true;
}
