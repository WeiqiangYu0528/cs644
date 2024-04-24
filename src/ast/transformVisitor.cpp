#include <cassert>
#include <algorithm>
#include "transformVisitor.hpp"
#include "Configuration.hpp"
#include "subtypeTable.hpp"

int TransformVisitor::labelCounter = 0;
int TransformVisitor::arrayCounter = 0;
int TransformVisitor::tempCounter = 0;

TransformVisitor::TransformVisitor(std::shared_ptr<Scope> s, std::shared_ptr<TIR::NodeFactory> nf, std::vector<std::string>& staticFields, std::vector<std::shared_ptr<TIR::Stmt>>& staticFieldStmts, std::vector<std::vector<std::string>>& staticMethods) : scope(s), nodeFactory(nf), node(nullptr), staticFields(staticFields), staticFieldStmts(staticFieldStmts), staticMethods(staticMethods) {
}

void TransformVisitor::visit(std::shared_ptr<ClassDecl> n) {
    className = n->id->name;
    std::vector<std::string> staticMethodVec;
    std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> methods;
    for (auto fieldDecl: n->declarations[0]) {
        auto field = std::dynamic_pointer_cast<Field>(fieldDecl);
        std::string fieldName{className + "." + field->fieldName->name};
        std::replace(fieldName.begin(), fieldName.end(), '.', '_');
        if (field->isStatic) {
            field->accept(this);
            staticFields.push_back(fieldName);
            staticFieldStmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp(fieldName), getExpr()));
        }
    }
    for (auto methodDecl: n->declarations[1]) {
        auto method = std::dynamic_pointer_cast<Method>(methodDecl);
        if (!method->isAbstract) {
            if (method->isStatic && method->isNative) {
                int tempNum{TIR::Temp::counter};
                std::vector<std::shared_ptr<TIR::Stmt>> stmts;
                std::shared_ptr<TIR::Temp> argument = nodeFactory->IRTemp(method->formalParameters[0]->variableName->name);
                stmts.push_back(nodeFactory->IRMove(argument, nodeFactory->IRTemp(std::string(TIR::Configuration::ABSTRACT_ARG_PREFIX) + "0")));
                stmts.push_back(nodeFactory->IRReturn(nodeFactory->IRCall(nodeFactory->IRName("NATIVEjava.io.OutputStream.nativeWrite"), argument)));
                auto func = nodeFactory->IRFuncDecl(method->getSignature(), 1,  nodeFactory->IRSeq(stmts));
                func->numTemps = TIR::Temp::counter - tempNum;
                methods[method->getSignature()] = func;
            } else {
                method->accept(this);
                methods[method->getSignature()] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
            }
            staticMethodVec.push_back(method->getSignature());
        }
    }
    for (auto constructorDecl: n->declarations[2]) {
        auto constructor = std::dynamic_pointer_cast<Constructor>(constructorDecl);
        constructor->accept(this);
        methods[constructor->getSignature()] = std::dynamic_pointer_cast<TIR::FuncDecl>(node);
        staticMethodVec.push_back(constructor->getSignature());
    }
    std::shared_ptr<TIR::CompUnit> compUnit = nodeFactory->IRCompUnit(n->id->name, methods);
    staticMethods.push_back(std::move(staticMethodVec));
    node = compUnit;
}

void TransformVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    className = n->id->name;
    std::unordered_map<std::string, std::shared_ptr<TIR::FuncDecl>> methods;
    node = nodeFactory->IRCompUnit(className, methods);
    staticMethods.push_back(std::vector<std::string>{});
}

void TransformVisitor::visit(std::shared_ptr<Constructor> n) {
    int tempNum{TIR::Temp::counter};
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Temp> obj = nodeFactory->IRTemp("this");
    std::vector<std::shared_ptr<Field>>& fields = scope->current->getVtableFields();
    size_t fieldSize = fields.size();
    stmts.push_back(nodeFactory->IRMove(obj, nodeFactory->IRCall(nodeFactory->IRName("__malloc"), nodeFactory->IRConst((fieldSize + 1) * 4))));
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(obj), nodeFactory->IRTemp(className + "_vtable")));
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
    auto func = nodeFactory->IRFuncDecl(n->getSignature(), n->formalParameters.size(), nodeFactory->IRSeq(stmts));
    func->numTemps = TIR::Temp::counter - tempNum;
    node = func;
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
    if (n->stringConcat) {
        // std::cout << "Plus Expr" << std::endl;
        std::shared_ptr<SymbolTable> st = scope->getNameInScope("String", true);
        std::vector<std::shared_ptr<TIR::Stmt>> stmts;
        std::vector<std::shared_ptr<TIR::Expr>> args;
        std::shared_ptr<TIR::Temp> str1 = nodeFactory->IRTemp("str1_" + std::to_string(tempCounter));
        std::shared_ptr<TIR::Temp> str2 = nodeFactory->IRTemp("str2_" + std::to_string(tempCounter++));
        stmts.push_back(nodeFactory->IRMove(str1, getString(n->exp1)));
        stmts.push_back(nodeFactory->IRMove(str2, getString(n->exp2)));
        args.push_back(str2);
        args.push_back(str1);
        std::shared_ptr<TIR::Temp> vtable = nodeFactory->IRTemp("strtdv");
        stmts.push_back(nodeFactory->IRMove(vtable, nodeFactory->IRMem(str1)));
        size_t i = 0;
        const std::string funcSignature{"String_concat_String_String"};
        for (; i < st->getVtableMethods().size(); ++i) {
            if (st->getVtableMethods()[i]->getSignature() == funcSignature) break;
        }
        assert(i < st->getVtableMethods().size());
        std::shared_ptr<TIR::Call> call = nodeFactory->IRCall(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, vtable, nodeFactory->IRConst(4 * i))), args);
        call->setSignature(funcSignature);
        node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), call);
        data = DataType::STRING;
    } else {
        // std::cout << "non string" << std::endl;
        n->exp1->accept(this);
        auto exp1 = getExpr();
        n->exp2->accept(this);
        auto exp2 = getExpr();
        node = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, exp1, exp2);
        data = DataType::INT;
    }
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

void TransformVisitor::visit(std::shared_ptr<InstanceOfExp> n) {

    //if true, transform to true
    n->exp->accept(this);

    if (className == "String") {
        node = nodeFactory->IRConst(true);
        return;
    }

    std::shared_ptr<SymbolTable> exp_st;
    std::shared_ptr<IdentifierExp> idExp = std::dynamic_pointer_cast<IdentifierExp>(n->exp);
    if (idExp != nullptr && localTypes.contains(idExp->id->name)) {
        exp_st = localTypes[idExp->id->name];
    } else {
        exp_st = temp_st;
    }
    if (exp_st == nullptr) {
        node = nodeFactory->IRConst(false);
        return;
    }

    std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(n->type);
    std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(n->type);
    if (idType == nullptr && arrayType == nullptr) {
        throw std::runtime_error("Error: Instanceof used with type that isn't an IdentifierType or ArrayType");
    }


    if (idType) {

        std::shared_ptr<SymbolTable> type_st = scope->getNameInScope(idType->id->name, true);
        if (idType->id->name == "Object") {
            node = nodeFactory->IRConst(true);
            return;
        }
        
        if ((subtypeTable.contains(exp_st) && subtypeTable.at(exp_st).contains(type_st))
        || exp_st == type_st) {
            node = nodeFactory->IRConst(true);
        } else {
            node = nodeFactory->IRConst(false);
        }
    }

//if exp type is null, value should be false. Test this!!
//if exp type is array, we currently don't support it
}

void TransformVisitor::visit(std::shared_ptr<EqualExp> n) {
     n->exp1->accept(this);
    auto exp1 = getExpr();
    n->exp2->accept(this);
    auto exp2 = getExpr();
    node = nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, exp1, exp2);
    data = DataType::BOOLEAN;
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
        assert(left != nullptr);
        assert(right != nullptr);
        node = nodeFactory->IRMove(left, right);
    }
}

void TransformVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) {
    data = DataType::INT;
    node = nodeFactory->IRConst(static_cast<int>(n->value));
}

void TransformVisitor::visit(std::shared_ptr<BoolLiteralExp> n) {
    data = DataType::BOOLEAN;
    node = nodeFactory->IRConst(static_cast<int>(n->value));
}

void TransformVisitor::visit(std::shared_ptr<CharLiteralExp> n) {
    data = DataType::CHAR;
    node = nodeFactory->IRConst(static_cast<int>(n->value));
}

void TransformVisitor::visit(std::shared_ptr<NulLiteralExp> n) {
    data = DataType::NULLTYPE;
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

void TransformVisitor::visit(std::shared_ptr<StringLiteralExp> n) {
    std::string& str{n->value};
    int len = str.size();
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Temp> tm = nodeFactory->IRTemp("tm" + std::to_string(arrayCounter++));
    stmts.push_back(nodeFactory->IRMove(tm, nodeFactory->IRCall(nodeFactory->IRName("__malloc"), nodeFactory->IRConst(4 * len + 8))));
    stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(tm), nodeFactory->IRConst(len)));
    for (size_t i = 1; i <= len; ++i) {
        stmts.push_back(nodeFactory->IRMove(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(4 * (i + 1)))), nodeFactory->IRConst(str[i-1])));
    }
    std::shared_ptr<TIR::BinOp> offset = nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, tm, nodeFactory->IRConst(4));
    node = nodeFactory->IRCall(nodeFactory->IRName("Constructor_String_array_char"), std::vector<std::shared_ptr<TIR::Expr>>{nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), offset)});
    data = DataType::STRING;
}

void TransformVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::vector<std::shared_ptr<TIR::Expr>> args;

    for (auto arg : n->arguments) {
        arg->accept(this);
        args.push_back(std::dynamic_pointer_cast<TIR::Expr>(node));
    }
    if (n->method->isStatic) {
        std::shared_ptr<TIR::Call> call = nodeFactory->IRCall(nodeFactory->IRName(n->method->getSignature()), args);
        node = call;
    }
    else {
        std::vector<std::shared_ptr<TIR::Stmt>> stmts;
        if (n->primary && std::dynamic_pointer_cast<ThisExp>(n->primary) == nullptr) {
            n->primary->accept(this);
            stmts.push_back(nodeFactory->IRMove(nodeFactory->IRTemp("tmp"), getExpr()));
        }
        std::shared_ptr<TIR::Expr> expr;
        std::shared_ptr<SymbolTable> st;
        reclassifyAmbiguousName(n->exprs, expr, st);
        assert (expr != nullptr);
        assert (st != nullptr);
        args.push_back(expr);
        std::shared_ptr<TIR::Temp> vtable = nodeFactory->IRTemp("tdv");
        stmts.push_back(nodeFactory->IRMove(vtable, nodeFactory->IRMem(expr)));
        size_t i = 0;
        for (; i < st->getVtableMethods().size(); ++i) {
            if (st->getVtableMethods()[i] == n->method) break;
        }
        assert(i < st->getVtableMethods().size());
        std::shared_ptr<TIR::Call> call = nodeFactory->IRCall(nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, vtable, nodeFactory->IRConst(4 * i))), args);
        call->setSignature(n->method->getSignature());
        node = nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), call);
    }
    updateDataType(n->expr.type);
}

void TransformVisitor::visit(std::shared_ptr<ParExp> n) {
    n->exp->accept(this);
}

void TransformVisitor::visit(std::shared_ptr<NewArrayExp> n) {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> nonNegLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
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
    if (auto cice = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->exp)) {
        localTypes[n->id->name] = scope->getNameInScope(cice->constructor->constructorName->name, true);
    } else if (auto nullExp = std::dynamic_pointer_cast<NulLiteralExp>(n->exp)) {
        localTypes[n->id->name] = nullptr;
    }

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
    std::shared_ptr<TIR::Expr> expr;
    std::shared_ptr<SymbolTable> st;
    reclassifyAmbiguousName(n->exprs, expr, st);
    updateDataType(n->exprs.back().type);
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
    if (auto thisexp = std::dynamic_pointer_cast<ThisExp>(n->exp)) {
        std::shared_ptr<TIR::Expr> expr;
        std::shared_ptr<SymbolTable> st;
        reclassifyAmbiguousName(n->exprs, expr, st);
        assert(expr != nullptr);
        node = expr;
    } else {
        n->exp->accept(this);
        std::shared_ptr<TIR::Move> stmt = nodeFactory->IRMove(nodeFactory->IRTemp("tmp"), getExpr());
        assert(stmt != nullptr);
        std::shared_ptr<TIR::Expr> expr;
        std::shared_ptr<SymbolTable> st;
        // std::cout << n->exprs.size() << std::endl;
        reclassifyAmbiguousName(n->exprs, expr, st);
        assert(expr != nullptr);
        node = nodeFactory->IRESeq(stmt, expr);
    }
    // if (auto move = std::dynamic_pointer_cast<TIR::Move>(node)) {
    //     if (n->field->id->name == "length") {
    //         if (auto temp = std::dynamic_pointer_cast<TIR::Temp>(move->getTarget())) {
    //             node = nodeFactory->IRESeq(move, nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, temp, nodeFactory->IRConst(4))));
    //         }
    //     }
    // }
}

void TransformVisitor::visit(std::shared_ptr<ThisExp> n) {
    node = nodeFactory->IRTemp("this");
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
        else if (auto mem = std::dynamic_pointer_cast<TIR::Mem>(move->getTarget())) {
            expr = mem;
        }
        assert(expr != nullptr);
        ret = nodeFactory->IRESeq(move, expr);
    }
    assert(ret != nullptr);
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
    assert(st != nullptr);
    std::vector<std::shared_ptr<Field>>& fields = st->getVtableFields();
    size_t i = 0;
    for (; i < fields.size(); ++i) {
        if (fields[i]->fieldName->name == name) {
            break;
        }
    }
    assert(i < fields.size());
    return nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::ADD, expr, nodeFactory->IRConst((i + 1) * 4)));
}

std::shared_ptr<TIR::ESeq> TransformVisitor::getLength(std::shared_ptr<TIR::Expr> expr) const {
    std::vector<std::shared_ptr<TIR::Stmt>> stmts;
    std::shared_ptr<TIR::Label> errLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    std::shared_ptr<TIR::Label> nonNullLabel = nodeFactory->IRLabel(std::to_string(labelCounter++));
    stmts.push_back(nodeFactory->IRCJump(nodeFactory->IRBinOp(TIR::BinOp::OpType::EQ, expr, nodeFactory->IRConst(0)), errLabel->getName(), nonNullLabel->getName()));
    stmts.push_back(errLabel);
    stmts.push_back(nodeFactory->IRExp(nodeFactory->IRCall(nodeFactory->IRName("__exception"))));
    stmts.push_back(nonNullLabel);
    return nodeFactory->IRESeq(nodeFactory->IRSeq(stmts), nodeFactory->IRMem(nodeFactory->IRBinOp(TIR::BinOp::OpType::SUB, expr, nodeFactory->IRConst(4))));
}

void TransformVisitor::reclassifyAmbiguousName(const std::vector<ExpressionObject>& exprs, std::shared_ptr<TIR::Expr>& expr, std::shared_ptr<SymbolTable>& st) {
    if (exprs.empty() || exprs[0].exp == Expression::NON_STATIC_FIELD) {
        expr = nodeFactory->IRTemp("this");
        st = scope->current;
    }
    for (size_t i = 0; i < exprs.size(); ++i) {
        const ExpressionObject& exprObj = exprs[i];
        if (exprObj.exp == Expression::NON_STATIC_FIELD) {
            expr = getField(expr, st, exprObj.name);
        }
        else if (exprObj.exp == Expression::ARRAY) {
            expr = getLength(expr);
        }
        else {
            expr = nodeFactory->IRTemp(exprObj.name);
        }
        st = exprObj.st;
    }
    assert(expr != nullptr);
    temp_st = st;
}

std::shared_ptr<TIR::Expr> TransformVisitor::getString(std::shared_ptr<Exp> exp) {
    exp->accept(this);
    std::shared_ptr<TIR::Expr> expr = getExpr();
    std::string funcName;
    switch (data) {
        case DataType::INT:
            funcName = "String_valueOf_int_String";
            break;
        case DataType::BOOLEAN:
            funcName = "String_valueOf_boolean_String";
            break;
        case DataType::CHAR:
            funcName = "String_valueOf_char_String";
            break;
        case DataType::STRING:
            return expr;
        case DataType::NULLTYPE:
        case DataType::OBJECT:
            funcName = "String_valueOf_Object_String";
            break;
        default:
            assert(false);
    }
    std::vector<std::shared_ptr<TIR::Expr>> args;
    args.push_back(expr);
    return nodeFactory->IRCall(nodeFactory->IRName(funcName), args);
}

void TransformVisitor::updateDataType(std::shared_ptr<Type> type) {
    assert(type != nullptr);
    data = type->type;
    if (data == DataType::BYTE || data == DataType::SHORT) data = DataType::INT;
    if (data == DataType::OBJECT && type->typeToString() == "String") {
        data = DataType::STRING;
    }
}