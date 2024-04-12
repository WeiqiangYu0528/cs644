#include <unordered_map>
#include <sstream>
#include <fstream>
#include <stack>
#include <list>
#include "cfg.hpp"
#include <unordered_set>
#include <queue>

_BasicBlock::_BasicBlock() {
}

ControlFlowGraph::ControlFlowGraph() {
    start = std::make_shared<_BasicBlock>();
    startNode = std::make_shared<_Node>("Start");
    endNode = std::make_shared<_Node>("End");
    blocks.push_back(start);
    nodes.push_back(startNode);
}

void ControlFlowGraph::addEdge(std::shared_ptr<_BasicBlock> from, std::shared_ptr<_BasicBlock> to) {
    auto edge = std::make_shared<_Edge>(from, to);
    from->outgoing.push_back(edge);
    to->incoming.push_back(edge);
    edges.push_back(edge);
}

void ControlFlowGraph::addLink(std::shared_ptr<_Node> from, std::shared_ptr<_Node> to) {
    auto link = std::make_shared<Link>(from, to);
    from->outgoing.push_back(link);
    to->incoming.push_back(link);
    links.push_back(link);
}

std::shared_ptr<_Node> ControlFlowGraph::removeLastLink() {
    auto link = links.back();
    auto from = link->from;
    link->from->outgoing.pop_back();
    link->to->incoming.pop_back();
    links.pop_back();
    return from;
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
    std::unordered_set<std::shared_ptr<_BasicBlock>> visited;
    std::queue<std::shared_ptr<_BasicBlock>> queue;

    queue.push(start);

    while (!queue.empty()) {
        auto currentBlock = queue.front();
        queue.pop();

        if (visited.find(currentBlock) != visited.end()) {
            continue; 
        }
        visited.insert(currentBlock);
        if (!currentBlock->statements.empty()) {
            for (size_t i = 0; i < currentBlock->statements.size(); ++i) {
                auto stmt = currentBlock->statements[i];
                if (std::dynamic_pointer_cast<ReturnStatement>(stmt) != nullptr) {
                    if (i != currentBlock->statements.size() - 1) {
                        return false; 
                    }
                    goto NEXT_BLOCK;
                }
            }
        }
        for (const auto& edge : currentBlock->outgoing) {
            queue.push(edge->to);
        }

        NEXT_BLOCK:; 
    }

    for (const auto& block : blocks) {
        if (block->outgoing.empty() && !block->statements.empty() && visited.find(block) == visited.end()) {
            return false;
        }
    }
    return true;
}

// bool ControlFlowGraph::checkReachability() {
//     std::unordered_map<std::shared_ptr<_BasicBlock>, bool> in, out;
    
//     for (auto& block : blocks) {
//         in[block] = false;
//         out[block] = false;
//     }
    
//     bool changed = true;
//     while (changed) {
//         changed = false;

//         for (auto& block : blocks) {
//             if (block == start) {
//                 in[block] = true;
//             } else if (block->incoming.empty()) in[block] = false;
//             else 
//              {
//                 for (auto& edge : block->incoming) {
//                     in[block] = in[block] || out[edge->from];
//                 }
//             }
            
//             bool temp = out[block];
//             bool returnStmt {false};
//             for (auto& stmt : block->statements) {
//                 if (std::dynamic_pointer_cast<ReturnStatement>(stmt)) {
//                     returnStmt = true;
//                     if (stmt == block->statements.back()) out[block] = false;
//                     else return false; 
//                 }
//             }
//             if (!returnStmt) out[block] = in[block];
//             if (out[block] != temp) changed = true;
//         }
//     }

//     for (auto& block : blocks) {
//         if (!in[block] && !(block->outgoing.empty() && block->statements.empty())) return false;
//     }

//     return true;
// }

bool ControlFlowGraph::isTerminalBlock(const std::shared_ptr<_BasicBlock>& block) {
    
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
                                   [&](const std::shared_ptr<_Edge>& e) { return e->to == block; }),
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

void ControlFlowGraph::mergeUnusedNodes() {
    bool merged = true;

    while (merged) {
        merged = false;
        std::list<std::shared_ptr<_BasicBlock>> blocksToRemove;

        for (auto blockIt = blocks.begin(); blockIt != blocks.end(); ++blockIt) {
            auto& block = *blockIt;

            if (block->statements.empty() && block->outgoing.size() == 1) {
                auto nextBlock = block->outgoing.front()->to;

                if (nextBlock->statements.empty()) {
                    for (auto& incomingEdge : block->incoming) {
                        auto sourceBlock = incomingEdge->from;

                        for (auto& outgoingEdge : sourceBlock->outgoing) {
                            if (outgoingEdge->to == block) {
                                outgoingEdge->to = nextBlock;
                            }
                        }
                        nextBlock->incoming.push_back(incomingEdge);
                    }

                    blocksToRemove.push_back(block);
                    merged = true;
                }
            }
        }
        for (auto& toRemove : blocksToRemove) {
            auto removeEdges = [this, &toRemove](const std::shared_ptr<_BasicBlock>& block, bool outgoing) {
                auto& edgeList = outgoing ? block->outgoing : block->incoming;
                edgeList.erase(
                    std::remove_if(edgeList.begin(), edgeList.end(), 
                                   [&toRemove, outgoing](const std::shared_ptr<_Edge>& e) {
                                       return outgoing ? e->to == toRemove : e->from == toRemove;
                                   }), 
                    edgeList.end());
            };

            for (auto& block : blocks) {
                removeEdges(block, true); 
                removeEdges(block, false);
            }

            blocks.erase(std::remove(blocks.begin(), blocks.end(), toRemove), blocks.end());        
            edges.erase(
                std::remove_if(edges.begin(), edges.end(),
                               [&toRemove](const std::shared_ptr<_Edge>& e) {
                                   return e->from == toRemove || e->to == toRemove;
                               }),
                edges.end());
        }
    }
}

bool ControlFlowGraph::hasReturnInBlock(const std::shared_ptr<_BasicBlock>& block) {
    return !block->statements.empty() && std::dynamic_pointer_cast<ReturnStatement>(block->statements.back()) != nullptr;
}


bool ControlFlowGraph::checkPathForReturn(const std::shared_ptr<_BasicBlock>& block, std::vector<std::shared_ptr<_BasicBlock>>& visited) {
    if (std::find(visited.begin(), visited.end(), block) != visited.end()) {
        return true;
    }
    visited.push_back(block);

    if (isTerminalBlock(block)) {
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
    
    for (const auto& block : blocks) {
        
        if (block->outgoing.empty()) {
            if (block->statements.empty()) {
                for (const auto& edge : block->incoming) {
                    auto predBlock = edge->from;
                    if (!hasReturnInBlock(predBlock)) {
                        return false; 
                    }
                }
            } else {

                if (!hasReturnInBlock(block)) {
                    return false;
                }
            }
        }
    }
    return true;
}
std::unordered_map<std::shared_ptr<_Node>, std::set<std::string>> ControlFlowGraph::liveVariableAnalysis() {
    std::unordered_map<std::shared_ptr<_Node>, std::set<std::string>> liveAtEntry;
    std::vector<std::shared_ptr<_Node>> worklist = nodes;
    bool changed = true;

    while (changed) {
        changed = false;
        for (auto it = worklist.rbegin(); it != worklist.rend(); ++it) {
            auto node = *it;
            std::set<std::string> liveVariables = liveAtEntry[node];

            if (node->def != "") {
                liveVariables.erase(node->def);
            }
            liveVariables.insert(node->use.begin(), node->use.end());

            for (auto link : node->incoming) {
                auto pred = link->from;
                std::set<std::string> newLiveAtEntry;
                std::set_union(liveVariables.begin(), liveVariables.end(),
                                liveAtEntry[pred].begin(), liveAtEntry[pred].end(),
                                std::inserter(newLiveAtEntry, newLiveAtEntry.begin()));

                if (newLiveAtEntry.size() > liveAtEntry[pred].size()) {
                    liveAtEntry[pred] = newLiveAtEntry;
                    changed = true;
                }
            }
        }
    }
    return liveAtEntry;
}

bool ControlFlowGraph::checkDeadAssignments() {
    // First, perform live variable analysis
    std::unordered_map<std::shared_ptr<_Node>, std::set<std::string>> liveAtEntry = liveVariableAnalysis();

    // Now, check for dead assignments
    for (auto& node : nodes) {
        const std::set<std::string>& liveVariables = liveAtEntry[node];

        // Report dead assignments
        if (!node->init && node->def != "" && !liveVariables.contains(node->def)) {
            std::cerr << "Dead assignment found for variable: " << node->def << " at block " << node.get() << std::endl;
            return false;
        }
    }
    return true;
}

// bool ControlFlowGraph::checkDeadAssignments() {
//     std::vector<std::shared_ptr<_Node>> worklist = nodes;
//     std::unordered_map<std::shared_ptr<_Node>, std::set<std::string>> liveAtEntry;
//     while (!worklist.empty()) {
//         auto node = worklist.back();
//         worklist.pop_back();
//         std::cout << node.get() << std::endl;

//         std::set<std::string> liveVariables = liveAtEntry[node];

//         if (node->def != ""  && !liveVariables.contains(node->def)) {
//             for (auto var : liveVariables) std::cout << "var: " << var << " ";
//             if (liveVariables.empty()) std::cout << "empty" << std::endl;;
//             std::cerr << node.get() << std::endl;
//             std::cout << "Dead assignment found for variable: " << node->def << std::endl;
//             return false;
//         }

//         if (node->def != "") {
//             liveVariables.erase(node->def);
//         }
//         liveVariables.insert(node->use.begin(), node->use.end());

//         // Propagate live variable information to predecessors
//         for (auto link : node->incoming) {
//             auto pred = link->from;
//             std::set<std::string> newLiveAtEntry;
//             std::set_union(liveVariables.begin(), liveVariables.end(), liveAtEntry[pred].begin(), liveAtEntry[pred].end(), std::inserter(newLiveAtEntry, newLiveAtEntry.begin()));

//             if (newLiveAtEntry.size() > liveAtEntry[pred].size()) {
//                 liveAtEntry[pred] = newLiveAtEntry;
//                 // Avoiding duplicates when adding to worklist
//                 if (std::find(worklist.begin(), worklist.end(), pred) == worklist.end()) {
//                     worklist.push_back(pred);
//                 }
//             }
//         }
//     }
//     return true;
// }

void ControlFlowGraph::PrintNodes() {
    std::ostringstream dotGraph;
    dotGraph << "digraph CFG {" << std::endl;

    for (auto& node : nodes) {
        std::ostringstream t;
        t << "BB: " << node.get();
        t << "\\n def: " << node->def;
        t << "\\n use:";
        for (std::string str: node->use) {
            t << " " << str;
        }
        t << "\\n" << node->name;
        dotGraph << "    \"" << node.get() << "\" [label=\"" << t.str() << "\"];" << std::endl;
    }

    for (auto& link : links) {
        dotGraph << "    \"" << link->from.get() << "\" -> \"" << link->to.get() << "\" [label=\"" << link.get() << "\"];" << std::endl;
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

// void ControlFlowGraph::DFS(std::shared_ptr<_Node> node, std::set<std::shared_ptr<_Node>>& visited, std::vector<std::shared_ptr<_Node>>& postOrder) {
//     // Mark the current node as visited
//     visited.insert(node);

//     // Recur for all the vertices adjacent to this vertex
//     for (std::shared_ptr<Link> link : node->outgoing) {
//         std::shared_ptr<_Node> child = link->to;
//         if (visited.find(child) == visited.end()) {
//             DFS(child, visited, postOrder);
//         }
//     }

//     // Push current vertex to the postOrder vector (post-order)
//     postOrder.push_back(node);
// }

// std::vector<std::shared_ptr<_Node>> ControlFlowGraph::generateReversePostOrder(std::shared_ptr<_Node> endNode, std::vector<std::shared_ptr<_Node>>& nodes) {
//     std::set<std::shared_ptr<_Node>> visited;
//     std::vector<std::shared_ptr<_Node>> postOrder;

//     // Call the recursive helper function to store the post-order traversal starting from the end node
//     DFS(endNode, visited, postOrder);

//     // If there are nodes not reachable from the end node (e.g., due to loops), they need to be processed as well.
//     for (auto& node : nodes) {
//         if (visited.find(node) == visited.end()) {
//             DFS(node, visited, postOrder);
//         }
//     }

//     // Reverse the post-order to get the reverse post-order
//     std::reverse(postOrder.begin(), postOrder.end());

//     return postOrder;
// }
