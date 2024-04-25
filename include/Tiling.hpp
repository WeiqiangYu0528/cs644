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
    std::vector<std::string>& staticFields;

    std::unordered_map<std::string, std::string> regUsage;
    std::set<std::string> spilledVar;
    std::set<std::string> lockedReg;
    bool optimized;

public:
    RegisterManager(std::unordered_map<std::string, std::string>& regAlloc, std::set<std::string>& spills,
            int& currentStackOffset, std::unordered_map<std::string, int>& tempToStackOffset, std::vector<std::string>& staticFields, bool optimized
               ) : 
                registerAlloc(regAlloc),
                spills(spills),
                currentStackOffset(currentStackOffset),
                tempToStackOffset(tempToStackOffset),
                staticFields(staticFields),
                optimized(optimized)
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
    
    std::string getRegOrStackOffset(const std::string& var, std::vector<std::string>& assembly) {
        


        if(var.starts_with(TIR::Configuration::ABSTRACT_ARG_PREFIX) || var == TIR::Configuration::ABSTRACT_RET)
        {
            return "[ebp" + offset(var)  + "]";   
        } 
        
        // assert(registerAlloc.contains(var) && spills.contains(var));
        // if (registerAlloc.contains(var) && !spills.contains(var))
        // {
        //     auto reg = registerAlloc[var];
        //     regUsage[reg] = var;
        //     return reg;
        // } else {

        //     if (std::find(staticFields.begin(), staticFields.end(), var) != staticFields.end())
        //         return "[" + var + "]";
        //     else             
        //         return "[ebp" + offset(var)  + "]";   
        // }
        if(optimized) {
            if (registerAlloc.contains(var) && !spilledVar.contains(var)) {
                    auto& reg = registerAlloc[var];
                    auto& currentVar = regUsage[reg];
                    if(currentVar != var) {
                        // std::cout << "current: " << var << " allocated to register " << reg << std::endl;
                        if (spills.contains(currentVar)) {
                            if(!spilledVar.contains(currentVar))                    
                            {
                                spilledVar.insert(currentVar);
                            }
                            if (std::find(staticFields.begin(), staticFields.end(), currentVar) != staticFields.end())
                            {
                                std::cout << "current: staticFields" << currentVar << " spilled into" << " [" + currentVar + "]" << std::endl;
                            }
                            else {
                                assembly.push_back("mov [ebp"  + offset(currentVar) + "], " + reg);
                                // std::cout << "current: " << currentVar << " spilled into" << " [ebp"  + offset(currentVar) + "]" << std::endl;
                            }
                        
                        }
                        regUsage[reg] = var;
                    }
                    return reg;            
                }         

            if(!registerAlloc.contains(var) && spills.contains(var) && !spilledVar.contains(var)) {
                spilledVar.insert(var);
                offset(var);
                // std::cout << "current: " << var << " spilled into" << " [ebp"  + offset(var) + "]" << std::endl;

            }
        }
        if (std::find(staticFields.begin(), staticFields.end(), var) != staticFields.end())
            return "[" + var + "]";
        else             
            return "[ebp" + offset(var)  + "]";   

    }
};


class Tiling {
public:
    Tiling(std::shared_ptr<SymbolTable> st, std::vector<std::string>& fields, std::vector<std::string>& tables);
    std::shared_ptr<SymbolTable> st;
    std::vector<std::string>& staticFields;
    std::vector<std::string>& vtables;
    std::unordered_map<std::string, int> tempToStackOffset;
    std::unique_ptr<RegisterManager> regManager;

    int currentStackOffset = 0;
    int currentPos = 0;
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
