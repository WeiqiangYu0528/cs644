#include <algorithm>
#include "transformVisitor.hpp"
#include "Configuration.hpp"

TransformVisitor::TransformVisitor(std::shared_ptr<TIR::NodeFactory> nf) : nodeFactory(nf), node(nullptr), labelCounter(0) {
}

void TransformVisitor::visit(std::shared_ptr<ClassDecl> n) {
    std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> methods;
    for (auto methodDecl: n->declarations[1]) {
        auto method = std::dynamic_pointer_cast<Method>(methodDecl);
        method->accept(this);
        methods[method->methodName->name] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
    }
    node = nodeFactory->IRCompUnit(n->id->name, methods);
}

void TransformVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> methods;
    for (auto method : n->methods) {
        method->accept(this);
        methods[method->methodName->name] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
    }
    node = nodeFactory->IRCompUnit(n->id->name, methods);
}

void TransformVisitor::visit(std::shared_ptr<Method> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    for (size_t i = 0; i < n->formalParameters.size(); ++i) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(n->formalParameters[i]->variableName->name), nodeFactory->IRTemp(std::string(TIR::Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(i))));
    }
    n->block->accept(this);
    auto body = std::dynamic_pointer_cast<TIR::Seq>(node)->getStmts();
    stmts.insert(stmts.end(), body.begin(), body.end());
    node = nodeFactory->IRFuncDecl(n->methodName->name, n->formalParameters.size(), nodeFactory->IRSeq(stmts)); 
}

void TransformVisitor::visit(std::shared_ptr<PlusExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<MinusExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, exp1, exp2);
}
              
void TransformVisitor::visit(std::shared_ptr<TimesExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<DivideExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::DIV, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<ModuloExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::MOD, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<LessExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::LT, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<GreaterExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::GT, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<LessEqualExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::LEQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<GreaterEqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::GEQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<EqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<NotEqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::NEQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::AND, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    n->exp1->accept(this);
    auto exp1 = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->exp2->accept(this);
    auto exp2 = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::OR, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<Assignment> n) {
    n->left->accept(this);
    auto left = std::dynamic_pointer_cast<TIR::Expr>(node);
    n->right->accept(this);
    auto right = std::dynamic_pointer_cast<TIR::Expr>(node);
    node = nodeFactory->IRMove(left, right);
}

void TransformVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) {
    node = nodeFactory->IRConst(static_cast<int>(n->value));
}

void TransformVisitor::visit(std::shared_ptr<BoolLiteralExp> n) {
    node = nodeFactory->IRConst(static_cast<int>(n->value));
}

void TransformVisitor::visit(std::shared_ptr<CharLiteralExp> n) {
    node = nodeFactory->IRConst(static_cast<int>(n->value));
}

void TransformVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    // node = nodeFactory->IRExp(nodeFactory->IRConst(0));
}

void TransformVisitor::visit(std::shared_ptr<IfStatement> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    n->exp->accept(this);
    stmts.push_back(nodeFactory->IRCJump(std::dynamic_pointer_cast<TIR::Expr>(node), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    n->statement1->accept(this);
    stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    stmts.push_back(falseLabel);
    if (n->statement2) {
        n->statement2->accept(this);
        stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    }
    node = nodeFactory->IRSeq(stmts);
}

void TransformVisitor::visit(std::shared_ptr<WhileStatement> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> whileLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    stmts.push_back(whileLabel);
    n->exp->accept(this);
    stmts.push_back(nodeFactory->IRCJump(std::dynamic_pointer_cast<TIR::Expr>(node), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    n->statement->accept(this);
    stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    stmts.push_back(nodeFactory->IRJump(nodeFactory->IRName(whileLabel->getName())));
    stmts.push_back(falseLabel);
    node = nodeFactory->IRSeq(stmts);
}

void TransformVisitor::visit(std::shared_ptr<ForStatement> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> forLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    if (n->stmt1) {
        n->stmt1->accept(this);
        stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    }
    stmts.push_back(forLabel);
    n->exp->accept(this);
    stmts.push_back(nodeFactory->IRCJump(std::dynamic_pointer_cast<TIR::Expr>(node), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    if (n->stmt2) {
        n->stmt2->accept(this);
        stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    }
    if (n->expStmt2) {
        n->expStmt2->accept(this);
        stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    
    }
    stmts.push_back(nodeFactory->IRJump(nodeFactory->IRName(forLabel->getName())));
    stmts.push_back(falseLabel);
    node = nodeFactory->IRSeq(stmts);
}

void TransformVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    n->exp->accept(this);
    node = nodeFactory->IRReturn(std::dynamic_pointer_cast<TIR::Expr>(node));
}

void TransformVisitor::visit(std::shared_ptr<ExpressionStatement> n) {
    n->exp->accept(this);
}

void TransformVisitor::visit(std::shared_ptr<BlockStatement> n) {
    n->blockStatements->accept(this);
}

void TransformVisitor::visit(std::shared_ptr<BlockStatements> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    for (auto stmt : n->statements) {
        stmt->accept(this);
        stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    }
    node = nodeFactory->IRSeq(stmts);
}

void TransformVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::vector<std::shared_ptr<TIR::Expr>> args;
    for (auto arg : n->arguments) {
        arg->accept(this);
        args.push_back(std::dynamic_pointer_cast<TIR::Expr>(node));
    }
    node = nodeFactory->IRCall(nodeFactory->IRName(n->methodName->id->name), args);
}

void TransformVisitor::visit(std::shared_ptr<ParExp> n) {
    n->exp->accept(this);
}

void TransformVisitor::visit(std::shared_ptr<NewArrayExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> nonNegLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> inboundLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Temp> tn = nodeFactory->IRTemp("tn");
    std::shared_ptr<TIR::Temp> tm = nodeFactory->IRTemp("tm");
    n->exp->accept(this);
    auto len = std::dynamic_pointer_cast<TIR::Const>(node);
    stmts.push_back(nodeFactory->IRMove(tn, len));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LT, tn, nodeFactory->IRConst(0)), errLabel->getName(), nonNegLabel->getName()));
    stmts.push_back(errLabel);
    stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
    stmts.push_back(nonNegLabel);
    stmts.push_back(nodeFactory->IRMove(tm, nodeFactory->IRCall(nodeFactory->IRName("__malloc"), nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, nodeFactory->IRConst(2 * TIR::Configuration::WORD_SIZE), nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, tn, nodeFactory->IRConst(TIR::Configuration::WORD_SIZE))))));
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(tm), tn));
    size_t arrSize = len->getValue() + 1;
    for (size_t i = 1; i <= arrSize; ++i) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, nodeFactory->IRConst(i), nodeFactory->IRConst(TIR::Configuration::WORD_SIZE)))), nodeFactory->IRConst(0)));
    }
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(TIR::Configuration::WORD_SIZE));
    node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), offset);
}

void TransformVisitor::visit(std::shared_ptr<ArrayAccessExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> inboundLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::string arr = std::dynamic_pointer_cast<IdentifierExp>(n->array)->id->name;
    std::shared_ptr<TIR::Temp> ta = nodeFactory->IRTemp("ta");
    std::shared_ptr<TIR::Temp> ti = nodeFactory->IRTemp("ti");
    stmts.push_back(nodeFactory->IRMove(ta, nodeFactory->IRName(arr)));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, ta, nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
    stmts.push_back(errLabel);
    stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
    stmts.push_back(nonNullLabel);
    n->index->accept(this);
    stmts.push_back(nodeFactory->IRMove(ti, std::dynamic_pointer_cast<TIR::Expr>(node)));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LTU, ti, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, ta, nodeFactory->IRConst(TIR::Configuration::WORD_SIZE)))), inboundLabel->getName(), errLabel->getName()));
    stmts.push_back(inboundLabel);
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ta, nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, nodeFactory->IRConst(TIR::Configuration::WORD_SIZE), nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, ti, nodeFactory->IRConst(TIR::Configuration::WORD_SIZE))));
    std::shared_ptr<TIR::Mem> mem = nodeFactory->IRMem(offset);
    node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), mem);
}

void TransformVisitor::visit(std::shared_ptr<Program> n) {
    n->classOrInterfaceDecl->accept(this);
}

void TransformVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    auto temp = nodeFactory->IRTemp(n->id->name);
    std::shared_ptr<TIR::Expr> exp;
    if (n->exp) {
        n->exp->accept(this);
        exp = std::dynamic_pointer_cast<TIR::Expr>(node);
    }
    else {
        exp = nodeFactory->IRConst(0);
    }
    node = nodeFactory->IRMove(temp, exp);
}

void TransformVisitor::visit(std::shared_ptr<IdentifierExp> n) {
    node = nodeFactory->IRTemp(n->id->name);
}

void TransformVisitor::visit(std::shared_ptr<NotExp> n) {
    n->exp->accept(this);
    node = nodeFactory->IRUnaryOp(TIR::UnaryOp::OpType::NOT, std::dynamic_pointer_cast<TIR::Expr>(node));
}

void TransformVisitor::visit(std::shared_ptr<NegExp> n) {
    n->exp->accept(this);
    node = nodeFactory->IRUnaryOp(TIR::UnaryOp::OpType::NEG, std::dynamic_pointer_cast<TIR::Expr>(node));
}

std::shared_ptr<TIR::CompUnit> TransformVisitor::getCompUnit() const {
    return std::dynamic_pointer_cast<TIR::CompUnit>(node);
}
