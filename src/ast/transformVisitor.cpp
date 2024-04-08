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
    }
    for (auto methodDecl: n->declarations[1]) {
        auto method = std::dynamic_pointer_cast<Method>(methodDecl);
        if (method->isStatic && !method->isNative && method->type->type != DataType::OBJECT) {
            bool isPrimitive = true;
            for (auto fp : method->formalParameters) {
                if (fp->type->type == DataType::OBJECT) isPrimitive = false;
            }
            if (!isPrimitive) continue;
            method->accept(this);
            methods[method->getSignature()] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
        }
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

void TransformVisitor::visit(std::shared_ptr<Method> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    for (size_t i = 0; i < n->formalParameters.size(); ++i) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(n->formalParameters[i]->variableName->name), nodeFactory->IRTemp(std::string(TIR::Configuration::ABSTRACT_ARG_PREFIX) + std::to_string(i))));
    }
    n->block->accept(this);
    auto block = std::dynamic_pointer_cast<TIR::Seq>(node);
    stmts.push_back(block);
    if (n->type->type == DataType::VOID && block->getStmts().back()->getLabel() != "RETURN") {
        stmts.push_back(nodeFactory->IRReturn(nullptr));
    }
    node = nodeFactory->IRFuncDecl(n->getSignature(), n->formalParameters.size(), nodeFactory->IRSeq(stmts)); 
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
    std::shared_ptr<TIR::Label> trueLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
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
        stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LTU, ti, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, ta, nodeFactory->IRConst(1)))), inboundLabel->getName(), errLabel->getName()));
        stmts.push_back(inboundLabel);
        n->right->accept(this);
        stmts.push_back(nodeFactory->IRMove(te, getExpr()));
        std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ta, nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ti, nodeFactory->IRConst(1)));
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
    std::shared_ptr<TIR::Expr> expr;
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
    for (auto arg : n->arguments) {
        arg->accept(this);
        args.push_back(std::dynamic_pointer_cast<TIR::Expr>(node));
    }

    node = nodeFactory->IRCall(nodeFactory->IRName(n->method->getSignature()), args);
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
    stmts.push_back(nodeFactory->IRMove(tm, nodeFactory->IRCall(nodeFactory->IRName("__malloc"), nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tn, nodeFactory->IRConst(2)))));
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(tm), tn));
    // for (size_t i = 1; i <= arrSize; ++i) {
    //     stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(i))), nodeFactory->IRConst(0)));
    // }
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(1));
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
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::LTU, ti, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, ta, nodeFactory->IRConst(1)))), inboundLabel->getName(), errLabel->getName()));
    stmts.push_back(inboundLabel);
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ta, nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, ti, nodeFactory->IRConst(1)));
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
    std::string& str {n->id->name};
    std::size_t lastDotPos = str.find_last_of(".");
    bool isArray = false;
    if (lastDotPos != std::string::npos) {
        std::string arr = str.substr(0, lastDotPos);
        std::string lastPart = str.substr(lastDotPos + 1);
        if (lastPart == "length") {
            isArray = true;
            std::vector<std::shared_ptr<TIR::Stmt>> stmts;
            std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
            std::shared_ptr<TIR::Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
            std::shared_ptr<TIR::Temp> temp = nodeFactory->IRTemp(arr);
            stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, temp, nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
            stmts.push_back(errLabel);
            stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
            stmts.push_back(nonNullLabel);
            node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, temp, nodeFactory->IRConst(1))));
        }
    }
    if (!isArray) {
        node = nodeFactory->IRTemp(str);
    }
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
                node = nodeFactory->IRESeq(move, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, temp, nodeFactory->IRConst(1))));
            }
        }
    } else {
        node = nullptr;
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
