#include "canonicalVisitor.hpp"

CanonicalVisitor::CanonicalVisitor() {}

void CanonicalVisitor::visit(std::shared_ptr<CompUnit> cu) {
    for (auto pair : cu->getFunctions()) {
        visit(pair.second); //cu represents Program, has many functions.
    }
    //Now each FuncDecl should:
    //all SEQ and ESEQ are flattened into a single SEQ (per FuncDecl)
    //expressions have no side effects, and statements have <= 1 side effect each
    //CALLs are statements
    //CJUMP false case is fall-through

    //Now we should make FuncDecls all flattened into a single SEQ
}

void CanonicalVisitor::visit(std::shared_ptr<FuncDecl> fd) {
    std::shared_ptr<Seq> seq = std::dynamic_pointer_cast<Seq>(fd->body);
    assert(seq);
    auto flatSeq = visit(seq);
    fd->body = std::make_shared<Seq>(flatSeq);
    //Now this SEQ should be flattened into a single SEQ
    //all expressions should have no side effecst, and statements should have <= 1 side effect each
}

std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Seq> seq) {
    /*
    
    calling visit on seq should result in having a seq that is top-level, everything's been flattened
    seq has vector of Stmts
    so maybe visit can be void, and it can modify seq's vector
    */
    std::vector<std::shared_ptr<Stmt>> stmts = {};
    for (const std::shared_ptr<Stmt>& stmt : seq->getStmts()) {
        if (auto cjump = std::dynamic_pointer_cast<CJump>(stmt)) {
            auto x = visit(cjump);
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto exp = std::dynamic_pointer_cast<Exp>(stmt)) {
            auto x = visit(exp);
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto jump = std::dynamic_pointer_cast<Jump>(stmt)) {
            auto x = visit(jump);
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto label = std::dynamic_pointer_cast<Label>(stmt)) {
            auto x = visit(label);
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto move = std::dynamic_pointer_cast<Move>(stmt)) {
            auto x = visit(move);
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto ret = std::dynamic_pointer_cast<Return>(stmt)) {
            auto x = visit(ret);
            stmts.insert(stmts.end(), x.begin(), x.end());
        } else if (auto _seq = std::dynamic_pointer_cast<Seq>(stmt)) {
            auto x = visit(_seq);
            stmts.insert(stmts.end(), x.begin(), x.end());
        }
    }
    return stmts;
}
std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<CJump> cjump) {
    return {cjump};
    //UNIMPLEMENTED
}
std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Exp> exp) {
    auto pairEffectsPure = visit(exp->getExpr());
    return pairEffectsPure.first; //discard the expression result
}
std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Jump> jump) {
    auto pairEffectsPure = visit(jump->getTarget());
    std::shared_ptr<Jump> _jump = std::make_shared<Jump>(pairEffectsPure.second);
    pairEffectsPure.first.push_back(_jump);
    return pairEffectsPure.first;
}
std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Label> label) {
    return {label};
}
std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Move> move) {
    if (auto temp = std::dynamic_pointer_cast<Temp>(move->getTarget())) {
        auto pairEffectsPure = visit(move->getSource());
        std::shared_ptr<Move> _move = std::make_shared<Move>(temp, pairEffectsPure.second);
        pairEffectsPure.first.push_back(_move);
        return pairEffectsPure.first;
    } else {
        auto mem = std::dynamic_pointer_cast<Mem>(move->getTarget());
        assert(mem);
        auto pairEffectsPure1 = visit(move->getTarget());
        std::shared_ptr<Temp> _temp = std::make_shared<Temp>("some temp name");
        std::shared_ptr<Move> _move1 = std::make_shared<Move>(_temp, pairEffectsPure1.second);
        auto pairEffectsPure2 = visit(move->getSource());
        std::shared_ptr<Mem> _mem = std::make_shared<Mem>(_temp);
        std::shared_ptr<Move> _move2 = std::make_shared<Move>(_mem, pairEffectsPure2.second);
        std::vector<std::shared_ptr<Stmt>> combinedVector = {};
        combinedVector.insert(combinedVector.end(), pairEffectsPure1.first.begin(), pairEffectsPure1.first.end());
        combinedVector.push_back(_move1);
        combinedVector.insert(combinedVector.end(), pairEffectsPure2.first.begin(), pairEffectsPure2.first.end());
        combinedVector.push_back(_move2);
        return combinedVector;
    }
}
std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Return> ret) {
    auto pairEffectsPure = visit(ret->getRet());
    std::shared_ptr<Return> _ret = std::make_shared<Return>(pairEffectsPure.second);
    pairEffectsPure.first.push_back(_ret);
    return pairEffectsPure.first;
}

//returns pair of side effects and pure Expr - widely used
//std::pair<std::vector<std::shared_ptr<Stmt>>, std::shared_ptr<Expr>> visit(std::shared_ptr<Expr> expr) {
    //this selects the other expr types
//} 