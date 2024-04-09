#pragma once
#include "IRCfg.hpp"

namespace TIR {
    class CfgVisitor
    {
    public:
        CFG cfg;
        std::shared_ptr<BasicBlock> currentBlock;
        std::shared_ptr<BasicBlock> lastBlock; 
        
        CfgVisitor();
        void visit(std::shared_ptr<CompUnit> cu);
        void visit(std::shared_ptr<FuncDecl> fd);
        
        std::vector<std::shared_ptr<Stmt>> visit(std::shared_ptr<Seq>& seq);
        void visit(std::shared_ptr<Stmt> stmt);
        void visit(std::shared_ptr<Move> move);
        void visit(std::shared_ptr<Jump> jump);
        void visit(std::shared_ptr<CJump> cjump);
        void visit(std::shared_ptr<Label> label);
        void visit(std::shared_ptr<Return> ret);
        void visit(std::shared_ptr<Call_s> call);
        
        void endCurrentBlock();
        void startNewBlock();
        void connectBlocks();
        void addStatment(std::shared_ptr<Stmt> stmt);
    };
}