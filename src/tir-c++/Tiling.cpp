#include "Tiling.hpp"
// ISA: X86
// Opcodes:
//      mov | add | sub | mul | div | inc | dec | test | cmp | and | or | xor | not | shl
//      | shr | sar | jmp | jz | jnz | jg | push | pop | call | ret | ...
// Registers:
// eight 32-bit general-purpose registers: eax, ebx, ecx, edx, esi, edi, esp, ebp
// Operands: constant, register, memory address Addressing modes
// Size inference/directives
// Branching:
//      jcc  (cc is metavariable)
// cc register:
// contains flag bits set by instructions (e.g., cmp), tested by jcc z = zero
//           nz = not zero
//           l = less than
//           nge = less than
//           o = overflow
//           no = not overflow

void Tiling::tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assemblyInstructions) {
    // stmt have move(e_dst, e), jump(e), cjump(e, l), label(l), return(e), call(e)
    // e is expression and should be handled by tileExp(e)
    if (auto move = std::dynamic_pointer_cast<TIR::Move>(stmt)) {
        tileMove(move, assemblyInstructions);
    } else if (auto jump = std::dynamic_pointer_cast<TIR::Jump>(stmt)) {
        tileJump(jump, assemblyInstructions);
    } else if (auto cJump = std::dynamic_pointer_cast<TIR::CJump>(stmt)) {
        tileCJump(cJump, assemblyInstructions);
    } else if (auto label = std::dynamic_pointer_cast<TIR::Label>(stmt)) {
        tileLabel(label, assemblyInstructions);
    } else if (auto ret = std::dynamic_pointer_cast<TIR::Return>(stmt)) {
        tileReturn(ret, assemblyInstructions);
    } else if (auto call = std::dynamic_pointer_cast<TIR::Call>(stmt)) {
        tileCall(call, assemblyInstructions);
    }
}

void Tiling::tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& instructions) {
    instructions.push_back("mov " + node->getSource()->getLabel());
    tileEdst(node->getTarget(), instructions);
    instructions.push_back(node->getSource()->getLabel());
}

// edst = temp(t) | mem(e)
void Tiling::tileEdst(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& instructions) {
    if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        instructions.push_back("mov eax, " + tempNode->getLabel());
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        tileMem(memNode, instructions);
        tileExp(memNode->getExpr(), instructions);
    }
}

void Tiling::tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& instructions) {
    instructions.push_back("jmp " + node->getLabel());
}

void Tiling::tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& instructions) {
    instructions.push_back("cmp eax, 0");
    instructions.push_back("jz " + node->getTrueLabel());
}

void Tiling::tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& instructions) {
    instructions.push_back(node->getLabel() + ":");
}

void Tiling::tileCall(const std::shared_ptr<TIR::Call>& node, std::vector<std::string>& instructions) {
    instructions.push_back("call " + node->getLabel());
}

void Tiling::tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assemblyInstructions) {
    assemblyInstructions.push_back("mov eax, ");
    tileExp(node->getRet(), assemblyInstructions);
}

void Tiling::tileExp(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& instructions) {
    // Exp only have const(n), temp(t), op(e1, e2), mem(e), name(l)
    if (auto constNode = std::dynamic_pointer_cast<TIR::Const>(node)) {
        tileConst(constNode, instructions);
    } else if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        instructions.push_back("mov eax, " + tempNode->getLabel());
    } else if (auto binOpNode = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        tileBinOp(binOpNode, instructions);
        tileExp(binOpNode->getLeft(), instructions);
        tileExp(binOpNode->getRight(), instructions);
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        tileMem(memNode, instructions);
        tileExp(memNode->getExpr(), instructions);
    } else if (auto nameNode = std::dynamic_pointer_cast<TIR::Name>(node)) {
        tileName(nameNode, instructions);
    }
}

void Tiling::tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assemblyInstructions) {
    std::string asmInstr = opTypeToAssembly(binOp->getOpType());
    std::string leftOperand = binOp->getLeft()->getLabel();
    std::string rightOperand = binOp->getRight()->getLabel();
    assemblyInstructions.push_back(asmInstr + " " + leftOperand + ", " + rightOperand);
}

// ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT, EQ, NEQ, LT, LTU, GT, LEQ, GEQ
std::string Tiling::opTypeToAssembly(const TIR::BinOp::OpType& opType) {
    switch (opType) {
        case TIR::BinOp::OpType::ADD: return "add";
        case TIR::BinOp::OpType::SUB: return "sub";
        case TIR::BinOp::OpType::MUL: return "mul";
        case TIR::BinOp::OpType::DIV: return "div";
        case TIR::BinOp::OpType::AND: return "and";
        case TIR::BinOp::OpType::OR: return "or";
        case TIR::BinOp::OpType::XOR: return "xor";
        case TIR::BinOp::OpType::LSHIFT: return "shl";
        case TIR::BinOp::OpType::RSHIFT: return "shr";
        case TIR::BinOp::OpType::ARSHIFT: return "sar";
        case TIR::BinOp::OpType::EQ: return "cmp";
        case TIR::BinOp::OpType::NEQ: return "cmp";
        case TIR::BinOp::OpType::LT: return "cmp";
        case TIR::BinOp::OpType::LTU: return "cmp";
        case TIR::BinOp::OpType::GT: return "cmp";
        case TIR::BinOp::OpType::LEQ: return "cmp";
        case TIR::BinOp::OpType::GEQ: return "cmp";
        default: return "unknown";
    }
}

void Tiling::tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& instructions) {
    instructions.push_back("mov eax, " + node->getLabel());
}

void Tiling::tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& instructions) {
    instructions.push_back("mov eax, [" + node->getExpr()->getLabel() + "]");
}

void Tiling::tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& instructions) {
    instructions.push_back("mov eax, " + node->getLabel());
}