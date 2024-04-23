#include "IRCfgVisitor.hpp"

using namespace TIR;

CfgVisitor::CfgVisitor() {
}


void CfgVisitor::visit(std::shared_ptr<CompUnit> cu) {
    for (auto pair : cu->getFunctions()) {
        visit(pair.second);
    }
}

void CfgVisitor::visit(std::shared_ptr<FuncDecl> fd, bool optim) {
    std::shared_ptr<Seq> seq = std::dynamic_pointer_cast<Seq>(fd->body);
    // std::cout << fd->getLabel() << std::endl;
    auto stmts = visit(seq, optim);
    // std::cout << "====================================================" << std::endl;
    if(optim)
        fd->body = std::make_shared<Seq>(stmts);
    
    // seq = std::dynamic_pointer_cast<Seq>(fd->body);
    // visit(seq);
}

std::vector<std::shared_ptr<Stmt>> CfgVisitor::visit(std::shared_ptr<Seq>& seq, bool optim=true) {
    currentBlock = nullptr;
    lastBlock = nullptr;
    cfg = CFG();
    startNewBlock();
    
    // for (auto& stmt : seq->getStmts()) {
    //     std::cout << stmt->getLabel();
    //     if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
    //         std::cout << " " << cjump->getTrueLabel();
    //         if(cjump->hasFalseLabel()) 
    //             std::cout << " " << cjump->getFalseLabel();
    //     } 
    //     if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
    //         std::cout << " " << jump->getTarget()->getLabel();
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    for (auto& stmt : seq->getStmts()) {
        visit(stmt);
    }
    endCurrentBlock();
    connectBlocks();
    // cfg.Print();
    if (optim) {
        auto stmts = cfg.collectTraces();
        return stmts;
    }
    else
        return std::vector<std::shared_ptr<Stmt>>();
        
    // for (auto& stmt : stmts) {
    //     std::cout << stmt->getLabel();
    //     if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
    //         std::cout << " " << cjump->getTrueLabel();
    //         if(cjump->hasFalseLabel()) 
    //             std::cout << " " << cjump->getFalseLabel();
    //     } 
    //     if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
    //         std::cout << " " << jump->getTarget()->getLabel();
    //     }
    //     std::cout << std::endl;
    // }

    // seq = std::make_shared<TIR::Seq>(stmts);
    // std::shared_ptr<CheckCanonicalIRVisitor> ccv = std::make_shared<CheckCanonicalIRVisitor>();
    // std::cout << "Canonical? " << (ccv->visit(seq) ? "Yes" : "No") << std::endl;

}


void CfgVisitor::visit(std::shared_ptr<Stmt> stmt) {
    if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
        visit(cjump);
    } else if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
        visit(jump);
    } else if (auto label = std::dynamic_pointer_cast<Label>(stmt)) {
        visit(label);
    } else if (auto move = std::dynamic_pointer_cast<Move>(stmt)) {
        visit(move);
    } else if (auto ret = std::dynamic_pointer_cast<Return>(stmt)) {
        visit(ret);
    } else if (auto call = std::dynamic_pointer_cast<Call_s>(stmt)) {
        visit(call);
    } else {
        throw std::runtime_error("unknown stmt");
    }
}

void CfgVisitor::visit(std::shared_ptr<Label> label) {
   if (!currentBlock || !currentBlock->statements.empty()) {
        endCurrentBlock();
        startNewBlock();
    }
    addStatment(label);
    cfg.labelToBlock[label->getName()] = currentBlock;
}

void CfgVisitor::visit(std::shared_ptr<Jump> jump) {
    addStatment(jump);
    auto name = std::dynamic_pointer_cast<Name>(jump->getTarget());
    cfg.pendingJumps.emplace_back(currentBlock, name->getName());
    endCurrentBlock();
}

void CfgVisitor::visit(std::shared_ptr<CJump> cjump) {
    addStatment(cjump);
    cfg.pendingCJumps.emplace_back(currentBlock, make_pair(cjump->getTrueLabel(), cjump->getFalseLabel()));
    endCurrentBlock();
}

void CfgVisitor::visit(std::shared_ptr<Move> move) {
    addStatment(move);
}

void CfgVisitor::visit(std::shared_ptr<Return> ret) {
    addStatment(ret);
    endCurrentBlock();
}

void CfgVisitor::visit(std::shared_ptr<Call_s> call) {
    currentBlock->statements.push_back(call);
}

void CfgVisitor::startNewBlock() {
    currentBlock = cfg.newBlock();
}

void CfgVisitor::endCurrentBlock() {
    currentBlock = nullptr;
}

void CfgVisitor::addStatment(std::shared_ptr<Stmt> stmt) {
    if(!currentBlock)
        startNewBlock();
    currentBlock->statements.push_back(stmt);
}


void CfgVisitor::connectBlocks() {
    for (const auto& jump : cfg.pendingJumps) {
        auto from = jump.first;
        auto to = cfg.labelToBlock[jump.second];
        cfg.addEdge(from, to);
    }

    for (const auto& cjump : cfg.pendingCJumps) {
        auto from = cjump.first;
        auto trueTo = cfg.labelToBlock[cjump.second.first];
        auto falseTo = cfg.labelToBlock[cjump.second.second];
        cfg.addEdge(from, trueTo);
        if(!cjump.second.second.empty())
            cfg.addEdge(from, falseTo);
    }

    for (size_t i = 0; i < cfg.blocks.size() - 1; ++i) {
        auto& block = cfg.blocks[i];
        if (std::dynamic_pointer_cast<Jump>(block->statements.back())) continue;
        else if (auto cjump = std::dynamic_pointer_cast<CJump>(block->statements.back())) {
            if(cjump->hasFalseLabel()) continue;
        }
        else if (std::dynamic_pointer_cast<Return>(block->statements.back())) continue;
        cfg.addEdge(block, cfg.blocks[i + 1]);
    }

}