#include "NodeFactory_c.hpp"
#include <memory>

BinOp* NodeFactory_c::IRBinOp(OpType type, Expr* left, Expr* right) {
    return new BinOp(type, left, right);
}

Call* NodeFactory_c::IRCall(Expr* target, const std::vector<Expr*>& args) {
    return new Call(target, args);
}

CJump* NodeFactory_c::IRCJump(Expr* expr, const std::string& trueLabel) {
    return new CJump(expr, trueLabel);
}

CJump* NodeFactory_c::IRCJump(Expr* expr, const std::string& trueLabel, const std::string& falseLabel) {
    return new CJump(expr, trueLabel, falseLabel);
}

CompUnit* NodeFactory_c::IRCompUnit(const std::string& name) {
    return new CompUnit(name);
}

CompUnit* NodeFactory_c::IRCompUnit(const std::string& name, const std::map<std::string, FuncDecl*>& functions) {
    return new CompUnit(name, functions);
}

Const* NodeFactory_c::IRConst(int value) {
    return new Const(value);
}

ESeq* NodeFactory_c::IRESeq(Stmt* stmt, Expr* expr) {
    return new ESeq(stmt, expr);
}

Exp* NodeFactory_c::IRExp(Expr* expr) {
    return new Exp(expr);
}

FuncDecl* NodeFactory_c::IRFuncDecl(const std::string& name, int numParams, Stmt* stmt) {
    return new FuncDecl(name, numParams, stmt);
}

Jump* NodeFactory_c::IRJump(Expr* expr) {
    return new Jump(expr);
}

Label* NodeFactory_c::IRLabel(const std::string& name) {
    return new Label(name);
}

Mem* NodeFactory_c::IRMem(Expr* expr) {
    return new Mem(expr);
}

Move* NodeFactory_c::IRMove(Expr* target, Expr* expr) {
    return new Move(target, expr);
}

Name* NodeFactory_c::IRName(const std::string& name) {
    return new Name(name);
}

Return* NodeFactory_c::IRReturn(Expr* ret) {
    return new Return(ret);
}

Seq* NodeFactory_c::IRSeq(const std::vector<Stmt*>& stmts) {
    return new Seq(stmts);
}

Temp* NodeFactory_c::IRTemp(const std::string& name) {
    return new Temp(name);
}