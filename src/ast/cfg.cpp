#include <unordered_map>
#include <sstream>
#include <fstream>
#include <stack>
#include "cfg.hpp"

BasicBlock::BasicBlock() {
}

ControlFlowGraph::ControlFlowGraph() {
    start = std::make_shared<BasicBlock>();
    blocks.push_back(start);
}

void ControlFlowGraph::addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to) {
    auto edge = std::make_shared<Edge>(from, to);
    from->outgoing.push_back(edge);
    to->incoming.push_back(edge);
    edges.push_back(edge);
}

void ControlFlowGraph::Print() {
    std::ostringstream dotGraph;
    dotGraph << "digraph CFG {" << std::endl;

    for (auto& block : blocks) {
        std::ostringstream t;
        t << "BB: " << block.get();
        if (!block->statements.empty()) {
            t << "\\n"; 
            for (const auto& stmt : block->statements) {
                t << stmt->getStmtName() << "\\n";
            }
        }
        dotGraph << "    \"" << block.get() << "\" [label=\"" << t.str() << "\"];" << std::endl;
    }


    for (auto& edge : edges) {
        dotGraph << "    \"" << edge->from.get() << "\" -> \"" << edge->to.get() << "\" [label=\"" << edge.get() << "\"];" << std::endl;
    }
    dotGraph << "}" << std::endl;

    std::string dotFile = "cfg.dot";
    std::string pngFile = "cfg.png";
    std::ofstream out(dotFile);
    out << dotGraph.str();
    out.close();
    std::string command = "dot -Tpng " + dotFile + " -o " + pngFile;
    system(command.c_str());
    std::remove(dotFile.c_str());
}


bool ControlFlowGraph::checkReachability() {
    std::unordered_map<std::shared_ptr<BasicBlock>, bool> in, out;
    
    for (auto& block : blocks) {
        in[block] = false;
        out[block] = false;
    }
    
    bool changed = true;
    while (changed) {
        changed = false;

        for (auto& block : blocks) {
            if (block == start) {
                in[block] = true;
            } else if (block->incoming.empty()) in[block] = false;
            else 
             {
                for (auto& edge : block->incoming) {
                    in[block] = in[block] || out[edge->from];
                }
            }
            
            bool temp = out[block];
            bool returnStmt {false};
            for (auto& stmt : block->statements) {
                if (std::dynamic_pointer_cast<ReturnStatement>(stmt)) {
                    returnStmt = true;
                    if (stmt == block->statements.back()) out[block] = false;
                    else return false; 
                }
            }
            if (!returnStmt) out[block] = in[block];
            if (out[block] != temp) changed = true;
        }
    }

    for (auto& block : blocks) {
        if (!in[block] && !(block->outgoing.empty() && block->statements.empty())) return false;
    }

    return true;
}

bool ControlFlowGraph::isTerminalBlock(const std::shared_ptr<BasicBlock>& block) {
    if (hasReturnInBlock(block)) {
        if (block->outgoing.size() == 1) {
            auto current = block->outgoing[0]->to;
            while (1) {
                if (current->outgoing.size() == 1 && current->statements.empty())
                    current = current->outgoing[0]->to;
                else 
                    return false;
            }
            return current->outgoing.empty() && current->statements.empty();
        }
    }
    return false;
}

void ControlFlowGraph::removeUnusedNodes() {
    auto blockIt = blocks.begin();
    while (blockIt != blocks.end()) {
        auto block = *blockIt;
        if (block->outgoing.empty() && block->statements.empty()) {
            for (auto& incomingEdge : block->incoming) {
                auto sourceBlock = incomingEdge->from;
                sourceBlock->outgoing.erase(
                    std::remove_if(sourceBlock->outgoing.begin(), sourceBlock->outgoing.end(),
                                   [&](const std::shared_ptr<Edge>& e) { return e->to == block; }),
                    sourceBlock->outgoing.end());
                edges.erase(
                    std::remove(edges.begin(), edges.end(), incomingEdge),
                    edges.end());
            }
            blockIt = blocks.erase(blockIt);
        } else {
            ++blockIt;
        }
    }
}
bool ControlFlowGraph::hasReturnInBlock(const std::shared_ptr<BasicBlock>& block) {
    return !block->statements.empty() && std::dynamic_pointer_cast<ReturnStatement>(block->statements.back()) != nullptr;
}


bool ControlFlowGraph::checkPathForReturn(const std::shared_ptr<BasicBlock>& block, std::vector<std::shared_ptr<BasicBlock>>& visited) {
    if (std::find(visited.begin(), visited.end(), block) != visited.end()) {
        return true;
    }
    visited.push_back(block);

    if (isTerminalBlock(block)) {
        return true;
    }

    if (block->outgoing.empty()) {
        return hasReturnInBlock(block);
    }

    for (const auto& edge : block->outgoing) {
        if (!checkPathForReturn(edge->to, visited)) {
            return false;
        }
    }

    return true;
}

bool ControlFlowGraph::checkMissingReturn() {
    std::vector<std::shared_ptr<BasicBlock>> visited;
    return checkPathForReturn(start, visited);
}