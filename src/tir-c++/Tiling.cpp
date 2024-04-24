#include "Tiling.hpp"
#include "unordered_set"
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

Tiling::Tiling(std::shared_ptr<SymbolTable> st, std::vector<std::string>& fields, std::vector<std::string>& tables) : st(st), staticFields(fields), vtables(tables) {
}

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
    tileExp(node->getSource(), assembly);
    tileExp(node->getTarget(), assembly, false);
}

// jump(e)
void Tiling::tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly) {
    // TODO Fix jump
    assembly.push_back("jmp label" + std::dynamic_pointer_cast<TIR::Name>(node->getTarget())->getName());
}

// cjump(e, l)
void Tiling::tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly) {
    // jmp | jz | jnz | jg 
    std::string jump;
    if(callFlag) {
        tileExp(node->getCond(), assembly, "eax");
        callFlag = false;
    }
    
    tileExp(node->getCond(), assembly);
    
    if (auto binop = std::dynamic_pointer_cast<TIR::BinOp>(node->getCond())) {
        if (binop->getOpType() == TIR::BinOp::OpType::EQ) jump = "je";
        else if (binop->getOpType() == TIR::BinOp::OpType::NEQ) jump = "jne";
        else if (binop->getOpType() == TIR::BinOp::OpType::LT) jump = "jl";
        else if (binop->getOpType() == TIR::BinOp::OpType::LTU) jump = "jb";
        else if (binop->getOpType() == TIR::BinOp::OpType::LEQ) jump = "jle";
        else if (binop->getOpType() == TIR::BinOp::OpType::GT) jump = "jg";
        else if (binop->getOpType() == TIR::BinOp::OpType::GEQ) jump = "jge";
        else {
            assembly.push_back("cmp ebx, 1");
            jump = "je";
        }
    } else {
        assembly.push_back("cmp ebx, 1");
        jump = "je";
    }
    std::string jumpInstruction = jump + " label" + node->getTrueLabel();
    assembly.push_back(jumpInstruction);
}

// label(l)
void Tiling::tileLabel(const std::shared_ptr<TIR::Label>& node, std::vector<std::string>& assembly) {
    assembly.push_back("\nlabel" + node->getName() + ":");
}

// call(e)
void Tiling::tileCall(const std::shared_ptr<TIR::Call_s>& node, std::vector<std::string>& assembly) {
    const auto& args = node->getArgs();
    std::string funcName = node->getSignature();
    if (funcName == "__malloc" || funcName == "NATIVEjava.io.OutputStream.nativeWrite") {
        tileExp(args[0], assembly);
        assembly.push_back("mov eax, ebx");
    }
    else {
        for (int i = args.size() - 1; i >= 0; --i) {
            tileExp(args[i], assembly);
            assembly.push_back("push ebx");
        }
    }
    tileExp(node->getTarget(), assembly);
    assembly.push_back("call ebx");
    if (funcName != "__exception" && funcName.substr(funcName.length() - 4) != "void")
        callFlag = true;
}

// return(e)
void Tiling::tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly) {
    tileExp(node->getRet(), assembly);
    assembly.push_back("mov eax, ebx");
    assembly.push_back("mov esp, ebp");
    assembly.push_back("pop ebp");
    assembly.push_back("ret");
}

void Tiling::tileExp(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& assembly, bool read) {
    // Exp only have const(n), temp(t), op(e1, e2), mem(e), name(l)
    if (auto constNode = std::dynamic_pointer_cast<TIR::Const>(node)) {
        tileConst(constNode, assembly);
    } else if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        tileTemp(tempNode, assembly, read);
    } else if (auto binOpNode = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        tileBinOp(binOpNode, assembly);
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        tileMem(memNode, assembly, read);
    } else if (auto nameNode = std::dynamic_pointer_cast<TIR::Name>(node)) {
        tileName(nameNode, assembly);
    }
}

void Tiling::tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& assembly) {
    assembly.push_back("mov ebx, " + node->getName());
}

void Tiling::tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assembly) {    
    tileExp(binOp->getRight(), assembly);
    assembly.push_back("mov ecx, ebx");
    tileExp(binOp->getLeft(), assembly);
    auto op = binOp->getOpType();
    if (op == TIR::BinOp::OpType::DIV) {
        assembly.push_back("mov eax, ebx");
        assembly.push_back("cdq");
        assembly.push_back("mov ebx, ecx");
        assembly.push_back("cmp ebx, 0");
        assembly.push_back("je zerodivisionlabel");
        assembly.push_back(opTypeToAssembly(op) + " ebx");
        assembly.push_back("mov ebx, eax");
    }
    else if (op == TIR::BinOp::OpType::MOD) {
        assembly.push_back("mov eax, ebx");
        assembly.push_back("cdq");
        assembly.push_back("mov ebx, ecx");
        assembly.push_back("cmp ebx, 0");
        assembly.push_back("je zerodivisionlabel");
        assembly.push_back(opTypeToAssembly(op) + " ebx");
        assembly.push_back("mov ebx, edx");
    }
    else {
        assembly.push_back(opTypeToAssembly(op) + " ebx, ecx");
    }
    if (op == TIR::BinOp::OpType::LT) {
        assembly.push_back("setl bl");
        assembly.push_back("movzx ebx, bl");
    }
    else if (op == TIR::BinOp::OpType::LTU) {
        assembly.push_back("setb bl");
        assembly.push_back("movzx ebx, bl");
    }
    else if (op == TIR::BinOp::OpType::LEQ) {
        assembly.push_back("setle bl");
        assembly.push_back("movzx ebx, bl");
    }
    else if (op == TIR::BinOp::OpType::GT) {
        assembly.push_back("setg bl");
        assembly.push_back("movzx ebx, bl");
    }
    else if (op == TIR::BinOp::OpType::GEQ) {
        assembly.push_back("setge bl");
        assembly.push_back("movzx ebx, bl");
    }
    else if (op == TIR::BinOp::OpType::EQ) {
        assembly.push_back("sete bl");
        assembly.push_back("movzx ebx, bl");
    }
    else if (op == TIR::BinOp::OpType::NEQ) {
        assembly.push_back("setne bl");
        assembly.push_back("movzx ebx, bl");
    }
}

// ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT, EQ, NEQ, LT, LTU, GT, LEQ, GEQ
std::string Tiling::opTypeToAssembly(const TIR::BinOp::OpType& opType) {
    switch (opType) {
        case TIR::BinOp::OpType::ADD: return "add";
        case TIR::BinOp::OpType::SUB: return "sub";
        case TIR::BinOp::OpType::MUL: return "imul";
        case TIR::BinOp::OpType::DIV: return "idiv";
        case TIR::BinOp::OpType::MOD: return "idiv";
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

void Tiling::tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& assembly) {
    assembly.push_back("mov ebx, " + std::to_string(node->getValue()));
}

// mem(e)
void Tiling::tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& assembly, bool read) {
    if (!read) {
        assembly.push_back("mov ecx, ebx");
        tileExp(node->getExpr(), assembly);
        assembly.push_back("mov [ebx], ecx");
    }
    else {
        tileExp(node->getExpr(), assembly);
        assembly.push_back("mov ebx, [ebx]");
    }
}

void Tiling::tileTemp(const std::shared_ptr<TIR::Temp>& node, std::vector<std::string>& assembly, bool read) {
    std::string localVarName{node->getName()};
    if (std::find(vtables.begin(), vtables.end(), localVarName) != vtables.end()) {
        assembly.push_back("mov ebx, " + localVarName);
        return;
    }
    if (!tempToStackOffset.contains(localVarName)) {
        // assembly.push_back("sub esp, 4");
        currentStackOffset -= 4;
        tempToStackOffset[localVarName] = currentStackOffset;
    }
    auto offset = tempToStackOffset[localVarName];
    auto offset_string = std::to_string(tempToStackOffset[localVarName]);
    if (offset > 0)
        offset_string = std::string("+") + offset_string;

    if (localVarName == TIR::Configuration::ABSTRACT_RET && callFlag) {
        assembly.push_back("mov [ebp" + offset_string  + "], eax");
        callFlag = false;
    }
    if(read) {
        if (std::find(staticFields.begin(), staticFields.end(), localVarName) != staticFields.end()) {
            assembly.push_back("mov ebx, [" + localVarName + "]");
        }
        else assembly.push_back("mov ebx, [ebp" + offset_string  + "]");
    }
    else {
        if (std::find(staticFields.begin(), staticFields.end(), localVarName) != staticFields.end()) {
            assembly.push_back("mov [" + localVarName + "], ebx");
        }
        else assembly.push_back("mov [ebp" + offset_string  + "], ebx");
    }
}
