#pragma once
#include "IRAst.hpp"
#include <set>
#include <queue>
#include "IRCfg.hpp"
namespace TIR {

    class LinearScanner
    {
    public:
        CFG& cfg;

        std::unordered_map<std::string, std::string> register_allocation;
        std::set<std::string> spills;

        struct Interval {
            std::string name;
            int start;
            int end;     
            bool operator<(const Interval& other) const {
                return end < other.end;
            }       
        };

   
        std::vector<Interval> active;
        
        std::set<std::string> free_registers;
        bool isDefContext;
        int currentPos;
        int currentCounter;

        LinearScanner(CFG& cfg): cfg(cfg){ currentPos = 0; isDefContext = false;}
  
        void visit(std::shared_ptr<Stmt> stmt, std::shared_ptr<BasicBlock> block);      
        void visit(std::shared_ptr<Move> move, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Jump> jump, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<CJump> cjump, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Return> ret, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Call_s> call, std::shared_ptr<BasicBlock> block);         
        void visit(std::shared_ptr<Label> call, std::shared_ptr<BasicBlock> block);       

        void visit(std::shared_ptr<Expr> node, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<BinOp> node, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Mem> node, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Temp> node, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Const> node, std::shared_ptr<BasicBlock> block);  
        void visit(std::shared_ptr<Name> node, std::shared_ptr<BasicBlock> block);  

        void allocateRegisters(std::set<std::string> available_registers);
        void expireOldIntervals(Interval& interval);
        void spillAtInterval(Interval& interval);
        void allocateRegister(Interval& interval);
        void computeLiveVariables();
        void insertIntervalSorted(const Interval& interval);      
    };
}