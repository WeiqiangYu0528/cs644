#pragma once
#include "NodeFactory.hpp"
#include <vector>
#include <map>
#include <string>

class NodeFactory_c : public NodeFactory {
public:
    std::shared_ptr<BinOp> IRBinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) override;
    std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args) override;
    std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel) override;
    std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel, const std::string& falseLabel) override;
    std::shared_ptr<CompUnit> IRCompUnit(const std::string& name) override;
    std::shared_ptr<CompUnit> IRCompUnit(const std::string& name, const std::map<std::string, std::shared_ptr<FuncDecl>>& functions) override;
    std::shared_ptr<Const> IRConst(int value) override;
    std::shared_ptr<ESeq> IRESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) override;
    std::shared_ptr<Exp> IRExp(std::shared_ptr<Expr> expr) override;
    std::shared_ptr<FuncDecl> IRFuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> stmt) override;
    std::shared_ptr<Jump> IRJump(std::shared_ptr<Expr> expr) override;
    std::shared_ptr<Label> IRLabel(const std::string& name) override;
    std::shared_ptr<Mem> IRMem(std::shared_ptr<Expr> expr) override;
    std::shared_ptr<Move> IRMove(std::shared_ptr<Expr> target, std::shared_ptr<Expr> expr) override;
    std::shared_ptr<Name> IRName(const std::string& name) override;
    std::shared_ptr<Return> IRReturn(std::shared_ptr<Expr> ret) override;
    std::shared_ptr<Seq> IRSeq(const std::vector<std::shared_ptr<Stmt>>& stmts) override;
    std::shared_ptr<Temp> IRTemp(const std::string& name) override;

    template<typename... Exprs>
    std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, Exprs... args) {
        std::vector<std::shared_ptr<Expr>> argsVec = {args...};
        return IRCall(target, argsVec);
    }

    template<typename... Stmts>
    std::shared_ptr<Seq> IRSeq(Stmts... stmts) {
        std::vector<std::shared_ptr<Stmt>> stmtsVec = {stmts...};
        return IRSeq(stmtsVec);
    }
};
