#include <cassert>
#include "Configuration.hpp"
#include "canonicalVisitor.hpp"

using namespace TIR;

int CanonicalVisitor::labelCounter = 0;

CanonicalVisitor::CanonicalVisitor() {}

void CanonicalVisitor::visit(std::shared_ptr<CompUnit> cu) {
    for (auto pair : cu->getFunctions()) {
        visit(pair.second);
    }
}

void CanonicalVisitor::visit(std::shared_ptr<FuncDecl> fd) {
    std::shared_ptr<Seq> seq = std::dynamic_pointer_cast<Seq>(fd->body);
    assert(seq);

    int startLabelCounter = labelCounter;
    auto flatSeq = visit(seq);
    fd->numTemps = labelCounter - startLabelCounter + 1; //+1 is to make it more fault-tolerant
    fd->body = std::make_shared<Seq>(flatSeq.stmts);
}

CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Stmt> stmt) {
    if (auto seq = std::dynamic_pointer_cast<Seq>(stmt)) {
        return visit(seq);
    } else if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
        return visit(cjump);
    } else if (auto exp = std::dynamic_pointer_cast<Exp>(stmt)) {
        return visit(exp);
    } else if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
        return visit(jump);
    } else if (auto label = std::dynamic_pointer_cast<Label>(stmt)) {
        return visit(label);
    } else if (auto move = std::dynamic_pointer_cast<Move>(stmt)) {
        return visit(move);
    } else if (auto ret = std::dynamic_pointer_cast<Return>(stmt)) {
        return visit(ret);
    } else {
        std::cerr << "CanonicalVisitor tried visiting unknown subclass of Stmt, canonicalVisitor.cpp" << std::endl;
        assert(false);
    }
}


CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Seq> seq) {
    std::vector<std::shared_ptr<Stmt>> stmts = {};
    for (const std::shared_ptr<Stmt>& stmt : seq->getStmts()) {
        if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
            auto x = visit(cjump).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto exp = std::dynamic_pointer_cast<Exp>(stmt)) {
            auto x = visit(exp).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
            auto x = visit(jump).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto label = std::dynamic_pointer_cast<Label>(stmt)) {
            auto x = visit(label).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto move = std::dynamic_pointer_cast<Move>(stmt)) {
            auto x = visit(move).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto ret = std::dynamic_pointer_cast<Return>(stmt)) {
            auto x = visit(ret).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto _seq = std::dynamic_pointer_cast<Seq>(stmt)) {
            auto x = visit(_seq).stmts;
            stmts.insert(stmts.end(), x.begin(), x.end());
        }
    }
    return CanonicalVisitor::VisitResult(stmts);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<CJump> cjump) {
    CanonicalVisitor::VisitResult vr = visit(cjump->getCond());
    std::shared_ptr<CJump> _cjump = std::make_shared<CJump>(vr.pureExpr, cjump->getTrueLabel(), cjump->getFalseLabel());
    vr.stmts.push_back(_cjump);
    return CanonicalVisitor::VisitResult(vr.stmts);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Exp> exp) {
    CanonicalVisitor::VisitResult vr = visit(exp->getExpr());
    return CanonicalVisitor::VisitResult(vr.stmts);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Jump> jump) { 
    CanonicalVisitor::VisitResult vr = visit(jump->getTarget());
    std::shared_ptr<Jump> _jump = std::make_shared<Jump>(vr.pureExpr);
    vr.stmts.push_back(_jump);
    return CanonicalVisitor::VisitResult(vr.stmts);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Label> label) {
    return CanonicalVisitor::VisitResult({label});
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Move> move) {
    if (auto temp = std::dynamic_pointer_cast<Temp>(move->getTarget())) {
        labelCounter++;
        CanonicalVisitor::VisitResult vr = visit(move->getSource());
        std::shared_ptr<Move> _move = std::make_shared<Move>(temp, vr.pureExpr);
        vr.stmts.push_back(_move);
        return CanonicalVisitor::VisitResult(vr.stmts);
    } else {
        auto mem = std::dynamic_pointer_cast<Mem>(move->getTarget());
        assert(mem);
        CanonicalVisitor::VisitResult vr1 = visit(mem->getExpr());
        CanonicalVisitor::VisitResult vr2 = visit(move->getSource());
        std::shared_ptr<Temp> _temp = std::make_shared<Temp>(std::to_string(labelCounter++));
        std::shared_ptr<Move> _move1 = std::make_shared<Move>(_temp, vr1.pureExpr);
        std::shared_ptr<Mem> _mem = std::make_shared<Mem>(_temp);
        std::shared_ptr<Move> _move2 = std::make_shared<Move>(_mem, vr2.pureExpr);

        vr1.stmts.push_back(_move1);
        vr1.stmts.insert(vr1.stmts.end(), vr2.stmts.begin(), vr2.stmts.end());
        vr1.stmts.push_back(_move2);
        return CanonicalVisitor::VisitResult(vr1.stmts);
    }
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Return> ret) {
    CanonicalVisitor::VisitResult vr = visit(ret->getRet());
    std::shared_ptr<Return> _ret = std::make_shared<Return>(vr.pureExpr);
    // std::shared_ptr<Temp> temp = std::make_shared<Temp>("RET");
    // std::shared_ptr<Move> move = std::make_shared<Move>(temp, vr.pureExpr);
    // vr.stmts.push_back(move);
    vr.stmts.push_back(_ret);
    return CanonicalVisitor::VisitResult(vr.stmts);
}

CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Expr> expr) {
    //this selects the other expr types
    if (auto binop = std::dynamic_pointer_cast<BinOp>(expr)) {
        return visit(binop);
    } else if (auto call = std::dynamic_pointer_cast<Call>(expr)) {
        return visit(call);
    } else if (auto _const = std::dynamic_pointer_cast<Const>(expr)) {
        return visit(_const);
    } else if (auto mem = std::dynamic_pointer_cast<Mem>(expr)) {
        return visit(mem);
    } else if (auto name = std::dynamic_pointer_cast<Name>(expr)) {
        return visit(name);
    } else if (auto temp = std::dynamic_pointer_cast<Temp>(expr)) {
        labelCounter++;
        return visit(temp);
    } else if (auto eseq = std::dynamic_pointer_cast<ESeq>(expr)) {
        return visit(eseq);
    } else {
        std::cerr << "visiting unknown Expr type, canonicalVisitor.cpp" << std::endl;
        assert(false);
        return CanonicalVisitor::VisitResult(expr); //just to avoid the compiler warnings
    }
} 

CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Const> _const) {
    //no side effects
    return CanonicalVisitor::VisitResult(_const);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Temp> temp) {
    //no side effects
    return CanonicalVisitor::VisitResult(temp);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Name> name) {
    //no side effects
    return CanonicalVisitor::VisitResult(name);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Mem> mem) {
    CanonicalVisitor::VisitResult vr = visit(mem->getExpr());
    std::shared_ptr<Mem> _mem = std::make_shared<Mem>(vr.pureExpr);
    return CanonicalVisitor::VisitResult(vr.stmts, _mem);
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<ESeq> eseq) {
    CanonicalVisitor::VisitResult vr1 = visit(eseq->getStmt());
    CanonicalVisitor::VisitResult vr2 = visit(eseq->getExpr());
    vr1.stmts.insert(vr1.stmts.end(), vr2.stmts.begin(), vr2.stmts.end());
    return CanonicalVisitor::VisitResult(vr1.stmts, vr2.pureExpr);
    
}
CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<BinOp> binop) {
    CanonicalVisitor::VisitResult vr1 = visit(binop->getLeft());
    CanonicalVisitor::VisitResult vr2 = visit(binop->getRight());
    //reuse vr1.stmts
    std::shared_ptr<Temp> temp = std::make_shared<Temp>(std::to_string(labelCounter++));
    std::shared_ptr<Move> move = std::make_shared<Move>(temp, vr1.pureExpr);
    vr1.stmts.push_back(move);
    vr1.stmts.insert(vr1.stmts.end(), vr2.stmts.begin(), vr2.stmts.end());
    std::shared_ptr<BinOp> _binop = std::make_shared<BinOp>(binop->getOpType(), temp, vr2.pureExpr);
    return CanonicalVisitor::VisitResult(vr1.stmts, _binop);
}

CanonicalVisitor::VisitResult CanonicalVisitor::visit(std::shared_ptr<Call> call) {
    std::vector<std::shared_ptr<Stmt>> stmts;
    std::vector<std::shared_ptr<Temp>> temps;
    // CanonicalVisitor::VisitResult vr;
    // std::shared_ptr<Temp> temp;
    // std::shared_ptr<Move> move;
    // stmts.insert(stmts.end(), vr.stmts.begin(), vr.stmts.end());
    // stmts.push_back(move);
    std::shared_ptr<Name> functionName = std::dynamic_pointer_cast<Name>(call->getTarget());

    for (auto expr : call->getArgs()) {
        CanonicalVisitor::VisitResult vr = visit(expr);
        std::shared_ptr<Temp> temp = std::make_shared<Temp>(std::to_string(labelCounter++));
        temps.push_back(temp);
        std::shared_ptr<Move> move = std::make_shared<Move>(temp, vr.pureExpr);
        stmts.insert(stmts.end(), vr.stmts.begin(), vr.stmts.end());
        stmts.push_back(move);
    }

    std::shared_ptr<Call_s> call_s = std::make_shared<Call_s>(functionName, temps);
    stmts.push_back(call_s);
    std::shared_ptr<Temp> temp = std::make_shared<Temp>(Configuration::ABSTRACT_RET);
    labelCounter++; //for temp counting purposes

    return CanonicalVisitor::VisitResult(stmts, temp);
}
