#pragma once
#include "IRAst.hpp"
#include <set>
#include <queue>
namespace TIR {

    class LinearScanner
    {
    public:
        std::unordered_map<std::string, int> live_intervals_start;
        std::unordered_map<std::string, int> live_intervals_end;
        std::unordered_map<std::string, std::string> register_allocation;
        std::set<std::string> spills;

        struct Interval {
            std::string name;
            int start;
            int end;
            bool operator<(const Interval& other) const {
            if (end != other.end)
                    return end < other.end;
                return name < other.name;
            }            
        };
        std::set<Interval> active;
        std::set<std::string> free_registers;
        std::unordered_map<std::string, Interval> intervalTable;
       
 
        int currentPos = 0;

        LinearScanner();
        void visit(std::shared_ptr<FuncDecl> fd);        
        void visit(std::shared_ptr<Seq> seq);

        void visit(std::shared_ptr<Stmt> stmt);
        
        void visit(std::shared_ptr<Move> move);
        void visit(std::shared_ptr<Jump> jump);
        void visit(std::shared_ptr<CJump> cjump);
        void visit(std::shared_ptr<Return> ret);
        void visit(std::shared_ptr<Call_s> call);       
        void visit(std::shared_ptr<Label> call);      

        void visit(std::shared_ptr<Expr> node);
        void visit(std::shared_ptr<BinOp> node);
        void visit(std::shared_ptr<Mem> node);
        void visit(std::shared_ptr<Temp> node);
        void visit(std::shared_ptr<Const> node);
        void visit(std::shared_ptr<Name> node);

        void updateLiveInterval(const std::string& name, int position);
        void allocateRegisters(std::set<std::string> free_registers);
        void expireOldIntervals(const Interval& interval);
        void spillAtInterval(const Interval& interval);
        int generateNewStackLocation();
    };
}