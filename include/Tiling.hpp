#pragma once
#include "IRAst.hpp"
#include <vector>
#include <string>


class Tiling {
public:
    void tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assemblyInstructions);
    void tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly);
    void tileEdst(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& instructions);
    void tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly);
    void tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly);

    void tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assemblyInstructions);
    std::string opTypeToAssembly(const TIR::BinOp::OpType& opType);
    void tileCall(const std::shared_ptr<TIR::Call>& node, std::vector<std::string>& assembly);
    void tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly);
    void tileExp(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& assembly);
    void tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& assembly);
    void tileTemp(const std::shared_ptr<TIR::Temp>& node, std::vector<std::string>& assembly);
    void tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& assembly);
    void tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& assembly);
    void tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& instructions);
};