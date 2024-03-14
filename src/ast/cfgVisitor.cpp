#include <functional>
#include <unordered_set>
#include "cfgVisitor.hpp"

CFGVisitor::CFGVisitor() {
}

std::shared_ptr<BasicBlock> CFGVisitor::newBlock() {
    auto newBlock = std::make_shared<BasicBlock>();
    cfg.blocks.push_back(newBlock);
    return newBlock;
}

void CFGVisitor::addStatement(std::shared_ptr<Statement> stmt) {
     if (!currentBlock) {
        currentBlock = std::make_shared<BasicBlock>();
        cfg.blocks.push_back(currentBlock);
    }
    currentBlock->statements.push_back(stmt);
}

void CFGVisitor::visit(std::shared_ptr<BlockStatement> n) {
    Visitor::visit(n);
}

void CFGVisitor::visit(std::shared_ptr<Constructor> n) {
    Visitor::visit(n);
}

void CFGVisitor::visit(std::shared_ptr<Method> n) {
    if (!n->block) return;
    if (!n->isAbstract) {
        cfg = ControlFlowGraph();
        currentBlock = cfg.start;
        currentMethodReturnType = n->type->type;
        Visitor::visit(n);
        cfg.Print();
        if(!cfg.checkReachability()) {
            std::cerr << "Error: checkReachability()" << std::endl;
            exit(42);
        }
        if(n->type->type != DataType::VOID && !cfg.checkMissingReturn()) {
            std::cerr << "Error: checkMissingReturn()" << std::endl;
            exit(42);
        }
    }
}

void CFGVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    addStatement(n);
}

void CFGVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    addStatement(n);
}

void CFGVisitor::visit(std::shared_ptr<IfStatement> n) {
    addStatement(n);
    auto thenBlock = newBlock();
    std::shared_ptr<BasicBlock> elseBlock = nullptr;
    auto continueBlock = newBlock();
    cfg.addEdge(currentBlock, thenBlock);

    if (n->statement2) {
        elseBlock = newBlock();
        cfg.addEdge(currentBlock, elseBlock);
    }
    else {
        cfg.addEdge(currentBlock, continueBlock);
    }
    
    currentBlock = thenBlock;
    n->statement1->accept(this);
    cfg.addEdge(currentBlock, continueBlock);    

    if (elseBlock) {
        currentBlock = elseBlock;
        n->statement2->accept(this);
        cfg.addEdge(currentBlock, continueBlock);       
    }
    currentBlock = continueBlock;
}

void CFGVisitor::visit(std::shared_ptr<WhileStatement> n) {
    auto conditionBlock = newBlock();
    cfg.addEdge(currentBlock, conditionBlock);

    currentBlock = conditionBlock;
    if (auto f = std::dynamic_pointer_cast<BoolLiteralExp>(n->exp)) {
        if (!f->value) {
            std::cerr << "Error: While loop condition is always false" << std::endl;
            exit(42);
        }
    }
    n->exp->accept(this);    
    auto bodyBlock = newBlock();
    cfg.addEdge(conditionBlock, bodyBlock);

    currentBlock = bodyBlock;
    n->statement->accept(this);
    cfg.addEdge(currentBlock, conditionBlock);

    auto afterLoopBlock = newBlock();
    cfg.addEdge(conditionBlock, afterLoopBlock);

    currentBlock = afterLoopBlock;
}

void CFGVisitor::visit(std::shared_ptr<ForStatement> n) {
    auto initBlock = newBlock();
    cfg.addEdge(currentBlock, initBlock);
    currentBlock = initBlock;
    if (n->stmt1) {
        n->stmt1->accept(this);
    }

    auto conditionBlock = newBlock();
    cfg.addEdge(currentBlock, conditionBlock);
    currentBlock = conditionBlock;
    if (n->exp) {
        n->exp->accept(this);
    }

    auto bodyBlock = newBlock();
    cfg.addEdge(conditionBlock, bodyBlock);


    auto updateBlock = newBlock();
    currentBlock = bodyBlock;

    if(n->stmt2) {
        n->stmt2->accept(this);
    }

    cfg.addEdge(currentBlock, updateBlock);

    currentBlock = updateBlock;
    if (n->expStmt2) {
        n->expStmt2->accept(this);
    }
    cfg.addEdge(currentBlock, conditionBlock);

    auto afterLoopBlock = newBlock();
    cfg.addEdge(conditionBlock, afterLoopBlock);
    currentBlock = afterLoopBlock;
}


void CFGVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    addStatement(n);
}

void CFGVisitor::visit(std::shared_ptr<ExpressionStatement> n) {
    addStatement(n);
}

