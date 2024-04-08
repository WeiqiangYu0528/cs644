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
// contains flag bits set by assembly (e.g., cmp), tested by jcc z = zero
//           nz = not zero
//           l = less than
//           nge = less than
//           o = overflow
//           no = not overflow

void Tiling::tileStmt(const std::shared_ptr<TIR::Stmt>& stmt, std::vector<std::string>& assembly) {
    // stmt have move(e_dst, e), jump(e), cjump(e, l), label(l), return(e), call(e)
    // e is expression and should be handled by tileExp(e)
    if (auto move = std::dynamic_pointer_cast<TIR::Move>(stmt)) {
        tileMove(move, assembly);
    } else if (auto jump = std::dynamic_pointer_cast<TIR::Jump>(stmt)) {
        tileJump(jump, assembly);
    } else if (auto cJump = std::dynamic_pointer_cast<TIR::CJump>(stmt)) {
        tileCJump(cJump, assembly);
    } else if (auto label = std::dynamic_pointer_cast<TIR::Label>(stmt)) {
        tileLabel(label, assembly);
    } else if (auto ret = std::dynamic_pointer_cast<TIR::Return>(stmt)) {
        tileReturn(ret, assembly);
    } else if (auto call = std::dynamic_pointer_cast<TIR::Call>(stmt)) {
        tileCall(call, assembly);
    }
}

void Tiling::tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly) {
    std::string res;
    res += "mov eax, " + tileEdst(node->getTarget()) + " "+ node->getSource()->getLabel();
    assembly.push_back(res);
}

// edst = temp(t) | mem(e)
std::string Tiling::tileEdst(const std::shared_ptr<TIR::Expr>& node) {
    if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        return tempNode->getName();
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        return tileMem(memNode) + tileExp(memNode->getExpr());
    }
}

// jump(e)
void Tiling::tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly) {
    std::string res = "jmp " + tileExp(node->getTarget());
    assembly.push_back(res);
}

// cjump(e, l)
void Tiling::tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly) {
    // TODO Fix cjump
    std::string res = "cmp eax, " + tileExp(node->getCond()) + " , 0" + node->getLabel();
    assembly.push_back(res);
}

// label(l)
void Tiling::tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly) {
    assembly.push_back(node->getName() + ":");
}

// call(e)
void Tiling::tileCall(const std::shared_ptr<TIR::Call>& node, std::vector<std::string>& assembly) {
    std::string res = "mov eax, " + tileExp(node->getTarget());
    assembly.push_back(res);
}

// return(e)
void Tiling::tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly) {
    assembly.push_back("mov eax, " + tileExp(node->getRet()));
    assembly.push_back("ret");
}

std::string Tiling::tileExp(const std::shared_ptr<TIR::Expr>& node) {
    // Exp only have const(n), temp(t), op(e1, e2), mem(e), name(l)
    if (auto constNode = std::dynamic_pointer_cast<TIR::Const>(node)) {
        return tileConst(constNode);
    } else if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        return "mov eax, " + tempNode->getName();
    } else if (auto binOpNode = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        return tileBinOp(binOpNode) + " " + tileExp(binOpNode->getLeft()) + " " + tileExp(binOpNode->getRight());
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        return tileMem(memNode);
    } else if (auto nameNode = std::dynamic_pointer_cast<TIR::Name>(node)) {
        return tileName(nameNode);
    }
}

std::string Tiling::tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp) {
    std::string res;
    std::string asmInstr = opTypeToAssembly(binOp->getOpType());
    std::string leftOperand = binOp->getLeft()->getLabel();
    std::string rightOperand = binOp->getRight()->getLabel();
    res += asmInstr + " eax, " + leftOperand + ", " + rightOperand;
    return res;
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

std::string Tiling::tileConst(const std::shared_ptr<TIR::Const>& node) {
    return std::to_string(node->getValue());
}

// mem(e)
std::string Tiling::tileMem(const std::shared_ptr<TIR::Mem>& node) {
    return "move eax, ["+  tileExp(node->getExpr()) + "]";
}

std::string Tiling::tileName(const std::shared_ptr<TIR::Name>& node) {
    return node->getName();
}