#include <functional>
#include <unordered_set>
#include "cfgVisitor.hpp"

CFGVisitor::CFGVisitor() {
}

void CFGVisitor::visit(std::shared_ptr<Constructor> n) {
    cfg = ControlFlowGraph();
    block = cfg.start;
    from = std::vector<std::shared_ptr<BasicBlock>>{block};
    Visitor::visit(n);
    cfgs.push_back(cfg);
}

void CFGVisitor::visit(std::shared_ptr<Method> n) {
    if (!n->isAbstract) {
        cfg = ControlFlowGraph();
        block = cfg.start;
        from = std::vector<std::shared_ptr<BasicBlock>>{block};
        Visitor::visit(n);
        cfgs.push_back(cfg);
    }
}

void CFGVisitor::visit(std::shared_ptr<BlockStatement> n) {
    createBasicBlock();
    std::shared_ptr<BasicBlock> bb = block;
    n->blockStatements->accept(this);
    from = std::vector<std::shared_ptr<BasicBlock>>{bb};
    // build a new basic block after block stmt
    createBasicBlock();
}

void CFGVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::visit(std::shared_ptr<IfStatement> n) {
    std::vector<std::shared_ptr<BasicBlock>> temp = from;
    createBasicBlock();
    std::shared_ptr<BasicBlock> ib = block;
    n->statement1->accept(this);
    if (n->statement2) {
        from = temp;
        createBasicBlock();
        std::shared_ptr<BasicBlock> eb = block;
        n->statement2->accept(this);
        from = std::vector<std::shared_ptr<BasicBlock>>{ib, eb};
    } else {
        from = std::vector<std::shared_ptr<BasicBlock>>{ib};
    }
    // build a new basic block after if stmt
    createBasicBlock();
}

void CFGVisitor::visit(std::shared_ptr<WhileStatement> n) {
    createBasicBlock();
    std::shared_ptr<BasicBlock> bb = block;
    bb->statements.push_back(n);
    n->statement->accept(this);
    from = std::vector<std::shared_ptr<BasicBlock>>{bb};
    // add edge to itself
    cfg.addEdge(from, bb);
    // build a new basic block after while stmt
    createBasicBlock();
}

void CFGVisitor::visit(std::shared_ptr<ForStatement> n) {
    createBasicBlock();
    std::shared_ptr<BasicBlock> bb = block;
    bb->statements.push_back(n);
    n->stmt2->accept(this);
    from = std::vector<std::shared_ptr<BasicBlock>>{bb};
    cfg.addEdge(from, bb);
    createBasicBlock();
}

void CFGVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::visit(std::shared_ptr<ExpressionStatement> n) {
    block->statements.push_back(n);
}

void CFGVisitor::createBasicBlock() {
    block = std::make_shared<BasicBlock>();
    cfg.addEdge(from, block);
    cfg.blocks.push_back(block);
    from = std::vector<std::shared_ptr<BasicBlock>>{block};
}

void CFGVisitor::printCFG(const ControlFlowGraph& cfg) {
    std::function<void(std::shared_ptr<BasicBlock>, int)> visitBlock;

    visitBlock = [&](std::shared_ptr<BasicBlock> block, int depth) {
        if (!block) return;

        // Indentation for readability based on depth
        std::string indent(depth * 2, ' ');
        
        std::cout << indent << "BasicBlock: " << block.get() << "\n";

        // Iterate over statements in the block
        for (const auto& stmt : block->statements) {
            // Assuming toString() is a method that returns a string representation of the statement
            std::cout << indent << "  Stmt: " << stmt->getStmtName() << "\n";
        }

        // Print successors
        if (!block->succ.empty()) {
            std::cout << indent << "  Successors: ";
            for (const auto& succ : block->succ) {
                std::cout << succ.get() << " ";
            }
            std::cout << "\n";
        }

        // Recursively visit successors with increased depth for indentation
        for (const auto& succ : block->succ) {
            if (succ.get() != block.get())
                visitBlock(succ, depth + 1);
        }
    };

    visitBlock(cfg.start, 0); // Start with depth 0
}
