#pragma once
#include "IRAst.hpp"

using namespace TIR;

class CanonicalVisitor {
    private:
        static int labelCounter;
    public:
        //calling visit on a Stmt only populates stmts
        //calling visit on an Expr populates stmts and pureExpr
        struct VisitResult {
            std::vector<std::shared_ptr<Stmt>> stmts;
            std::shared_ptr<Expr> pureExpr;

            VisitResult(std::vector<std::shared_ptr<Stmt>> s) : stmts(s) {}
            VisitResult(std::shared_ptr<Expr> e) : pureExpr(e) {}
            VisitResult(std::vector<std::shared_ptr<Stmt>> s, std::shared_ptr<Expr> e) : stmts(s), pureExpr(e) {}
        };

        CanonicalVisitor(/*Everything I need to populate private fields*/);
        void visit(std::shared_ptr<CompUnit> cu);
        void visit(std::shared_ptr<FuncDecl> fd);
        VisitResult visit(std::shared_ptr<Stmt> stmt);
        VisitResult visit(std::shared_ptr<Seq> seq);
        VisitResult visit(std::shared_ptr<CJump> cjump);
        VisitResult visit(std::shared_ptr<TIR::Exp> exp);
        VisitResult visit(std::shared_ptr<Jump> jump);
        VisitResult visit(std::shared_ptr<Label> label);
        VisitResult visit(std::shared_ptr<Move> move);
        VisitResult visit(std::shared_ptr<Return> ret);

        VisitResult visit(std::shared_ptr<Expr> expr);
        VisitResult visit(std::shared_ptr<Const> _const);
        VisitResult visit(std::shared_ptr<Temp> temp);
        VisitResult visit(std::shared_ptr<Name> name);
        VisitResult visit(std::shared_ptr<Mem> mem);
        VisitResult visit(std::shared_ptr<ESeq> eseq);
        VisitResult visit(std::shared_ptr<BinOp> binop);
        VisitResult visit(std::shared_ptr<Call> call);



};
