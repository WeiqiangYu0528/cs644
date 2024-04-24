#include "Tiling.hpp"
#include "unordered_set"

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

void Tiling::move(const std::string& dst, const std::string& src, std::vector<std::string>& assembly) {
    if (dst != src) {
        if (dst == "[" + src + "]") {
            auto t = assembly.back();
            assembly.pop_back();
            assembly.push_back("push ecx");
            assembly.push_back("mov ecx, " + src);            
            assembly.push_back(t);
            assembly.push_back("mov " + dst + ", ecx");
            assembly.push_back("pop ecx");
        } else {
            assembly.push_back("mov " + dst + ", " + src);
        }
    }
}


Tiling::Tiling(std::shared_ptr<SymbolTable> st, std::vector<std::string>& fields) : st(st), staticFields(fields) {
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
    move("ebx", tileExp(node->getSource(), assembly), assembly);
    move(tileExp(node->getTarget(), assembly), "ebx", assembly);
}

// jump(e)
void Tiling::tileJump(const std::shared_ptr<TIR::Jump>& node, std::vector<std::string>& assembly) {
    assembly.push_back("jmp label" + std::dynamic_pointer_cast<TIR::Name>(node->getTarget())->getName());
}

// cjump(e, l)
void Tiling::tileCJump(const std::shared_ptr<TIR::CJump>& node, std::vector<std::string>& assembly) {
    // jmp | jz | jnz | jg 
    std::string jump;
    if(callFlag) {
        auto cond = tileExp(node->getCond(), assembly);
        move("ebx", cond, assembly);
        callFlag = false;
    }
    
    auto cond = tileExp(node->getCond(), assembly);
    move("ebx", cond, assembly);


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
    if (funcName == "__malloc") {
        auto arg = tileExp(args[0], assembly);
        move("eax", arg, assembly);        
    }
    else {
        for (int i = args.size() - 1; i >= 0; --i) {
            auto arg = tileExp(args[i], assembly);
            move("ebx", arg, assembly);
            assembly.push_back("push ebx");
        }
    }
    auto target = tileExp(node->getTarget(), assembly);
    assembly.push_back("call " + target);
    if (funcName != "__exception" && funcName.substr(funcName.length() - 4) != "void")
        callFlag = true;
}

// return(e)
void Tiling::tileReturn(const std::shared_ptr<TIR::Return>& node, std::vector<std::string>& assembly) {
    auto ret = tileExp(node->getRet(), assembly);
    move("eax", ret, assembly);    
    assembly.push_back("mov esp, ebp");
    assembly.push_back("pop ebp");
    assembly.push_back("ret");
}

std::string Tiling::tileExp(const std::shared_ptr<TIR::Expr>& node, std::vector<std::string>& assembly) {
    // Exp only have const(n), temp(t), op(e1, e2), mem(e), name(l)
    if (auto constNode = std::dynamic_pointer_cast<TIR::Const>(node)) {
        return tileConst(constNode, assembly);
    } else if (auto tempNode = std::dynamic_pointer_cast<TIR::Temp>(node)) {
        return tileTemp(tempNode, assembly);
    } else if (auto binOpNode = std::dynamic_pointer_cast<TIR::BinOp>(node)) {
        return tileBinOp(binOpNode, assembly);
    } else if (auto memNode = std::dynamic_pointer_cast<TIR::Mem>(node)) {
        return tileMem(memNode, assembly);
    } else if (auto nameNode = std::dynamic_pointer_cast<TIR::Name>(node)) {
        return tileName(nameNode, assembly);
    }
    return "";
}

std::string Tiling::tileName(const std::shared_ptr<TIR::Name>& node, std::vector<std::string>& assembly) {
    return node->getName();
}

std::string Tiling::tileBinOp(const std::shared_ptr<TIR::BinOp>& binOp, std::vector<std::string>& assembly) {    
    auto right = tileExp(binOp->getRight(), assembly);
    move("ecx", right, assembly);
    auto left = tileExp(binOp->getLeft(), assembly);
    move("ebx", left, assembly);    
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
    return "ebx";
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

std::string Tiling::tileConst(const std::shared_ptr<TIR::Const>& node, std::vector<std::string>& assembly) {
    return std::to_string(node->getValue());
}

// mem(e)
std::string Tiling::tileMem(const std::shared_ptr<TIR::Mem>& node, std::vector<std::string>& assembly) {
    std::string addressExp = tileExp(node->getExpr(), assembly);
    move("ebx", addressExp, assembly);
    return "[ebx]";
}
    // auto t = regManager.getReg(localVarName, currentStackOffset, tempToStackOffset, assembly);
std::string Tiling::tileTemp(const std::shared_ptr<TIR::Temp>& node, std::vector<std::string>& assembly) {
    std::string localVarName{node->getName()};
    if (localVarName == TIR::Configuration::VTABLE) {
        return st->getClassName() + "_vtable"; 
    }

    if (localVarName == TIR::Configuration::ABSTRACT_RET && callFlag) {
        auto reg = regManager->getRegOrStackOffset(localVarName, assembly);
        move(reg, "eax", assembly);
        callFlag = false;
        return reg;
    }
    if (std::find(staticFields.begin(), staticFields.end(), localVarName) != staticFields.end())
        return "[" + localVarName + "]";
    else 
        return regManager->getRegOrStackOffset(localVarName, assembly);

    // std::string localVarName{node->getName()};
    // if (localVarName == TIR::Configuration::VTABLE) {
    //     return st->getClassName() + "_vtable"; 
    // }
    
    // if (!tempToStackOffset.contains(localVarName)) {
    //     currentStackOffset -= 4;
    //     tempToStackOffset[localVarName] = currentStackOffset;
    // }
    // auto offset = tempToStackOffset[localVarName];
    // auto offset_string = std::to_string(offset);
    // if (offset > 0)
    //     offset_string = std::string("+") + offset_string;

    // if (localVarName == TIR::Configuration::ABSTRACT_RET && callFlag) {
    //     assembly.push_back("mov [ebp" + offset_string  + "], eax");
    //     callFlag = false;
    //     return "[ebp" + offset_string  + "]";
    // }
    // if (std::find(staticFields.begin(), staticFields.end(), localVarName) != staticFields.end())
    //     return "[" + localVarName + "]";
    // else 
    //     return "[ebp" + offset_string  + "]";
}




std::string RegisterManager::getRegOrStackOffset(std::string var, std::vector<std::string>& assembly) {


    // return "[ebp" + offset(var)  + "]";      

    if (registerAlloc.contains(var) && !spills.contains(var))
    {
        auto reg = registerAlloc[var];
        regUsage[reg] = var;
        return reg;
    } else {
        return "[ebp" + offset(var)  + "]";   
    }


    // if (registerAlloc.contains(var) && !spilledVars.contains(var))
    // {
    //     auto& reg = registerAlloc[var];
    //     auto& currentVar = regUsage[reg];
    //     if (!currentVar.empty() && currentVar != var) {
    //         if (spills.contains(currentVar)) {
    //             spillToStack(currentVar, offset(currentVar), reg, assembly);                   
    //         }      
    //     }
    //     regUsage[reg] = var;  
    //     return reg;    
    // }
    // else {
    //     return "[ebp" + offset(var)  + "]";   
    // }
}           
