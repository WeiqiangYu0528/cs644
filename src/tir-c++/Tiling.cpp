#include "Tiling.hpp"
#include <map>
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
    } else if (auto call = std::dynamic_pointer_cast<TIR::Call_s>(stmt)) {
        tileCall(call, assembly);
    }
}

void Tiling::tileMove(const std::shared_ptr<TIR::Move>& node, std::vector<std::string>& assembly) {
    std::string res;
    res += tileExp(node->getSource());
    res += tileExp(node->getTarget(), false) + "ebx" + "\n";
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
    // TODO Fix jump
    std::string res = "jmp label" + tileExp(node->getTarget());
    assembly.push_back(res);
}

// cjump(e, l)
void Tiling::tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly) {
    // jmp | jz | jnz | jg 
    std::string res = "cmp " + tileExp(node->getCond()) + ", 0";
    assembly.push_back(res);
    std::string jumpInstruction = "jne label" + node->getTrueLabel();
    assembly.push_back(jumpInstruction);
}

// label(l)
void Tiling::tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly) {
    assembly.push_back("label" + node->getName() + ":");
}

// call(e)
void Tiling::tileCall(const std::shared_ptr<TIR::Call_s>& node, std::vector<std::string>& assembly) {
    const auto& args = node->getArgs();
    for (int i = args.size() - 1; i >= 0; --i) {
        std::string argAssembly = tileExp(args[i]);
        assembly.push_back(argAssembly);
        assembly.push_back("push ebx");
    }

    std::string target = tileExp(node->getTarget());
    assembly.push_back("call " + target);
}

// return(e)
void Tiling::tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly) {
    assembly.push_back(tileExp(node->getRet()));
    assembly.push_back("mov eax, ebx");
    assembly.push_back("mov esp, ebp");
    assembly.push_back("mov pop, ebp");
    assembly.push_back("ret");
}

std::string Tiling::tileExp(const std::shared_ptr<TIR::Expr>& node, bool read) {
    // Exp only have const(n), temp(t), op(e1, e2), mem(e), name(l)
    std::string assembly;
    if (auto constNode = std::dynamic_pointer_cast<TIR::Const>(node)) {
        assembly = tileConst(constNode);
    } else if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        assembly = tileTemp(tempNode, read);
    } else if (auto binOpNode = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        assembly = tileBinOp(binOpNode);
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        assembly = tileMem(memNode);
    } else if (auto nameNode = std::dynamic_pointer_cast<TIR::Name>(node)) {
        assembly = tileName(nameNode);
    }
    return assembly;
}

std::string Tiling::tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp) {    
    std::string assembly;

    assembly += tileExp(binOp->getRight());
    assembly += "mov ecx, ebx\n";
    assembly += tileExp(binOp->getLeft());
    assembly += opTypeToAssembly(binOp->getOpType()) + " ebx, ecx" + "\n";
    return assembly;
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
    return std::string("mov ebx, " + std::to_string(node->getValue()) + "\n");
}

// mem(e)
std::string Tiling::tileMem(const std::shared_ptr<TIR::Mem>& node) {
    std::string assembly;
    assembly += tileExp(node->getExpr());
    assembly += "mov ebx, [ebx]\n";
    return assembly;
}

std::string Tiling::tileName(const std::shared_ptr<TIR::Name>& node) {
    return node->getName();
}

std::string Tiling::tileTemp(const std::shared_ptr<TIR::Temp>& node, bool read) {
    std::string assembly;
    if (!tempToStackOffset.contains(node->getName())) {
        assembly += "sub esp, 4\n"; 
        currentStackOffset -= 4;
        tempToStackOffset[node->getName()] = currentStackOffset;
    }
    auto offset = tempToStackOffset[node->getName()];
    auto offset_string = std::to_string(tempToStackOffset[node->getName()]);
    if (offset > 0)
        offset_string = std::string("+") + offset_string;

    if(read)
        assembly += std::string("mov ebx, ") + "[ebp" + offset_string  + "]" + "\n";
    else
        assembly += std::string("mov ") + "[ebp" + offset_string  + "], ";
    return assembly;
}

// "mov ebx, " + "[ebp" + std::to_string(it->second) + "]";
