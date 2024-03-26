#include "NodeFactory_c.hpp"

std::shared_ptr<BinOp> NodeFactory_c::IRBinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) {
    return std::make_shared<BinOp>(type, left, right);
}

std::shared_ptr<Call> NodeFactory_c::IRCall(std::shared_ptr<Expr> target, std::vector<std::shared_ptr<Expr>>& args) {
    return std::make_shared<Call>(target, args);
}

std::shared_ptr<CJump> NodeFactory_c::IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel) {
    return std::make_shared<CJump>(expr, trueLabel);
}

std::shared_ptr<CJump> NodeFactory_c::IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel, const std::string& falseLabel) {
    return std::make_shared<CJump>(expr, trueLabel, falseLabel);
}

std::shared_ptr<CompUnit> NodeFactory_c::IRCompUnit(const std::string& name) {
    return std::make_shared<CompUnit>(name);
}

std::shared_ptr<CompUnit> NodeFactory_c::IRCompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions) {
    return std::make_shared<CompUnit>(name, functions);
}

std::shared_ptr<Const> NodeFactory_c::IRConst(int value) {
    return std::make_shared<Const>(value);
}

std::shared_ptr<ESeq> NodeFactory_c::IRESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) {
    return std::make_shared<ESeq>(stmt, expr);
}

std::shared_ptr<Exp> NodeFactory_c::IRExp(std::shared_ptr<Expr> expr) {
    return std::make_shared<Exp>(expr);
}

std::shared_ptr<FuncDecl> NodeFactory_c::IRFuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> stmt) {
    return std::make_shared<FuncDecl>(name, numParams, stmt);
}

std::shared_ptr<Jump> NodeFactory_c::IRJump(std::shared_ptr<Expr> expr) {
    return std::make_shared<Jump>(expr);
}

std::shared_ptr<Label> NodeFactory_c::IRLabel(const std::string& name) {
    return std::make_shared<Label>(name);
}

std::shared_ptr<Mem> NodeFactory_c::IRMem(std::shared_ptr<Expr> expr) {
    return std::make_shared<Mem>(expr);
}

std::shared_ptr<Move> NodeFactory_c::IRMove(std::shared_ptr<Expr> target, std::shared_ptr<Expr> expr) {
    return std::make_shared<Move>(target, expr);
}

std::shared_ptr<Name> NodeFactory_c::IRName(const std::string& name) {
    return std::make_shared<Name>(name);
}

std::shared_ptr<Return> NodeFactory_c::IRReturn(std::shared_ptr<Expr> ret) {
    return std::make_shared<Return>(ret);
}

std::shared_ptr<Seq> NodeFactory_c::IRSeq(std::vector<std::shared_ptr<Stmt>>& stmts) {
    return std::make_shared<Seq>(stmts);
}

std::shared_ptr<Temp> NodeFactory_c::IRTemp(const std::string& name) {
    return std::make_shared<Temp>(name);
}
