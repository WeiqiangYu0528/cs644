#include "linearScanner.hpp"

using namespace TIR;



 void LinearScanner::visit(std::shared_ptr<Stmt> stmt, std::shared_ptr<BasicBlock> block) {
    if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
        visit(cjump, block);
    } else if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
        visit(jump, block);
    } else if (auto move = std::dynamic_pointer_cast<Move>(stmt)) {
        visit(move, block);
    } else if (auto ret = std::dynamic_pointer_cast<Return>(stmt)) {
        visit(ret, block);
    } else if (auto call = std::dynamic_pointer_cast<Call_s>(stmt)) {
        visit(call, block);
    }
}

void LinearScanner::visit(std::shared_ptr<Jump> jump, std::shared_ptr<BasicBlock> block) {
    visit(jump->getTarget(), block);
}

void LinearScanner::visit(std::shared_ptr<CJump> cjump, std::shared_ptr<BasicBlock> block) {
    visit(cjump->getCond(), block);
}

void LinearScanner::visit(std::shared_ptr<Move> move, std::shared_ptr<BasicBlock> block) {
    isDefContext = false;
    visit(move->getSource(), block);
    isDefContext = true;
    visit(move->getTarget(), block);
    isDefContext = false;
}

void LinearScanner::visit(std::shared_ptr<Return> ret, std::shared_ptr<BasicBlock> block) {
    visit(ret->getRet(), block);
}

void LinearScanner::visit(std::shared_ptr<Call_s> call, std::shared_ptr<BasicBlock> block) {
    for (int i = call->getNumArgs() - 1; i >= 0; --i) {
        visit(call->getArgs()[i], block);
    }    
    visit(call->getTarget(), block);
}

void LinearScanner::visit(std::shared_ptr<BinOp> node, std::shared_ptr<BasicBlock> block) {   
    visit(node->getRight(), block);
    visit(node->getLeft(), block);
}

void LinearScanner::visit(std::shared_ptr<Mem> node, std::shared_ptr<BasicBlock> block) {      
    visit(node->getExpr(), block);
}

void LinearScanner::visit(std::shared_ptr<Temp> node, std::shared_ptr<BasicBlock> block) {   
    if(node->getName().starts_with(TIR::Configuration::ABSTRACT_ARG_PREFIX) || node->getName() == TIR::Configuration::ABSTRACT_RET)
        return;
    
    if (isDefContext) {
        block->defs.insert(node->getName());
    } else {
        block->uses.insert(node->getName());
    }
}


void LinearScanner::visit(std::shared_ptr<Expr> node, std::shared_ptr<BasicBlock> block) {
     if (auto temp = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        visit(temp, block);
    } else if (auto binOp = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        visit(binOp, block);
    } else if (auto mem = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        visit(mem, block);
    } 
}

void LinearScanner::allocateRegisters(std::set<std::string> available_registers) {
    for(auto & block: cfg.blocks){
        for(auto& stmt: block->statements) {
            visit(stmt, block);
        }
    }

    cfg.computeLiveVariables();

    std::unordered_map<std::string, Interval> intervals;

    for (auto& block : cfg.blocks) {
        for (const auto& var : cfg.liveOut[block]) {
            if (intervals.find(var) == intervals.end()) {
                intervals[var] = {var, block->index, block->index};
            }        
            intervals[var].end = std::max(intervals[var].end, block->index);
        }
    }

    for (auto& block : cfg.blocks) {
        for (const auto& var : cfg.liveIn[block]) {
            if (intervals.find(var) == intervals.end()) {
                intervals[var] = {var, block->index, block->index};
            }
            intervals[var].start = std::min(intervals[var].start, block->index);
        }
    }

    std::vector<Interval> live_intervals;
    free_registers = available_registers;

    for (auto& p : intervals) {
        live_intervals.push_back({p.first, p.second.start, p.second.end});
    }


    std::sort(live_intervals.begin(), live_intervals.end(), [](const Interval& a, const Interval& b) {
        return a.start < b.start;
    });


 for (auto& interval : live_intervals) {
        expireOldIntervals(interval);
        if (active.size() == available_registers.size()) {
            spillAtInterval(interval);
        } else {
            std::string reg = *free_registers.begin();
            free_registers.erase(reg);
            register_allocation[interval.name] = reg;
            insertIntervalSorted(interval);
        }
    }

}

void LinearScanner::insertIntervalSorted(const Interval& interval) {
    auto it = std::upper_bound(active.begin(), active.end(), interval, [](const Interval& a, const Interval& b) {
        return a.end < b.end;
    });
    active.insert(it, interval);
}


void LinearScanner::expireOldIntervals(Interval& current_interval) {
    auto it = std::remove_if(active.begin(), active.end(),
                                [&current_interval, this](Interval& interval) {
                                    if (interval.end < current_interval.start) {
                                        free_registers.insert(register_allocation[interval.name]);
                                        register_allocation.erase(interval.name);
                                        return true;
                                    }
                                    return false;
                                });
    active.erase(it, active.end());
}


void LinearScanner::spillAtInterval(Interval& interval) {
    if (!active.empty() && active.back().end > interval.end) {
        Interval& top = active.back();
        register_allocation[interval.name] = register_allocation[top.name];
        register_allocation.erase(top.name);
        active.pop_back();
        insertIntervalSorted(interval);
    } else {

        if (free_registers.empty()) {
            spills.insert(interval.name);
        } else {

            std::string reg = *free_registers.begin();
            free_registers.erase(reg);
            register_allocation[interval.name] = reg;
            insertIntervalSorted(interval);
        }
    }
}


void LinearScanner::visit(std::shared_ptr<Label> node, std::shared_ptr<BasicBlock> block) {}

void LinearScanner::visit(std::shared_ptr<Const> node, std::shared_ptr<BasicBlock> block) {}

void LinearScanner::visit(std::shared_ptr<Name> node, std::shared_ptr<BasicBlock> block) {}


// void LinearScanner::visit(std::shared_ptr<Temp> node) {
//     if(node->getName().starts_with(Configuration::ABSTRACT_ARG_PREFIX)) return;  
//     if(node->getName() == TIR::Configuration::VTABLE) return;
//     if(node->getName() == TIR::Configuration::ABSTRACT_RET) return;
//     updateLiveInterval(node->getName(), currentPos);   
//     // std::cout << node->getName() << std::endl;
// }
