#pragma once
#include <vector>
#include <string>
#include "Configuration.hpp"
#include "IRAst.hpp"
#include "symbolTable.hpp"
#include "linearScanner.hpp"


#include <unordered_map>
#include <string>
#include <stdexcept>


class RegisterManager {
private:
    std::unordered_map<std::string, std::string> registerAlloc;
    std::set<std::string> spills;
    std::set<std::string> spilled;
    std::unordered_map<std::string, std::string> regUsage;

public:
    RegisterManager() {}
    
    void reset(const std::unordered_map<std::string, std::string>& regAlloc,
                const std::set<std::string>& spills)
    {
        this->registerAlloc = regAlloc;
        this->spills = spills;
        for (const auto& pair : regAlloc) {
            regUsage[pair.second] = "";
        }

    }
    std::string offset(std::string var, int& currentStackOffset, std::unordered_map<std::string, int>& tempToStackOffset)
    {
        if (!tempToStackOffset.contains(var)) {
            currentStackOffset -= 4;
            tempToStackOffset[var] = currentStackOffset;
        }                
        auto offset = tempToStackOffset[var];
        auto offset_string = std::to_string(offset);
        if (offset > 0)
            offset_string = std::string("+") + offset_string;     
        return offset_string;
    }

    std::string getReg(std::string newVar, int& currentStackOffset, std::unordered_map<std::string, int>& tempToStackOffset, std::vector<std::string>& assembly) {
        auto reg = registerAlloc[newVar];
        auto regVar = regUsage[reg];
        if (!regVar.empty()) {
            if (newVar == regVar)
                return reg;
            else {                
                if(spills.contains(regVar)) {
                    auto offset_string = offset(regVar, currentStackOffset, tempToStackOffset);
                    spilled.insert(regVar);
                    assembly.push_back("mov [ebp" + offset_string  + "]," + reg);     
                }
                
                if(spilled.contains(newVar)) {
                    auto offset_string = offset(newVar, currentStackOffset, tempToStackOffset);
                    assembly.push_back("mov " + reg + ", " +  "[ebp" + offset_string  + "]"); 
                    spilled.erase(newVar);
                }

                regUsage[reg] = newVar;
                
                return reg;
            }            
        }
        else {
            regUsage[reg] = newVar;                             
        }
    }

};


class Tiling {
public:
    Tiling(std::shared_ptr<SymbolTable> st, std::vector<std::string>& fields);
    std::shared_ptr<SymbolTable> st;
    std::vector<std::string>& staticFields;
    std::unordered_map<std::string, int> tempToStackOffset;
    RegisterManager regManager;
    
    int currentStackOffset = 0;
    bool callFlag = false;

    void tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assembly);
    void tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly);
    void tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly);
    void tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly);
    void tileCall(const std::shared_ptr<TIR::Call_s>& node, std::vector<std::string>& assembly);
    void tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly);
    void tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly);

    std::string tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assembly);
    std::string tileExp(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& assembly);
    std::string tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& assembly);
    std::string tileTemp(const std::shared_ptr<TIR::Temp>& node, std::vector<std::string>& assembly);
    std::string tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& assembly);
    std::string tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& assembly);
    std::string opTypeToAssembly(const TIR::BinOp::OpType& opType);
};
