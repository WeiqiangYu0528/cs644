
#include "NodeFactory.hpp"
#include <vector>
#include <map>
#include <string>

class NodeFactory_c : public NodeFactory {
public:
    NodeFactory_c() = default;

    BinOp* IRBinOp(BinOp::OpType type, Expr* left, Expr* right) override;
    Call* IRCall(Expr* target, const std::vector<Expr*>& args) override;
    CJump* IRCJump(Expr* expr, const std::string& trueLabel) override;
    CJump* IRCJump(Expr* expr, const std::string& trueLabel, const std::string& falseLabel) override;
    CompUnit* IRCompUnit(const std::string& name) override;
    CompUnit* IRCompUnit(const std::string& name, const std::map<std::string, FuncDecl*>& functions) override;
    Const* IRConst(int value) override;
    ESeq* IRESeq(Stmt* stmt, Expr* expr) override;
    Exp* IRExp(Expr* expr) override;
    FuncDecl* IRFuncDecl(const std::string& name, int numParams, Stmt* stmt) override;
    Jump* IRJump(Expr* expr) override;
    Label* IRLabel(const std::string& name) override;
    Mem* IRMem(Expr* expr) override;
    Move* IRMove(Expr* target, Expr* expr) override;
    Name* IRName(const std::string& name) override;
    Return* IRReturn(Expr* ret) override;
    Seq* IRSeq(const std::vector<Stmt*>& stmts) override;
    Temp* IRTemp(const std::string& name) override;

    template<typename... Exprs>
    Call* IRCall(Expr* target, Exprs... args) {
        std::vector<Expr*> argsVec = {args...};
        return IRCall(target, argsVec);
    }

    template<typename... Stmts>
    Seq* IRSeq(Stmts... stmts) {
        std::vector<Stmt*> stmtsVec = {stmts...};
        return IRSeq(stmtsVec);
    }
};
