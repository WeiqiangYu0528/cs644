#include "IRCfg.hpp"


namespace TIR {
    BasicBlock::BasicBlock() {}

    CFG::CFG() {}

    std::shared_ptr<BasicBlock> CFG::newBlock() {
        auto newBlock = std::make_shared<BasicBlock>();
        blocks.push_back(newBlock);
        return newBlock;
    }

    void CFG::addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to) {
        auto edge = std::make_shared<Edge>(from, to);
        from->outgoing.push_back(edge);
        to->incoming.push_back(edge);
        edges.push_back(edge);
    }

    void CFG::Print(const std::string filename) {
        std::ostringstream dotGraph;
        dotGraph << "digraph CFG {" << std::endl;

        for (auto& block : blocks) {
            std::ostringstream t;
            t << "BB: " << block.get();
            if (!block->statements.empty()) {
                t << "\\n"; 
                for (const auto& stmt : block->statements) {
                    t << stmt->getLabel();
                    if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
                        t << " " << cjump->getTrueLabel();
                        if(cjump->hasFalseLabel()) 
                            t << " " << cjump->getFalseLabel();
                    }              
                    if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
                        t << " " << jump->getTarget()->getLabel();
                    }                           
                    t << "\\n";
                }
            }
            dotGraph << "    \"" << block.get() << "\" [label=\"" << t.str() << "\"];" << std::endl;
        }


        for (auto& edge : edges) {
            dotGraph << "    \"" << edge->from.get() << "\" -> \"" << edge->to.get() << "\";" << std::endl;
        }
        dotGraph << "}" << std::endl;

        std::string dotFile = filename + ".dot";
        std::string pngFile = filename + ".png";
        std::ofstream out(dotFile);
        out << dotGraph.str();
        out.close();
        std::string command = "dot -Tpng " + dotFile + " -o " + pngFile;
        system(command.c_str());
        std::remove(dotFile.c_str());
    }

    void CFG::collectTrace(std::shared_ptr<BasicBlock> block, std::vector<std::shared_ptr<Stmt>>& stmts) {
        if (!block || block->marked) return;

        block->marked = true;
        for (auto& stmt : block->statements) {
            stmts.push_back(stmt);
        }

        if (!block->outgoing.empty()) {
            if (std::dynamic_pointer_cast<CJump>(block->statements.back()) && block->outgoing.size() > 1) {
                collectTrace(block->outgoing[1]->to, stmts);
            }
            else
                collectTrace(block->outgoing[0]->to, stmts); 
        }
    }



    std::vector<std::shared_ptr<Stmt>> CFG::collectTraces() {
        std::vector<std::shared_ptr<Stmt>> collectedStmts;
        for (auto& block : blocks) {
            if (!block->marked) {
                std::vector<std::shared_ptr<Stmt>> stmts;
                collectTrace(block, stmts);          
                optimizeJumps(stmts);
                collectedStmts.insert(collectedStmts.end(), stmts.begin(), stmts.end());
            }

        }
        std::unordered_set<std::string> referencedLabels;
        for (auto& stmt : collectedStmts) {
            if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
                
                referencedLabels.insert(std::dynamic_pointer_cast<Name>(jump->getTarget())->getName());

            } else if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
                referencedLabels.insert(cjump->getTrueLabel());
            }
        }
        // Remove LABELs not referenced
        for (auto it = collectedStmts.begin(); it != collectedStmts.end(); ) {
            if(it != collectedStmts.begin()) {
                if (auto label = std::dynamic_pointer_cast<Label>(*it)) {
                    if (!referencedLabels.contains(label->getName())) {
                        it = collectedStmts.erase(it);
                        continue;
                    }
                }
            }
            ++it;
        }
        return collectedStmts;
    }

    void CFG::optimizeJumps(std::vector<std::shared_ptr<Stmt>>& stmts) {
        // Remove unnecessary JUMP
        for (auto it = stmts.begin(); it != stmts.end(); ) {
            if (auto jump = std::dynamic_pointer_cast<Jump>(*it)) {
                auto nextIt = std::next(it);
                if (nextIt != stmts.end()) {
                    auto label = std::dynamic_pointer_cast<Label>(*nextIt);
                    auto target = std::dynamic_pointer_cast<Name>(jump->getTarget());
                    if (label->getName() == target->getName()) {
                        it = stmts.erase(it); 
                        continue;  
                    }
                }
            }
            ++it;
        }
        // Convert CJUMP and add necessary JUMP
        for (auto it = stmts.begin(); it != stmts.end(); ++it) {
            if (auto cjump = std::dynamic_pointer_cast<CJump>(*it)) {
                if(cjump->hasFalseLabel()) {
                    *it = std::make_shared<CJump>(cjump->getCond(), cjump->getTrueLabel(), "");
                    auto trueBlock = labelToBlock[cjump->getTrueLabel()];
                    auto falseBlock = labelToBlock[cjump->getFalseLabel()];
                    if(trueBlock->outgoing.size() > 0 && trueBlock->outgoing[0]->to == falseBlock) {
                        trueBlock->statements.push_back(
                            std::make_shared<Jump>(std::make_shared<Name>(cjump->getFalseLabel()))
                        );
                    }
                }

            }
        }
      
    }

}