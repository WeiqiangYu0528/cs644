#include "linearScanner.hpp"

using namespace TIR;

LinearScanner::LinearScanner() {
}

void LinearScanner::visit(std::shared_ptr<FuncDecl> fd) {
    std::shared_ptr<Seq> seq = std::dynamic_pointer_cast<Seq>(fd->body);
    if(seq) visit(seq);
}

void LinearScanner::visit(std::shared_ptr<Seq> seq) {
    for (auto& stmt : seq->getStmts()) {
        currentPos++;
        visit(stmt);
    }

}

 void LinearScanner::visit(std::shared_ptr<Stmt> stmt) {
    if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
        visit(cjump);
    } else if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
        visit(jump);
    } else if (auto move = std::dynamic_pointer_cast<Move>(stmt)) {
        visit(move);
    } else if (auto ret = std::dynamic_pointer_cast<Return>(stmt)) {
        visit(ret);
    } else if (auto call = std::dynamic_pointer_cast<Call_s>(stmt)) {
        visit(call);
    }
}

void LinearScanner::visit(std::shared_ptr<Jump> jump) {
    visit(jump->getTarget());
}

void LinearScanner::visit(std::shared_ptr<CJump> cjump) {
    visit(cjump->getCond());
}

void LinearScanner::visit(std::shared_ptr<Move> move) {
    visit(move->getSource());
    visit(move->getTarget());
}

void LinearScanner::visit(std::shared_ptr<Return> ret) {
    visit(ret->getRet());
}

void LinearScanner::visit(std::shared_ptr<Call_s> call) {
    visit(call->getTarget());
    for (auto& arg: call->getArgs())
        visit(arg);
}

void LinearScanner::visit(std::shared_ptr<BinOp> node) {   
    visit(node->getLeft());
    visit(node->getRight());
}

void LinearScanner::visit(std::shared_ptr<Mem> node) {      
    visit(node->getExpr());
}

void LinearScanner::visit(std::shared_ptr<Temp> node) {
    // std::cout  << "ls:" << node->getName() << std::endl;
    updateLiveInterval(node->getName(), currentPos);   
}


void LinearScanner::visit(std::shared_ptr<Expr> node) {
     if (auto temp = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        visit(temp);
    } else if (auto binOp = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        visit(binOp);
    } else if (auto mem = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        visit(mem);
    } 
}

void LinearScanner::updateLiveInterval(const std::string& name, int position) {
    if(!live_intervals_start.contains(name)) {
        live_intervals_start[name] = position;
    }
    live_intervals_end[name] = position;
}

void LinearScanner::allocateRegisters(std::set<std::string> available_registers) {
    std::vector<Interval> intervals;
    free_registers = available_registers;

    for (auto& p : live_intervals_start) {
        intervals.push_back({p.first, p.second, live_intervals_end[p.first]});
    }

    std::sort(intervals.begin(), intervals.end(), [](const Interval& a, const Interval& b) {
        return a.start < b.start;
    });


    for (auto& interval : intervals) {
       
        expireOldIntervals(interval);
        if(active.size() == available_registers.size()) {
            spillAtInterval(interval);
        }
        else
        {
            std::string reg = *free_registers.begin();
            free_registers.erase(reg);     
            register_allocation[interval.name] = reg;
            active.insert(interval);
        }
    }
}

void LinearScanner::expireOldIntervals(const Interval& current_interval) {
    for (auto it = active.begin(); it != active.end(); ) {
        if (it->end >= current_interval.start)
            return;
        auto name = it->name;
        free_registers.insert(register_allocation[name]);
        it = active.erase(it);                                
    }
}

void LinearScanner::spillAtInterval(const Interval& interval) {
    auto it = std::prev(active.end());
    Interval spill = *it;
    if (spill.end > interval.end) {
        register_allocation[interval.name] = register_allocation[spill.name];
        spills.insert(spill.name);
        active.erase(it);
        active.insert(interval);
    }
    else {
        spills.insert(interval.name);
    }
}

int LinearScanner::generateNewStackLocation() {
    static int location = 0;
    return ++location;
}


void LinearScanner::visit(std::shared_ptr<Label> node) {}

void LinearScanner::visit(std::shared_ptr<Const> node) {}

void LinearScanner::visit(std::shared_ptr<Name> node) {}