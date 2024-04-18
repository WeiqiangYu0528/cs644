#include <cassert>
#include <algorithm>
#include "transformVisitor.hpp"
#include "Configuration.hpp"

int TransformVisitor::labelCounter = 0;
int TransformVisitor::arrayCounter = 0;

TransformVisitor::TransformVisitor(std::shared_ptr<Scope> s, std::shared_ptr<TIR::NodeFactory> nf) : scope(s), nodeFactory(nf), node(nullptr) {
}

void TransformVisitor::visit(std::shared_ptr<ClassDecl> n) {
    className = n->id->name;
    std::vector<std::shared_ptr<TIR::Move>> fields;
    std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> methods;
    for (auto fieldDecl: n->declarations[0]) {
        auto field = std::dynamic_pointer_cast<Field>(fieldDecl);
        std::string fieldName{className + "." + field->fieldName->name};
        if (field->isStatic && field->type->type != DataType::OBJECT) {
            field->accept(this);
            fields.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(fieldName), std::dynamic_pointer_cast<TIR::Expr>(node)));
        }
        else if (!field->isStatic) {
            scope->current->fields.push_back(field);
        }
    }
    for (auto methodDecl: n->declarations[1]) {
        auto method = std::dynamic_pointer_cast<Method>(methodDecl);
        if (!method->isNative) {
            method->accept(this);
            methods[method->getSignature()] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
        }
        if (!method->isStatic) {
            scope->current->methods.push_back(method);
        }
    }
    for (auto constructorDecl: n->declarations[2]) {
        auto constructor = std::dynamic_pointer_cast<Constructor>(constructorDecl);
        constructor->accept(this);
        methods[constructor->getSignature()] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
    }
    std::shared_ptr<TIR::CompUnit> compUnit = nodeFactory->IRCompUnit(n->id->name, methods);
    compUnit->setFields(fields);
    node = compUnit;
}

void TransformVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    className = n->id->name;
    std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> methods;
    // for (auto method : n->methods) {
    //     if (method->isStatic && !method->isNative && method->type->type != DataType::OBJECT) {
    //         method->accept(this);
    //         methods[className + "." + method->methodName->name] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
    //     }
    // }
    node = nodeFactory->IRCompUnit(n->id->name, methods);
}

void TransformVisitor::visit(std::shared_ptr<Constructor> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Temp> obj = nodeFactory->IRTemp("obj");
    std::vector<std::shared_ptr<Field>>& fields = scope->current->fields;
    size_t fieldSize = fields.size();
    stmts.push_back(nodeFactory->IRMove(obj, nodeFactory->IRCall(nodeFactory->IRName("__malloc"), nodeFactory->IRConst((fieldSize + 1) * 4))));
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(obj), nodeFactory->IRTemp(TIR::Configuration::VTABLE)));
    for (size_t i = 0; i < fieldSize; ++i) {
        fields[i]->accept(this);
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, obj, nodeFactory->IRConst((i + 1) * 4))), getExpr()));
    }
    for (size_t i = 0; i < n->formalParameters.size(); ++i) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(n->formalParameters[i]->variableName->name), nodeFactory->IRTemp(std::string(TIR::Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(i))));
    }
    n->block->accept(this);
    auto block = std::dynamic_pointer_cast<TIR::Seq>(node);
    stmts.push_back(block);
    stmts.push_back(nodeFactory->IRReturn(obj));
    node = nodeFactory->IRFuncDecl(n->getSignature(), n->formalParameters.size(), nodeFactory->IRSeq(stmts));
}

void TransformVisitor::visit(std::shared_ptr<Method> n) {
    int tempNum{TIR::Temp::counter};
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    size_t paramSize{n->formalParameters.size()};
    for (size_t i = 0; i < paramSize; ++i) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(n->formalParameters[i]->variableName->name), nodeFactory->IRTemp(std::string(TIR::Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(i))));
    }
    if (!n->isStatic) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp("this"), nodeFactory->IRTemp(std::string(TIR::Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(paramSize++))));
    }
    n->block->accept(this);
    auto block = std::dynamic_pointer_cast<TIR::Seq>(node);
    stmts.push_back(block);
    if (n->type->type == DataType::VOID && (block->getStmts().empty() || block->getStmts().back()->getLabel() != "RETURN")) {
        stmts.push_back(nodeFactory->IRReturn(nodeFactory->IRConst(0)));
    }
    auto func = nodeFactory->IRFuncDecl(n->getSignature(), paramSize, nodeFactory->IRSeq(stmts));
    func->numTemps = TIR::Temp::counter - tempNum;
    node = func;
}

void TransformVisitor::visit(std::shared_ptr<PlusExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<MinusExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, exp1, exp2);
}
              
void TransformVisitor::visit(std::shared_ptr<TimesExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<DivideExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::DIV, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<ModuloExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::MOD, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<LessExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::LT, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<GreaterExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::GT, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<LessEqualExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::LEQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<GreaterEqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::GEQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<EqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<NotEqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::NEQ, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<AndExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::AND, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<OrExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::OR, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<XorExp> n) {
    n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::XOR, exp1, exp2);
}

void TransformVisitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR:Label:> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Temp> t = nodeFactory->IRTemp("t");
    stmts.push_back(nodeFactory->IRMove(t, nodeFactory->IRConst(0)));
    n->exp1->accept(this);
    stmts.push_back(nodeFactory->IRCJump(getExpr(), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    n->exp2->accept(this);
    stmts.push_back(nodeFactory->IRMove(t, getExpr()));
    stmts.push_back(falseLabel);
    node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), t);
}

void TransformVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Temp> t = nodeFactory->IRTemp("t");
    stmts.push_back(nodeFactory->IRMove(t, nodeFactory->IRConst(1)));
    n->exp1->accept(this);
    stmts.push_back(nodeFactory->IRCJump(getExpr(), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(falseLabel);
    n->exp2->accept(this);
    stmts.push_back(nodeFactory->IRMove(t, getExpr()));
    stmts.push_back(trueLabel);
    node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), t);
}

void TransformVisitor::visit(std::shared_ptr<Assignment> n) {
    if (auto leftexp = std::dynamic_pointer_cast<ArrayAccessExp>(n->left)) {
        std::vector<std::shared_ptr<TIR::Stmt>> stmts;
        std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
        std::shared_ptr<TIR::Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
        std::shared_ptr<TIR::Label> inboundLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
        std::shared_ptr<TIR::Temp> ta = nodeFactory->IRTemp("ta" + std::to_string(arrayCounter));
        std::shared_ptr<TIR::Temp> ti = nodeFactory->IRTemp("ti" + std::to_string(arrayCounter));
        std::shared_ptr<TIR::Temp> te = nodeFactory->IRTemp("te" + std::to_string(arrayCounter++));
        leftexp->array->accept(this);
        stmts.push_back(nodeFactory->IRMove(ta, getExpr()));
        stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, ta, nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
        stmts.push_back(errLabel);
        stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
        stmts.push_back(nonNullLabel);
        leftexp->index->accept(this);
        stmts.push_back(nodeFactory->IRMove(ti, getExpr()));
        stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LTU, ti, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, ta, nodeFactory->IRConst(4)))), inboundLabel->getName(), errLabel->getName()));
        stmts.push_back(inboundLabel);
        n->right->accept(this);
        stmts.push_back(nodeFactory->IRMove(te, getExpr()));
        std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ta, nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, ti, nodeFactory->IRConst(4)), nodeFactory->IRConst(4)));
        std::shared_ptr<TIR::Mem> mem = nodeFactory->IRMem(offset);
        stmts.push_back(nodeFactory->IRMove(mem, te));
        node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), te);
    } else {
        n->left->accept(this);
        auto left = getExpr();
        n->right->accept(this);
        auto right = getExpr();
        node = nodeFactory->IRMove(left, right);
    }
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

void TransformVisitor::visit(std::shared_ptr<NulLiteralExp> n) {
    node = nodeFactory->IRConst(0);
}

void TransformVisitor::visit(std::shared_ptr<SemicolonStatement> n) {
    node = nodeFactory->IRExp(nodeFactory->IRConst(0));
}

void TransformVisitor::visit(std::shared_ptr<IfStatement> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> endLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    n->exp->accept(this);
    stmts.push_back(nodeFactory->IRCJump(std::dynamic_pointer_cast<TIR::Expr>(node), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    n->statement1->accept(this);
    stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    stmts.push_back(nodeFactory->IRJump(nodeFactory->IRName(endLabel->getName())));
    stmts.push_back(falseLabel);
    if (n->statement2) {
        n->statement2->accept(this);
        stmts.push_back(std::dynamic_pointer_cast<TIR::Stmt>(node));
    }
    stmts.push_back(endLabel);
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
        stmts.push_back(getStmt());
    }
    stmts.push_back(forLabel);
    n->exp->accept(this);
    stmts.push_back(nodeFactory->IRCJump(getExpr(), trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    if (n->stmt2) {
        n->stmt2->accept(this);
        stmts.push_back(getStmt());
    }
    if (n->expStmt2) {
        n->expStmt2->accept(this);
        stmts.push_back(getStmt());
    
    }
    stmts.push_back(nodeFactory->IRJump(nodeFactory->IRName(forLabel->getName())));
    stmts.push_back(falseLabel);
    node = nodeFactory->IRSeq(stmts);
}

void TransformVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    std::shared_ptr<TIR::Expr> expr = nodeFactory->IRConst(0);
    if (n->exp) {
        n->exp->accept(this);
        expr = std::dynamic_pointer_cast<TIR::Expr>(node);
    }
    node = nodeFactory->IRReturn(expr);
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
        stmts.push_back(getStmt());
    }
    node = nodeFactory->IRSeq(stmts);
}

void TransformVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::vector<std::shared_ptr<TIR::Expr>> args;

    std::shared_ptr<TIR::Expr> expr;
    std::shared_ptr<SymbolTable> st;
    reclassifyAmbiguousName(n->exprs, expr, st);
    assert (expr != nullptr);
    assert (st != nullptr);
    for (auto arg : n->arguments) {
        arg->accept(this);
        args.push_back(std::dynamic_pointer_cast<TIR::Expr>(node));
    }
    if (n->method->isStatic) {
        std::shared_ptr<TIR::Call> call = nodeFactory->IRCall(nodeFactory->IRName(n->method->getSignature()), args);
        node = call;
    }
    else {
        args.push_back(expr);
        std::shared_ptr<TIR::Temp> vtable = nodeFactory->IRTemp("tdv");
        std::shared_ptr<TIR::Move> move = nodeFactory->IRMove(vtable, nodeFactory->IRMem(expr));
        size_t i = 0;
        for (; i < st->methods.size(); ++i) {
            if (st->methods[i] == n->method) break;
        }
        assert(i < st->methods.size());
        std::shared_ptr<TIR::Call> call = nodeFactory->IRCall(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, vtable, nodeFactory->IRConst(4 * i))), args);
        call->setSignature(n->method->getSignature());
        node = nodeFactory->IRESeq(move, call);
    }
}

void TransformVisitor::visit(std::shared_ptr<ParExp> n) {
    n->exp->accept(this);
}

void TransformVisitor::visit(std::shared_ptr<NewArrayExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> nonNegLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> inboundLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Temp> tn = nodeFactory->IRTemp("tn" + std::to_string(arrayCounter));
    std::shared_ptr<TIR::Temp> tm = nodeFactory->IRTemp("tm" + std::to_string(arrayCounter++));
    n->exp->accept(this);
    auto len = std::dynamic_pointer_cast<TIR::Expr>(node);
    stmts.push_back(nodeFactory->IRMove(tn, len));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LT, tn, nodeFactory->IRConst(0)), errLabel->getName(), nonNegLabel->getName()));
    stmts.push_back(errLabel);
    stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
    stmts.push_back(nonNegLabel);
    stmts.push_back(nodeFactory->IRMove(tm, nodeFactory->IRCall(nodeFactory->IRName("__malloc"), nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, tn, nodeFactory->IRConst(4)), nodeFactory->IRConst(8)))));
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(tm), tn));
    // for (size_t i = 1; i <= arrSize; ++i) {
    //     stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(i))), nodeFactory->IRConst(0)));
    // }
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(4));
    node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), offset);
}

void TransformVisitor::visit(std::shared_ptr<ArrayAccessExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> inboundLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Temp> ta = nodeFactory->IRTemp("ta" + std::to_string(arrayCounter));
    std::shared_ptr<TIR::Temp> ti = nodeFactory->IRTemp("ti" + std::to_string(arrayCounter++));
    n->array->accept(this);
    stmts.push_back(nodeFactory->IRMove(ta, getExpr()));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, ta, nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
    stmts.push_back(errLabel);
    stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
    stmts.push_back(nonNullLabel);
    n->index->accept(this);
    stmts.push_back(nodeFactory->IRMove(ti, getExpr()));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LTU, ti, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, ta, nodeFactory->IRConst(4)))), inboundLabel->getName(), errLabel->getName()));
    stmts.push_back(inboundLabel);
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ta, nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, ti, nodeFactory->IRConst(4)), nodeFactory->IRConst(4)));
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
    // std::string& str {n->id->name};
    // std::size_t lastDotPos = str.find_last_of(".");
    // bool isArray = false;
    // if (lastDotPos != std::string::npos) {
    //     std::string arr = str.substr(0, lastDotPos);
    //     std::string lastPart = str.substr(lastDotPos + 1);
    //     if (lastPart == "length") {
    //         isArray = true;
    //         std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    //         std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    //         std::shared_ptr<TIR::Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    //         std::shared_ptr<TIR::Temp> temp = nodeFactory->IRTemp(arr);
    //         stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, temp, nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
    //         stmts.push_back(errLabel);
    //         stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
    //         stmts.push_back(nonNullLabel);
    //         node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, temp, nodeFactory->IRConst(4))));
    //     }
    // }
    // if (!isArray) {
        // if (n->simple) node = nodeFactory->IRTemp(str);
    std::shared_ptr<TIR::Expr> expr;
    std::shared_ptr<SymbolTable> st;
    reclassifyAmbiguousName(n->exprs, expr, st);
    node = expr;
    // }
}

void TransformVisitor::visit(std::shared_ptr<CastExp> n) {
    n->exp->accept(this);
    int mask;
    switch (n->type->type) {
        case DataType::BYTE:
            mask = 0XFF;
            node = getCastExpr(mask);
            break;
        case DataType::SHORT:
            mask = 0XFFFF;
            node = getCastExpr(mask);
            break;
        case DataType::CHAR:
            mask = 0XFFFF;
            node = nodeFactory->IRBinOp(TIR::BinOp::OpType::AND, getExpr(), nodeFactory->IRConst(mask));
            break;
        default:
            break;
    }
}

void TransformVisitor::visit(std::shared_ptr<NotExp> n) {
    n->exp->accept(this);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::XOR, nodeFactory->IRConst(1), getExpr());
}

void TransformVisitor::visit(std::shared_ptr<NegExp> n) {
    n->exp->accept(this);
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::MUL, nodeFactory->IRConst(-1), getExpr());
}

void TransformVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    n->exp->accept(this);
    if (auto move = std::dynamic_pointer_cast<TIR::Move>(node)) {
        if (n->field->id->name == "length") {
            if (auto temp = std::dynamic_pointer_cast<TIR::Temp>(move->getTarget())) {
                node = nodeFactory->IRESeq(move, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, temp, nodeFactory->IRConst(4))));
            }
        }
    } else {
        node = nodeFactory->IRConst(0);
    }
    //undefined
    // node = nullptr;
}

void TransformVisitor::visit(std::shared_ptr<Field> n) {
    if (n->initializer) {
        n->initializer->accept(this);
    } else {
        node = nodeFactory->IRConst(0);
    }
}

void TransformVisitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    std::vector<std::shared_ptr<TIR::Expr>> args;
    for (auto arg : n->arguments) {
        arg->accept(this);
        args.push_back(std::dynamic_pointer_cast<TIR::Expr>(node));
    }

    node = nodeFactory->IRCall(nodeFactory->IRName(n->constructor->getSignature()), args);
}

std::shared_ptr<TIR::CompUnit> TransformVisitor::getCompUnit() const {
    return std::dynamic_pointer_cast<TIR::CompUnit>(node);
}

std::shared_ptr<TIR::Expr> TransformVisitor::getExpr() const {
    std::shared_ptr<TIR::Expr> ret;
    if (auto expr = std::dynamic_pointer_cast<TIR::Expr>(node)) {
        ret = expr;
    }
    else if (auto move = std::dynamic_pointer_cast<TIR::Move>(node)) {
        std::shared_ptr<TIR::Expr> expr;
        if (auto temp = std::dynamic_pointer_cast<TIR::Temp>(move->getTarget())) {
            expr = temp;
        }
        else if (auto eseq = std::dynamic_pointer_cast<TIR::ESeq>(move->getTarget())) {
            expr = eseq->getExpr();
        }
        ret = nodeFactory->IRESeq(move, expr);
    }
    return ret;
}

std::shared_ptr<TIR::Stmt> TransformVisitor::getStmt() const {
    std::shared_ptr<TIR::Stmt> ret;
    if (auto stmt = std::dynamic_pointer_cast<TIR::Stmt>(node)) {
        ret = stmt;
    }
    else if (auto expr = std::dynamic_pointer_cast<TIR::Expr>(node)) {
        ret = nodeFactory->IRExp(expr);
    }
    return ret;
}

std::shared_ptr<TIR::ESeq> TransformVisitor::getCastExpr(int mask) const {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> falseLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Temp> temp = nodeFactory->IRTemp("maskedValue");
    stmts.push_back(nodeFactory->IRMove(temp, nodeFactory->IRBinOp(TIR::BinOp::OpType::AND, getExpr(), nodeFactory->IRConst(mask)))); 
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::GT, temp, nodeFactory->IRConst(mask / 2)) , trueLabel->getName(), falseLabel->getName()));
    stmts.push_back(trueLabel);
    stmts.push_back(nodeFactory->IRMove(temp, nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, temp, nodeFactory->IRConst(mask + 1))));
    stmts.push_back(falseLabel);
    return nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), temp);
}

std::shared_ptr<TIR::Mem> TransformVisitor::getField(std::shared_ptr<TIR::Expr> expr, std::shared_ptr<SymbolTable> st, const std::string& name) const {
    std::vector<std::shared_ptr<Field>>& fields = st->fields;
    size_t i = 0;
    for (; i < fields.size(); ++i) {
        if (fields[i]->fieldName->name == name) {
            break;
        }
    }
    assert(i < fields.size());
    return nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, expr, nodeFactory->IRConst((i + 1) * 4)));
}

void TransformVisitor::reclassifyAmbiguousName(const std::vector<ExpressionObject>& exprs, std::shared_ptr<TIR::Expr>& expr, std::shared_ptr<SymbolTable>& st) {
    for (size_t i = 0; i < exprs.size(); ++i) {
        const ExpressionObject& exprObj = exprs[i];
        if (exprObj.exp == Expression::NON_STATIC_FIELD) {
            if (expr == nullptr) expr = nodeFactory->IRTemp("this");
            expr = getField(expr, st, exprObj.name);
        } 
        else {
            expr = nodeFactory->IRTemp(exprObj.name);
        }
        st = exprObj.st;
    }
    assert(expr != nullptr);
}
