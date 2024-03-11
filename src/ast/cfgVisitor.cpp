#include "cfgVisitor.hpp"

CFGVisitor::CFGVisitor() {
}

void CFGVisitor::visit(std::shared_ptr<Constructor> n) {
    ControlFlowGraph cfg;
    block = cfg.start;
    from = std::vector<std::shared_ptr<BasicBlock>>{block};
    Visitor::visit(n);
    cfgs.push_back(cfg);
}

void CFGVisitor::visit(std::shared_ptr<Method> n) {
    ControlFlowGraph cfg;
    block = cfg.start;
    from = std::vector<std::shared_ptr<BasicBlock>>{block};
    Visitor::visit(n);
    cfgs.push_back(cfg);
}

void CFGVisitor::visit(std::shared_ptr<BlockStatement> n) {
    std::shared_ptr<BasicBlock> bb = std::make_shared<BasicBlock>();
    cfg.addEdge(from, bb);
    block = bb;
    from = std::vector<std::shared_ptr<BasicBlock>>{bb};
    n->blockStatements->accept(this);
    from = std::vector<std::shared_ptr<BasicBlock>>{bb};
    // build a new basic block after block stmt
    block = std::make_shared<BasicBlock>();
}

void CFGVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::visit(std::shared_ptr<IfStatement> n) {
    std::vector<std::shared_ptr<BasicBlock>> temp = std::move(from);
    std::shared_ptr<BasicBlock> ib = std::make_shared<BasicBlock>();
    cfg.addEdge(temp, ib);
    block = ib;
    from = std::vector<std::shared_ptr<BasicBlock>>{ib};
    n->statement1->accept(this);
    // need to reassign from to {b1} because the then statement may change from value(due to block stmt)
    from = std::vector<std::shared_ptr<BasicBlock>>{ib};
    if (n->statement2) {
        std::shared_ptr<BasicBlock> eb = std::make_shared<BasicBlock>();
        cfg.addEdge(temp, eb);
        block = eb;
        from = std::vector<std::shared_ptr<BasicBlock>>{eb};
        n->statement2->accept(this);
        from = std::vector<std::shared_ptr<BasicBlock>>{ib, eb};
    }
    // build a new basic block after if stmt
    block = std::make_shared<BasicBlock>();
}

void CFGVisitor::visit(std::shared_ptr<WhileStatement> n) {
    std::shared_ptr<BasicBlock> wb = std::make_shared<BasicBlock>();
    wb->statements.push_back(n);
    cfg.addEdge(from, wb);
    block = wb;
    from = std::vector<std::shared_ptr<BasicBlock>>{wb};
    n->statement->accept(this);
    from = std::vector<std::shared_ptr<BasicBlock>>{wb};
    cfg.addEdge(from, wb);
    // build a new basic block after while stmt
    block = std::make_shared<BasicBlock>();
}

void CFGVisitor::visit(std::shared_ptr<ForStatement> n) {
    std::shared_ptr<BasicBlock> fb = std::make_shared<BasicBlock>();
    fb->statements.push_back(n);
    cfg.addEdge(from, fb);
    block = fb;
    from = std::vector<std::shared_ptr<BasicBlock>>{fb};
    n->stmt2->accept(this);
    from = std::vector<std::shared_ptr<BasicBlock>>{fb};
    cfg.addEdge(from, fb);
    // build a new basic block after for stmt
    block = std::make_shared<BasicBlock>();
}

void CFGVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::visit(std::shared_ptr<ExpressionStatement> n) {
    block->statements.push_back(n);
}
