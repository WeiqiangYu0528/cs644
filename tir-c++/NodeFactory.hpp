#ifndef NODEFACTORY_HPP
#define NODEFACTORY_HPP

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

    virtual BinOp* IRBinOp(BinOp::OpType type, Expr* left, Expr* right) = 0;
    virtual Call* IRCall(Expr* target, const std::vector<Expr*>& args) = 0;
    template<typename... Exprs>
    Call* IRCall(Expr* target, Exprs... args);
    virtual CJump* IRCJump(Expr* expr, const std::string& trueLabel) = 0;
    virtual CJump* IRCJump(Expr* expr, const std::string& trueLabel, const std::string& falseLabel) = 0;
    virtual CompUnit* IRCompUnit(const std::string& name) = 0;
    virtual CompUnit* IRCompUnit(const std::string& name, const std::map<std::string, FuncDecl*>& functions) = 0;
    virtual Const* IRConst(int value) = 0;
    virtual ESeq* IRESeq(Stmt* stmt, Expr* expr) = 0;
    virtual Exp* IRExp(Expr* expr) = 0;
    virtual FuncDecl* IRFuncDecl(const std::string& name, int numParams, Stmt* stmt) = 0;
    virtual Jump* IRJump(Expr* expr) = 0;
    virtual Label* IRLabel(const std::string& name) = 0;
    virtual Mem* IRMem(Expr* expr) = 0;
    virtual Move* IRMove(Expr* target, Expr* expr) = 0;
    virtual Name* IRName(const std::string& name) = 0;
    virtual Return* IRReturn(Expr* ret) = 0;
    virtual Seq* IRSeq(const std::vector<Stmt*>& stmts) = 0;
    template<typename... Stmts>
    Seq* IRSeq(Stmts... stmts);
    virtual Temp* IRTemp(const std::string& name) = 0;
};

#endif // NODEFACTORY_HPP
