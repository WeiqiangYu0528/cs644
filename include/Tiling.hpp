#pragma once
#include <vector>
#include <string>
#include "Configuration.hpp"
#include "IRAst.hpp"
#include "symbolTable.hpp"
#include "linearScanner.hpp"

#include <cassert>
#include <unordered_map>
#include <string>
#include <stdexcept>


class RegisterManager {
private:
    std::unordered_map<std::string, std::string>& registerAlloc;
    std::set<std::string>& spills;
    int& currentStackOffset;
    std::unordered_map<std::string, int>& tempToStackOffset;

    std::unordered_map<std::string, std::string> regUsage;
    std::set<std::string> spilledVars;

public:
    RegisterManager(std::unordered_map<std::string, std::string>& regAlloc, std::set<std::string>& spills,
                int& currentStackOffset, std::unordered_map<std::string, int>& tempToStackOffset) : 
                registerAlloc(regAlloc),
                spills(spills),
                currentStackOffset(currentStackOffset),
                tempToStackOffset(tempToStackOffset)                
                {
                    for (const auto& pair : regAlloc) {
                        if (!pair.second.empty()) {
                            regUsage[pair.second] = "";
                        }
                    }
                }
    
    std::string offset(const std::string& var)
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

    void spillToStack(const std::string& var, const std::string& offset_string, const std::string& reg, std::vector<std::string>& assembly) {
        assembly.push_back("mov [ebp" + offset_string  + "], " + reg);   
        spilledVars.insert(var);
    }    

    void temporaryUseRegs(const std::vector<std::string>& regs, std::vector<std::string>& assembly, std::function<void()> useRegs) {
        std::vector<std::pair<std::string, std::string>> spilledVars;

        for (const auto& reg : regs) {
            if (!regUsage[reg].empty()) {
                auto var = regUsage[reg];
                std::string offset_string = offset(var);
                spillToStack(var, offset_string, reg, assembly);
                spilledVars.emplace_back(reg, var);
            }
        }

        useRegs();
        
        for (const auto& spilled : spilledVars) {
            const auto& reg = spilled.first;
            const auto& var = spilled.second;
            std::string offset_string = offset(var);
            assembly.push_back("mov " + reg + ", [ebp" + offset_string + "]");
        }
    }


    std::string getRegOrStackOffset(std::string var, std::vector<std::string>& assembly);
};


class Tiling {
public:
    Tiling(std::shared_ptr<SymbolTable> st, std::vector<std::string>& fields);
    std::shared_ptr<SymbolTable> st;
    std::vector<std::string>& staticFields;
    std::unordered_map<std::string, int> tempToStackOffset;
    std::unique_ptr<RegisterManager> regManager;

    int currentStackOffset = 0;
    bool callFlag = false;
    void move(const std::string& dst, const std::string& src, std::vector<std::string>& assembly);
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
