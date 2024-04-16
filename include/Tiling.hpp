#pragma once
#include <vector>
#include <string>
#include "Configuration.hpp"
#include "IRAst.hpp"
#include "symbolTable.hpp"

class Tiling {
public:
    Tiling(std::shared_ptr<SymbolTable> st, std::unordered_map<std::string, int>& staticFieldsMap);
    std::shared_ptr<SymbolTable> st;
    std::unordered_map<std::string, int>& staticFieldsMap;
    std::unordered_map<std::string, int> tempToStackOffset;
    int currentStackOffset = 0;
    bool callFlag = false;

    void tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assembly);
    void tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly);
    void tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly);
    void tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly);
    void tileCall(const std::shared_ptr<TIR::Call_s>& node, std::vector<std::string>& assembly);
    void tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly);
    void tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly);

    void tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assembly);
    void tileExp(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& assembly, const std::string& register_ = "");
    void tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& assembly, const std::string& register_ = "");
    void tileTemp(const std::shared_ptr<TIR::Temp>& node, std::vector<std::string>& assembly, const std::string& register_ = "");
    void tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& assembly);
    void tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& assembly);
    std::string opTypeToAssembly(const TIR::BinOp::OpType& opType);
};
