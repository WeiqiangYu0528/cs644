#pragma once
#include "IRAst.hpp"
#include <vector>
#include <string>


class Tiling {
public:
    void tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assembly);
    void tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly);
    void tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly);
    void tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly);
    void tileCall(const std::shared_ptr<TIR::Call>& node, std::vector<std::string>& assembly);
    void tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly);
    void tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly);

    std::string tileEdst(const std::shared_ptr<TIR::Expr>& node);
    std::string tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp);
    std::string opTypeToAssembly(const TIR::BinOp::OpType& opType);
    std::string tileExp(const std::shared_ptr<TIR::Expr>& node);
    std::string tileMem(const std::shared_ptr<TIR::Mem>& node);
    std::string tileTemp(const std::shared_ptr<TIR::Temp>& node);
    std::string tileConst(const std::shared_ptr<TIR::Const>& node);
    std::string tileName(const std::shared_ptr<TIR::Name>& node);
};