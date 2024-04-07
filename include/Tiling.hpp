#pragma once
#include "IRAst.hpp"
#include <vector>
#include <string>


class Tiling {
public:
    Tiling() = default;
    void tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assemblyInstructions);
    void tileFunction(const std::shared_ptr<TIR::FuncDecl>& func, std::vector<std::string>& assemblyInstructions);
    void tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assemblyInstructions);
    std::string opTypeToAssembly(const TIR::BinOp::OpType& opType);
    void tileCall(const std::shared_ptr<TIR::Call>& node, std::vector<std::string>& assembly);
    void tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly);
    void tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly);
    void tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly);
    void tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly);
    void tileSeq(const std::shared_ptr<TIR::Seq>& node, std::vector<std::string>& assembly);
    void tileExp(const std::shared_ptr<TIR::Exp>& node, std::vector<std::string>& assembly);
    void tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& assembly);
    void tileTemp(const std::shared_ptr<TIR::Temp>& node, std::vector<std::string>& assembly);
    void tileESeq(const std::shared_ptr<TIR::ESeq>& node, std::vector<std::string>& assembly);
    void tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& assembly);
    void tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& assembly);
};
