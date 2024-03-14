#include <unordered_map>
#include <sstream>
#include <fstream>
#include <stack>
#include "cfg.hpp"

BasicBlock::BasicBlock() {
}

ControlFlowGraph::ControlFlowGraph() {
    start = std::make_shared<BasicBlock>();
    startNode = std::make_shared<Node>("Start");
    endNode = std::make_shared<Node>("End");
    blocks.push_back(start);
    nodes.push_back(startNode);
}

void ControlFlowGraph::addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to) {
    auto edge = std::make_shared<Edge>(from, to);
    from->outgoing.push_back(edge);
    to->incoming.push_back(edge);
    edges.push_back(edge);
}

void ControlFlowGraph::addLink(std::shared_ptr<Node> from, std::shared_ptr<Node> to) {
    auto link = std::make_shared<Link>(from, to);
    from->outgoing.push_back(link);
    to->incoming.push_back(link);
    links.push_back(link);
}

std::shared_ptr<Node> ControlFlowGraph::removeLastLink() {
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

std::unordered_map<std::shared_ptr<Node>, std::set<std::string>> ControlFlowGraph::liveVariableAnalysis() {
    std::unordered_map<std::shared_ptr<Node>, std::set<std::string>> liveAtEntry;
    std::vector<std::shared_ptr<Node>> worklist = nodes;
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
    std::unordered_map<std::shared_ptr<Node>, std::set<std::string>> liveAtEntry = liveVariableAnalysis();

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
//     std::vector<std::shared_ptr<Node>> worklist = nodes;
//     std::unordered_map<std::shared_ptr<Node>, std::set<std::string>> liveAtEntry;
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

// void ControlFlowGraph::DFS(std::shared_ptr<Node> node, std::set<std::shared_ptr<Node>>& visited, std::vector<std::shared_ptr<Node>>& postOrder) {
//     // Mark the current node as visited
//     visited.insert(node);

//     // Recur for all the vertices adjacent to this vertex
//     for (std::shared_ptr<Link> link : node->outgoing) {
//         std::shared_ptr<Node> child = link->to;
//         if (visited.find(child) == visited.end()) {
//             DFS(child, visited, postOrder);
//         }
//     }

//     // Push current vertex to the postOrder vector (post-order)
//     postOrder.push_back(node);
// }

// std::vector<std::shared_ptr<Node>> ControlFlowGraph::generateReversePostOrder(std::shared_ptr<Node> endNode, std::vector<std::shared_ptr<Node>>& nodes) {
//     std::set<std::shared_ptr<Node>> visited;
//     std::vector<std::shared_ptr<Node>> postOrder;

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
