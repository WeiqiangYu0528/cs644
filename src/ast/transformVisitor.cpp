#include "transformVisitor.hpp"
#include "unordered_map"

TransformVisitor::TransformVisitor(std::shared_ptr<NodeFactory> nf) : nodeFactory(nf), labelCounter(0) {
}

std::shared_ptr<CompUnit> TransformVisitor::visit(std::shared_ptr<ClassDecl> n) {
    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> methods;
    for (std::shared_ptr<Method> method : n->declarations[1]) {
        methods[method->methodName->name] = visit(method);
    }
    return nodeFactory->IRCompUnit(n->id->name, methods);
}

std::shared_ptr<CompUnit> TransformVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> methods;
    for (auto method : n->methods) {
        methods[method->methodName->name] = visit(method);
    }
    return nodeFactory->IRCompUnit(n->id->name, methods);
}

std::shared_ptr<FuncDecl> TransformVisitor::visit(std::shared_ptr<Method> n) {
    return nodeFactory->IRFuncDecl(n->methodName->name, n->formalParameters.size(), visit(n->block));   
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<PlusExp> n) {
    return nodeFactory->IRBinOp(OpType::ADD, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<MinusExp> n) {
    return nodeFactory->IRBinOp(OpType::SUB, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<TimesExp> n) {
    return nodeFactory->IRBinOp(OpType::MUL, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<DivideExp> n) {
    return nodeFactory->IRBinOp(OpType::DIV, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<ModuloExp> n) {
    return nodeFactory->IRBinOp(OpType::MOD, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<LessExp> n) {
    return nodeFactory->IRBinOp(OpType::LT, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<GreaterExp> n) {
    return nodeFactory->IRBinOp(OpType::GT, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<LessEqualExp> n) {
    return nodeFactory->IRBinOp(OpType::LEQ, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<GreaterEqualExp> n) {
    return nodeFactory->IRBinOp(OpType::GEQ, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<EqualExp> n) {
    return nodeFactory->IRBinOp(OpType::EQ, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<NotEqualExp> n) {
    return nodeFactory->IRBinOp(OpType::NEQ, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    return nodeFactory->IRBinOp(OpType::AND, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<BinOp> TransformVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    return nodeFactory->IRBinOp(OpType::OR, visit(n->exp1), visit(n->exp2));
}

std::shared_ptr<Move> TransformVisitor::visit(std::shared_ptr<Assignment> n) {
    return nodeFactory->IRMove(visit(n->left), visit(n->right));
}

std::shared_ptr<Const> TransformVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) {
    return nodeFactory->IRConst(n->value);
}

std::shared_ptr<Const> TransformVisitor::visit(std::shared_ptr<BoolLiteralExp> n) {
    return nodeFactory->IRConst(n->value);
}

std::shared_ptr<Const> TransformVisitor::visit(std::shared_ptr<CharLiteralExp> n) {
    return nodeFactory->IRConst(n->value);
}

std::shared_ptr<Seq> TransformVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    return nodeFactory->IRSeq(std::vector<std::shared_ptr<Stmt>>{});
}

std::shared_ptr<Seq> TransformVisitor::visit(std::shared_ptr<IfStatement> n) {
    std::vector<std::shared_ptr<Stmt>> stmts;
    std::shared_ptr<Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    stmt.push_back(nodeFactory->IRCJump(visit(n->exp), trueLabel->getName(), falseLabel->getName()));
    stmt.push_back(trueLabel);
    stmt.push_back(visit(n->statement1));
    stmt.push_back(falseLabel);
    if (n->statement2) stmt.push_back(visit(n->statement2));
    return nodeFactory->IRSeq(stmts);
}

std::shared_ptr<Seq> TransformVisitor::visit(std::shared_ptr<WhileStatement> n) {
    std::vector<std::shared_ptr<Stmt>> stmts;
    std::shared_ptr<Label> whileLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    stmts.push_back(whileLabel);
    stmts.push_back(nodeFactory->IRCJump(visit(n->exp), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    stmts.push_back(visit(n->statement));
    stmts.push_back(nodeFactory->IRJump(nodeFactory->IRName(whileLabel->getName())));
    stmts.push_back(falseLabel);
    return nodeFactory->IRSeq(stmts);
}

std::shared_ptr<Seq> TransformVisitor::visit(std::shared_ptr<ForStatement> n) {
    std::vector<std::shared_ptr<Stmt>> stmts;
    std::shared_ptr<Label> forLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    if (n->stmt1) stmts.push_back(visit(n->stmt1));
    stmts.push_back(forLabel);
    stmts.push_back(nodeFactory->IRCJump(visit(n->exp), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    stmts.push_back(visit(n->stmt2));
    stmts.push_back(visit(n->expStmt2));
    stmts.push_back(nodeFactory->IRJump(nodeFactory->IRName(forLabel->getName())));
    stmts.push_back(falseLabel);
    return nodeFactory->IRSeq(stmts);
}

std::shared_ptr<Return> TransformVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    return nodeFactory->IRReturn(visit(n->exp));
}

std::shared_ptr<Node> TransformVisitor::visit(std::shared_ptr<ExpressionStatement> n) {
    return visit(n->exp);
}

std::shared_ptr<Seq> TransformVisitor::visit(std::shared_ptr<BlockStatement> n) {
    return visit(n->blockStatements);
}

std::shared_ptr<Seq> TransformVisitor::visit(std::shared_ptr<BlockStatements> n) {
    std::vector<std::shared_ptr<Stmt>> stmts;
    for (auto stmt : n->statements) {
        stmts.push_back(visit(stmt));
    }
    return nodeFactory->IRSeq(stmts);
}

std::shared_ptr<Call> TransformVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::vector<std::shared_ptr<Expr>> args;
    for (auto arg : n->arguments) {
        args.push_back(visit(arg));
    }
    return nodeFactory->IRCall(nodeFactory->IRName(n->methodName->id->name), args);
}

std::shared_ptr<Expr> TransformVisitor::visit(std::shared_ptr<ParExp> n) {
    return visit(n->exp);
}

std::shared_ptr<ESeq> TransformVisitor::visit(std::shared_ptr<ArrayAccessExp> n) {
    std::vector<std::shared_ptr<Stmt>> stmts;
    std::shared_ptr<Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<Label> inboundLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::string arr = std::dynamic_pointer_cast<IdentifierExp>(n->array)->id->name;
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(arr), nodeFactory->IRName(arr)));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(OpType::EQ, nodeFactory->IRMem(nodeFactory->IRTemp(arr)), nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
    stmts.push_back(errLabel);
    stmts.push_back(nodeFactory->IRCall());
    stmts.push_back(nonNullLabel);
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp("ti"), visit(n->index)));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(OpType::LT, nodeFactory->IRTemp("ti"), nodeFactory->IRMem(nodeFactory->Na)), inboundLabel->getName(), errLabel->getName()));
    stmts.push_back(inboundLabel);
    std::shared_ptr<BinOp>
    std::shared_ptr<Mem> mem = nodeFactory->IRMem();


    return ESeq* NodeFactory_c::IRESeq(Stmt* stmt, Expr* expr);
}

std::shared_ptr<CompUnit> TransformVisitor::visit(std::shared_ptr<Program> n) {
    return visit(n->classOrInterfaceDecl);
}

std::shared_ptr<Move> TransformVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    auto temp = nodeFactory->IRTemp(n->id->name);
    std::shared_ptr<Expr> exp = n->exp ? visit(n->exp) : nullptr;
    return nodeFactory->IRMove(temp, exp);
}
