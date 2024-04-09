#include <functional>
#include <unordered_set>
#include "cfgVisitor.hpp"

CFGVisitor::CFGVisitor(std::shared_ptr<Scope> s) : scope(s), isInsideIf(false), isInsideReturn(false), loopIndefinite(false) {
}

std::shared_ptr<_BasicBlock> CFGVisitor::newBlock() {
    auto newBlock = std::make_shared<_BasicBlock>();
    cfg.blocks.push_back(newBlock);
    return newBlock;
}

void CFGVisitor::createNewNode(const std::string& name) {
    if (!isInsideIf) {
        auto newNode = std::make_shared<_Node>(name);
        cfg.nodes.push_back(newNode);
        if (!isInsideReturn) cfg.addLink(currentNode, newNode);
        else isInsideReturn = false;
        currentNode = newNode;
    } else {
        isInsideIf = false;
        loopIndefinite = false;
        currentNode->name = name;
    }
}

void CFGVisitor::addStatement(std::shared_ptr<Statement> stmt) {
     if (!currentBlock) {
        currentBlock = std::make_shared<_BasicBlock>();
        cfg.blocks.push_back(currentBlock);
    }
    currentBlock->statements.push_back(stmt);
}

void CFGVisitor::visit(std::shared_ptr<BlockStatement> n) {
    if (isInsideReturn) exit(42);
    beginScope();
    Visitor::visit(n);
    endScope();
}

void CFGVisitor::visit(std::shared_ptr<Constructor> n) {
    cfg = ControlFlowGraph();
    currentBlock = cfg.start;
    currentNode = cfg.startNode;
    isInsideIf = false;
    isInsideReturn = false;
    loopIndefinite = false;
    beginScope();
    Visitor::visit(n);
    endScope();
    if (isInsideIf) {
        cfg.addLink(cfg.removeLastLink(), cfg.endNode);
        cfg.addLink(cfg.removeLastLink(), cfg.endNode);
        cfg.nodes.erase(std::remove(cfg.nodes.begin(), cfg.nodes.end(), currentNode));
        isInsideIf = false;
    }
    else if (currentMethodReturnType == DataType::VOID && !isInsideReturn) {
        cfg.addLink(currentNode, cfg.endNode);
    }
    cfg.nodes.push_back(cfg.endNode);
    cfg.mergeUnusedNodes();
    // cfg.Print();
    // cfg.PrintNodes();
    if(!cfg.checkReachability()) {
        std::cerr << "Error: checkReachability()" << std::endl;
        exit(42);
    }
    if (!cfg.checkDeadAssignments()) {
        std::cerr << "Warning: Dead assignment found" << std::endl;
        // exit(43);
    }
    cfgs.push_back(cfg);
}

void CFGVisitor::visit(std::shared_ptr<Method> n) {
    // TODO: FIX THIS, strong assumption！
    if ((!n->block || n->block->blockStatements->statements.empty()) && !n->isAbstract && !n->isStatic ) {
        if(n->type->type != DataType::VOID) {
            std::cerr << "Error: checkMissingReturn()" << std::endl;
            exit(42);
        }
        return;
    }
  
    if (!n->isAbstract) {
        cfg = ControlFlowGraph();
        currentBlock = cfg.start;
        currentNode = cfg.startNode;
        currentMethodReturnType = n->type->type;
        isInsideIf = false;
        isInsideReturn = false;
        loopIndefinite = false;
        beginScope();
        Visitor::visit(n);
        endScope();
        if (isInsideIf) {
            cfg.addLink(cfg.removeLastLink(), cfg.endNode);
            cfg.addLink(cfg.removeLastLink(), cfg.endNode);
            cfg.nodes.erase(std::remove(cfg.nodes.begin(), cfg.nodes.end(), currentNode));
            isInsideIf = false;
        }
        else if (currentMethodReturnType == DataType::VOID && !isInsideReturn) {
            cfg.addLink(currentNode, cfg.endNode);
        }
        cfg.nodes.push_back(cfg.endNode);
        cfg.mergeUnusedNodes();
        // cfg.Print();
        // cfg.PrintNodes();
        if(!cfg.checkReachability()) {
            std::cerr << "Error: checkReachability()" << std::endl;
            exit(42);
        }
        if(n->type->type != DataType::VOID && !cfg.checkMissingReturn() && !loopIndefinite) {
            std::cerr << "Error: checkMissingReturn()" << std::endl;
            exit(42);
        }
        if (!cfg.checkDeadAssignments()) {
            std::cerr << "Warning: Dead assignment found" << std::endl;
            // exit(43);
        }
        cfgs.push_back(cfg);
    }
}

void CFGVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    createNewNode("LocalVariableDeclarationStatement");
    const std::string key {n->id->name};
    stack_t.push(key);
    ltable.insert(key);
    uninitLocal.insert(key);
    if (n->exp) {
        Visitor::visit(n);
        uninitLocal.erase(key);
        currentNode->def = n->id->name;
        currentNode->init = true;
    }
    addStatement(n);
}

void CFGVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    createNewNode("SemicolonStatement");
    addStatement(n);
}

void CFGVisitor::visit(std::shared_ptr<IfStatement> n) {
    createNewNode("IfCondition");
    n->exp->accept(this);
    addStatement(n);
    auto ifNode = currentNode;

    auto thenBlock = newBlock();
    std::shared_ptr<_BasicBlock> elseBlock = nullptr;
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
    currentNode = ifNode;
    beginScope();
    n->statement1->accept(this);
    endScope();
    createNewNode("IfEnd");
    auto continueNode = currentNode;
    cfg.addEdge(currentBlock, continueBlock);    

    if (elseBlock) {
        currentNode = ifNode;
        currentBlock = elseBlock;
        beginScope();
        n->statement2->accept(this);
        endScope();
        cfg.addEdge(currentBlock, continueBlock);
        cfg.addLink(currentNode, continueNode);     
    } else {
        cfg.addLink(ifNode, continueNode);
    }
    currentBlock = continueBlock;
    currentNode = continueNode;
    isInsideIf = true;
}

void CFGVisitor::visit(std::shared_ptr<WhileStatement> n) {
    beginScope();
    auto conditionBlock = newBlock();
    cfg.addEdge(currentBlock, conditionBlock);

    currentBlock = conditionBlock;
    if (evaluateFalse(n->exp)) {
        std::cerr << "Error: While loop condition is a constant" << std::endl;
        exit(42);
    }
    if (evaluateTrue(n->exp)) {
        loopIndefinite = true;
    }
    createNewNode("WhileCondition");
    auto whileNode = currentNode;
    n->exp->accept(this);
    if (n->statement) {
        auto bodyBlock = newBlock();
        cfg.addEdge(conditionBlock, bodyBlock);
        currentBlock = bodyBlock;
        n->statement->accept(this);
        if (isInsideIf) {
            isInsideIf = false;
            cfg.addLink(cfg.removeLastLink(), whileNode);
            cfg.addLink(cfg.removeLastLink(), whileNode);
            cfg.nodes.erase(std::remove(cfg.nodes.begin(), cfg.nodes.end(), currentNode));
        } else {
            cfg.addLink(currentNode, whileNode);
        }
        cfg.addEdge(currentBlock, conditionBlock);
    }

    auto afterLoopBlock = newBlock();
    cfg.addEdge(conditionBlock, afterLoopBlock);

    currentBlock = afterLoopBlock;
    currentNode = whileNode;
    endScope();
}

void CFGVisitor::visit(std::shared_ptr<ForStatement> n) {
    beginScope();
    auto initBlock = newBlock();
    cfg.addEdge(currentBlock, initBlock);
    currentBlock = initBlock;
    if (n->stmt1) {
        n->stmt1->accept(this);
    }

    auto conditionBlock = newBlock();
    cfg.addEdge(currentBlock, conditionBlock);
    currentBlock = conditionBlock;

    createNewNode("ForCondition");
    auto forNode = currentNode;
    if (n->exp) {
        n->exp->accept(this);
    }
    if (evaluateFalse(n->exp)) {
        std::cerr << "Error: For loop condition is a constant" << std::endl;
        exit(42);
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
    createNewNode("ForUpdate");
    if (n->expStmt2) {
        n->expStmt2->accept(this);
    }
    cfg.addEdge(currentBlock, conditionBlock);
    cfg.addLink(currentNode, forNode);

    auto afterLoopBlock = newBlock();
    cfg.addEdge(conditionBlock, afterLoopBlock);
    currentBlock = afterLoopBlock;
    currentNode = forNode;
    endScope();
}

void CFGVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    createNewNode("ReturnStatement");
    Visitor::visit(n);
    addStatement(n);
    if (loopIndefinite) {
        std::cerr << "Error: Indefinite loop detected" << std::endl;
        exit(42);
    }
    cfg.addLink(currentNode, cfg.endNode);
    isInsideReturn = true;
}

void CFGVisitor::visit(std::shared_ptr<ExpressionStatement> n) {
    createNewNode("ExpressionStatement");
    Visitor::visit(n);
    addStatement(n);
}

void CFGVisitor::visit(std::shared_ptr<FormalParameter> n) {
    const std::string key {n->variableName->name};
    stack_t.push(key);
    ltable.insert(key);
}

void CFGVisitor::beginScope() {
    stack_t.push("#");
}

void CFGVisitor::endScope() {
    while (stack_t.top() != "#") {
        std::string& var{stack_t.top()};
        if (uninitLocal.contains(var)) {
            std::cerr << "Error: local variable " << var << " not initialized" << std::endl;
            exit(42);
        }
        ltable.erase(var);
        stack_t.pop();
    }
    stack_t.pop();
}

 void CFGVisitor::visit(std::shared_ptr<IdentifierExp> n) {
    std::string& var{n->id->name};
    if (ltable.contains(var)) {
        currentNode->use.insert(var);
    } 
    auto it = std::find(var.begin(), var.end(), '.');
    if (it != var.end()) {
        std::string temp{var.begin(), it};
        if (ltable.contains(temp)) {
            currentNode->use.insert(temp);
        }
    }
    if (uninitLocal.contains(n->id->name)) {
        std::cerr << "Error: use of uninitialized local variable " << n->id->name << std::endl;
        exit(42);
    }
 }

void CFGVisitor::visit(std::shared_ptr<Assignment> n) {
    if (auto ie = std::dynamic_pointer_cast<IdentifierExp>(n->left)) {
        if (ltable.contains(ie->id->name)) {
            currentNode->def = ie->id->name;
        }
        if (uninitLocal.contains(ie->id->name)) {
            uninitLocal.erase(ie->id->name);
        }
        n->right->accept(this);
    } else {
        Visitor::visit(n);
    }
}

void CFGVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    n->exp1->accept(this);
}

bool CFGVisitor::evaluateFalse(std::shared_ptr<AstNode> n) {
    if (auto bexp = std::dynamic_pointer_cast<BoolLiteralExp>(n)) {
        return bexp->value == false;
    }
    if (auto orexp = std::dynamic_pointer_cast<ConditionalOrExp>(n)) {
        if (evaluateFalse(orexp->exp1) && evaluateFalse(orexp->exp2)) return true;
    }
    if (auto andexp = std::dynamic_pointer_cast<ConditionalAndExp>(n)) {
        if (evaluateFalse(andexp->exp1) || evaluateFalse(andexp->exp2)) return true;
    }
    if (auto equalexp = std::dynamic_pointer_cast<EqualExp>(n)) {
        if (std::dynamic_pointer_cast<IntegerLiteralExp>(equalexp->exp1)) return true;
    }
    return false;
}

bool CFGVisitor::evaluateTrue(std::shared_ptr<AstNode> n) {
    if (auto bexp = std::dynamic_pointer_cast<BoolLiteralExp>(n)) {
        return bexp->value == true;
    }
    if (auto orexp = std::dynamic_pointer_cast<ConditionalOrExp>(n)) {
        if (evaluateTrue(orexp->exp1) || evaluateTrue(orexp->exp2)) return true;
    }
    if (auto andexp = std::dynamic_pointer_cast<ConditionalAndExp>(n)) {
        if (evaluateTrue(andexp->exp1) && evaluateTrue(andexp->exp2)) return true;
    }
    return false;
}
