#pragma once
#include "IRAst.hpp"

class CanonicalVisitor {
    private:
    public:
        CanonicalVisitor(/*Everything I need to populate private fields*/);
        void visit(std::shared_ptr<CompUnit> cu);
        void visit(std::shared_ptr<FuncDecl> fd);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Seq> seq);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<CJump> cjump);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Exp> exp);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Jump> jump);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Label> label);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Move> move);
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Return> ret);

        //std::pair<std::vector<std::shared_ptr<Stmt>>, std::shared_ptr<Expr>> visit(std::shared_ptr<Expr> expr);



};
