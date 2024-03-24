#pragma once
#include <vector>
#include <map>
#include <string>
#include "BinOp.hpp"
#include "Call.hpp"
#include "CJump.hpp"
#include "CompUnit.hpp"
#include "Const.hpp"
#include "ESeq.hpp"
#include "Exp.hpp"
#include "FuncDecl.hpp"
#include "Jump.hpp"
#include "Label.hpp"
#include "Mem.hpp"
#include "Move.hpp"
#include "Name.hpp"
#include "Return.hpp"
#include "Seq.hpp"
#include "Temp.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"

class NodeFactory {
public:
    virtual ~NodeFactory() {}

    virtual std::shared_ptr<BinOp> IRBinOp(BinOp::OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) = 0;
    virtual std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args) = 0;
    template<typename... Exprs>
    std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, Exprs... args);
    virtual std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel) = 0;
    virtual std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel, const std::string& falseLabel) = 0;
    virtual std::shared_ptr<CompUnit> IRCompUnit(const std::string& name) = 0;
    virtual std::shared_ptr<CompUnit> IRCompUnit(const std::string& name, const std::map<std::string, std::shared_ptr<FuncDecl>>& functions) = 0;
    virtual std::shared_ptr<Const> IRConst(int value) = 0;
    virtual std::shared_ptr<ESeq> IRESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) = 0;
    virtual std::shared_ptr<Exp> IRExp(std::shared_ptr<Expr> expr) = 0;
    virtual std::shared_ptr<FuncDecl> IRFuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> stmt) = 0;
    virtual std::shared_ptr<Jump> IRJump(std::shared_ptr<Expr> expr) = 0;
    virtual std::shared_ptr<Label> IRLabel(const std::string& name) = 0;
    virtual std::shared_ptr<Mem> IRMem(std::shared_ptr<Expr> expr) = 0;
    virtual std::shared_ptr<Move> IRMove(std::shared_ptr<Expr> target, std::shared_ptr<Expr> expr) = 0;
    virtual std::shared_ptr<Name> IRName(const std::string& name) = 0;
    virtual std::shared_ptr<Return> IRReturn(std::shared_ptr<std::shared_ptr<Expr> ret) = 0;
    virtual std::shared_ptr<Seq> IRSeq(const std::vector<std::shared_ptr<Stmt>>& stmts) = 0;
    template<typename... Stmts>
    std::shared_ptr<Seq> IRSeq(Stmts... stmts);
    virtual std::shared_ptr<Temp> IRTemp(const std::string& name) = 0;
};