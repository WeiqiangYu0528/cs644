#include "Tiling.hpp"

void Tiling::tileFunction(const std::shared_ptr<TIR::FuncDecl>& func, std::vector<std::string>& assemblyInstructions) {
    if (auto binOp = std::dynamic_pointer_cast<TIR::BinOp>(func->getBody())) {
        tileBinOp(binOp, assemblyInstructions);
    }
}

void Tiling::tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assemblyInstructions) {
    if (auto binOp = std::dynamic_pointer_cast<TIR::BinOp>(stmt)) {
        tileBinOp(binOp, assemblyInstructions);
    } else if (auto seq = std::dynamic_pointer_cast<TIR::Seq>(stmt)) {
        for (const auto& innerStmt : seq->getStmts()) {
            tileStmt(innerStmt, assemblyInstructions);
        }
    }
}

void Tiling::tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assemblyInstructions) {
    std::string asmInstr = opTypeToAssembly(binOp->getOpType());
    std::string leftOperand = binOp->getLeft()->getLabel();
    std::string rightOperand = binOp->getRight()->getLabel();
    assemblyInstructions.push_back(asmInstr + " " + leftOperand + ", " + rightOperand);
}

std::string Tiling::opTypeToAssembly(const TIR::BinOp::OpType& opType) {
    switch (opType) {
        case TIR::BinOp::OpType::ADD: return "add";
        case TIR::BinOp::OpType::SUB: return "sub";
        default: return "unknown";
    }
}

void Tiling::tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& instructions) {
    instructions.push_back("cmp eax, 0");
    instructions.push_back("jz " + node->getTrueLabel());
}