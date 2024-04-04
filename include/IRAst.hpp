#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "IRVisitor.hpp"
#include "InsnMapsBuilder.hpp"
#include "CheckCanonicalIRVisitor.hpp"
#include "Configuration.hpp"

namespace TIR {

class Node : public std::enable_shared_from_this<Node> {
public:
    virtual ~Node() = default;

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) = 0;

    virtual std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) = 0;

    virtual std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) = 0;

    virtual bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) = 0;

    virtual std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) = 0;

    virtual bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const = 0;

    virtual std::string getLabel() const = 0;
};

class Node_c : public Node {
public:
    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override {
        return shared_from_this();
    }

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        return v->unit();
    }

    virtual std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override {
        return v;
    }

    virtual std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) override {
        v->addInsn(shared_from_this());
        return shared_from_this();
    }

    virtual std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) override {
        return v;
    }

    virtual bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override {
        return true;
    }

    virtual std::string getLabel() const override = 0;
};


class Expr : public Node_c {
public:
    virtual bool isConstant() const = 0;

    virtual int getConstant() const = 0;
};

class Stmt : public Node_c {
};

/**
 * An intermediate representation for expressions
 */
class Expr_c : public Expr {
public:
    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) override;

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;

    bool isConstant() const override;

    int getConstant() const override;
};

/**
 * Binary operators
 */
class BinOp : public Expr_c {
private:
    std::shared_ptr<Expr> left, right;

public:
   /**
     * An intermediate representation for a binary operation
     * OP(left, right)
     */
    enum class OpType {
        ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, LSHIFT, RSHIFT, ARSHIFT,
        EQ, NEQ, LT, LTU, GT, LEQ, GEQ
    };

    std::unordered_map<OpType, std::string> OpTypeToString = {
        {OpType::ADD, "ADD"},
        {OpType::SUB, "SUB"},
        {OpType::MUL, "MUL"},
        {OpType::DIV, "DIV"},
        {OpType::MOD, "MOD"},
        {OpType::AND, "AND"},
        {OpType::OR, "OR"},
        {OpType::XOR, "XOR"},
        {OpType::LSHIFT, "LSHIFT"},
        {OpType::RSHIFT, "RSHIFT"},
        {OpType::ARSHIFT, "ARSHIFT"},
        {OpType::EQ, "EQ"},
        {OpType::NEQ, "NEQ"},
        {OpType::LT, "LT"},
        {OpType::LTU, "LTU"},
        {OpType::GT, "GT"},
        {OpType::LEQ, "LEQ"},
        {OpType::GEQ, "GEQ"}
    };
    OpType type;

    BinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right);

    OpType getOpType() const;

    std::shared_ptr<Expr> getLeft() const;

    std::shared_ptr<Expr> getRight() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(left));
        result = v->bind(result, v->visit(right));
        return result;
    }

    bool isConstant() const override;
};

/**
 * An intermediate representation for a function call
 * CALL(e_target, e_1, ..., e_n)
 */
class Call : public Expr_c {
protected:
    std::shared_ptr<Expr> target;
    std::vector<std::shared_ptr<Expr>> args;

public:
    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    template<typename... Args>
    Call(std::shared_ptr<Expr> target, Args... args) : Call(target, std::vector<std::shared_ptr<Expr>>{args...}) {
    }
    /**
     *
     * @param target address of the code for this function call
     * @param numRets number of return values for this function call
     * @param args arguments of this function call
     */
    Call(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args);

    std::shared_ptr<Expr> getTarget() const;

    std::vector<std::shared_ptr<Expr>> getArgs() const;

    int getNumArgs() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(target));
        for (std::shared_ptr<Expr> arg : args)
            result = v->bind(result, v->visit(arg));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
};

/**
 * An intermediate representation for a function call
 * CALL(e_target, e_1, ..., e_n)
 */
class Call_s : public Stmt {
protected:
    std::shared_ptr<Temp> target;
    std::vector<std::shared_ptr<Temp>> args;

public:
    /**
     *
     * @param target address of the code for this function call
     * @param args arguments of this function call
     */
    template<typename... Args>
    Call_s(std::shared_ptr<Temp> target, Args... args) : Call_s(target, std::vector<std::shared_ptr<Temp>>{args...}) {
    }
    /**
     *
     * @param target address of the code for this function call
     * @param numRets number of return values for this function call
     * @param args arguments of this function call
     */
    Call_s(std::shared_ptr<Temp> target, const std::vector<std::shared_ptr<Temp>>& args);

    std::shared_ptr<Temp> getTarget() const;

    std::vector<std::shared_ptr<Temp>> getArgs() const;

    int getNumArgs() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(target));
        for (std::shared_ptr<Expr> arg : args)
            result = v->bind(result, v->visit(arg));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
    
};

/**
 * An intermediate representation for a conditional transfer of control
 * CJUMP(expr, trueLabel, falseLabel)
 */
class CJump : public Stmt {
private:
    std::shared_ptr<Expr> cond;
    std::string trueLabel, falseLabel;

public:
    /**
     * Construct a CJUMP instruction with fall-through on false.
     * @param cond the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     */
    CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel);
    /**
     *
     * @param cond the condition for the jump
     * @param trueLabel the destination of the jump if {@code expr} evaluates
     *          to true
     * @param falseLabel the destination of the jump if {@code expr} evaluates
     *          to false
     */
    CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel, const std::string& falseLabel);

    std::shared_ptr<Expr> getCond() const;

    std::string getTrueLabel() const;

    std::string getFalseLabel() const;

    bool hasFalseLabel() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(cond));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
};

/** An IR function declaration */
class FuncDecl : public Node_c {
public:
    std::string name;
    std::shared_ptr<Stmt> body;
    int numParams;

    FuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> body);

    std::string getName() const;

    std::shared_ptr<Stmt> getBody() const;

    int getNumParams() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(body));
        return result;
    }

    std::shared_ptr<Node> buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) override;
};

class Move : public Stmt {
private:
    std::shared_ptr<Expr> target;
    std::shared_ptr<Expr> src;

public:
    Move(std::shared_ptr<Expr> target, std::shared_ptr<Expr> src, bool trash = false);

    std::shared_ptr<Expr> getTarget() const;

    std::shared_ptr<Expr> getSource() const;

    virtual std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(target));
        result = v->bind(result, v->visit(src));
        return result;
    }

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};

/**
 * An intermediate representation for a compilation unit
 */
class CompUnit : public Node_c {
private:
    std::string name;
    std::shared_ptr<FuncDecl> staticInitFunc;
    std::vector<std::shared_ptr<Move>> fields;
    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> functions;

public:
    CompUnit(const std::string& name);

    CompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions);

    void appendFunc(std::shared_ptr<FuncDecl> func);

    void setStaticInitFunc(std::shared_ptr<FuncDecl> func);

    void setFields(std::vector<std::shared_ptr<Move>>& fields);

    void setFunctions(std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions);

    std::string getName() const;

    const std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& getFunctions() const;

    const std::vector<std::shared_ptr<Move>>& getFields() const;

    std::shared_ptr<FuncDecl> getFunction(const std::string& name) const;

    std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        for (auto [key, func] : functions) {
            result = v->bind(result, v->visit(func));
        }
        return result;
    }

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};

/**
 * An intermediate representation for a 64-bit integer constant.
 * CONST(n)
 */
class Const : public Expr_c {
private:
    int value;

public:
    /**
     *
     * @param value value of this constant
     */
    Const(int value);

    int getValue() const;

    std::string getLabel() const override;

    bool isConstant() const override;

    int getConstant() const override;
};



/**
 * An intermediate representation for an expression evaluated under side effects
 * ESEQ(stmt, expr)
 */
class ESeq : public Expr_c {
private:
    std::shared_ptr<Stmt> stmt;
    std::shared_ptr<Expr> expr;

public:
    /**
     * @param stmt IR statement to be evaluated for side effects
     * @param expr IR expression to be evaluated after {@code stmt}
     */
    ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr);

    ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr, bool replaceParent);

    std::shared_ptr<Stmt> getStmt() const;

    std::shared_ptr<Expr> getExpr() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(stmt));
        result = v->bind(result, v->visit(expr));
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
};

/**
 * An intermediate representation for evaluating an expression for side effects,
 * discarding the result
 * EXP(e)
 */
class Exp : public Stmt {
private:
    std::shared_ptr<Expr> expr;

public:
    /**
     *
     * @param expr the expression to be evaluated and result discarded
     */
    Exp(std::shared_ptr<Expr> expr);

    std::shared_ptr<Expr> getExpr() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(expr));
        return result;
    }

    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) override;
};

/**
 * An intermediate representation for a transfer of control
 */
class Jump : public Stmt {
private:
    std::shared_ptr<Expr> target;

public:
    /**
     *
     * @param expr the destination of the jump
     */
    Jump(std::shared_ptr<Expr> expr);

    std::shared_ptr<Expr> getTarget() const;

    std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        result = v->bind(result, v->visit(target));
        return result;
    }

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};

/**
 * An intermediate representation for naming a memory address
 */
class Label : public Stmt {
private:
    std::string name;

public:
    /**
     *
     * @param name name of this memory address
     */
    Label(const std::string& name);

    std::string getName() const;

    std::string getLabel() const override;

    std::shared_ptr<InsnMapsBuilder> buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) override;
};

class Mem : public Expr_c {
private:
    std::shared_ptr<Expr> expr;

public:
    explicit Mem(std::shared_ptr<Expr> expr);

    std::shared_ptr<Expr> getExpr() const;

    virtual std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) override {
        bool result = v->unit();
        result = v->bind(result, v->visit(expr));
        return result;
    }

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};

class Name : public Expr_c {
private:
    std::string name;

public:
    Name(const std::string& name);

    std::string getName() const;

    virtual std::string getLabel() const override;
};

class Seq : public Stmt {
private:
    std::vector<std::shared_ptr<Stmt>> stmts;
    bool replaceParent;
    std::vector<std::shared_ptr<Stmt>> filterNulls(const std::vector<std::shared_ptr<Stmt>>& list);

public:
    Seq(const std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent = false);

    const std::vector<std::shared_ptr<Stmt>>& getStmts() const;

    std::string getLabel() const override;

    std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;

    std::shared_ptr<CheckCanonicalIRVisitor> checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        for (auto stmt : stmts) {
            result = v->bind(result, v->visit(stmt));
        }
        return result;
    }

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
};

class Temp : public Expr_c {
private:
    std::string name;

public:
    Temp(const std::string& name);

    std::string getName() const;

    virtual std::string getLabel() const override;
};

class Return : public Stmt {
protected:
    std::shared_ptr<Expr> ret;

public:
    explicit Return(std::shared_ptr<Expr> ret);

    std::shared_ptr<Expr> getRet() const;

    virtual std::string getLabel() const override;

    bool aggregateChildren(std::shared_ptr<AggregateVisitor> v) {
        bool result = v->unit();
        result = v->bind(result, v->visit(ret));
        return result;
    }

    virtual std::shared_ptr<Node> visitChildren(std::shared_ptr<IRVisitor> v) override;
};

/**
 * An intermediate representation for a function call in statement form
 * CALL(e_target, e_1, ..., e_n)
 */
class Call_s : public Stmt {
protected:
    std::shared_ptr<Temp> target;
    std::vector<std::shared_ptr<Temp>> args;

public:
    /**
     *
     * @param target address of the code for this function call
     * @param numRets number of return values for this function call
     * @param args arguments of this function call
     */
    Call_s(std::shared_ptr<Temp> target, const std::vector<std::shared_ptr<Temp>>& args);

    std::shared_ptr<Temp> getTarget() const;

    std::vector<std::shared_ptr<Temp>> getArgs() const;

    int getNumArgs() const;

    std::string getLabel() const override;

    bool isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const override;
    
};


class NodeFactory {
public:
    virtual std::shared_ptr<BinOp> IRBinOp(BinOp::OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) = 0;

    virtual std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args) = 0;

    template<typename... Exprs>
    std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, Exprs... args) {
        std::vector<std::shared_ptr<Expr>> argsVec = {args...};
        return IRCall(target, argsVec);
    }

    virtual std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel) = 0;

    virtual std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel, const std::string& falseLabel) = 0;

    virtual std::shared_ptr<CompUnit> IRCompUnit(const std::string& name) = 0;

    virtual std::shared_ptr<CompUnit> IRCompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions) = 0;

    virtual std::shared_ptr<Const> IRConst(int value) = 0;

    virtual std::shared_ptr<ESeq> IRESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) = 0;

    virtual std::shared_ptr<Exp> IRExp(std::shared_ptr<Expr> expr) = 0;

    virtual std::shared_ptr<FuncDecl> IRFuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> stmt) = 0;

    virtual std::shared_ptr<Jump> IRJump(std::shared_ptr<Expr> expr) = 0;

    virtual std::shared_ptr<Label> IRLabel(const std::string& name) = 0;

    virtual std::shared_ptr<Mem> IRMem(std::shared_ptr<Expr> expr) = 0;

    virtual std::shared_ptr<Move> IRMove(std::shared_ptr<Expr> target, std::shared_ptr<Expr> expr) = 0;

    virtual std::shared_ptr<Name> IRName(const std::string& name) = 0;

    virtual std::shared_ptr<Return> IRReturn(std::shared_ptr<Expr> ret) = 0;

    virtual std::shared_ptr<Seq> IRSeq(std::vector<std::shared_ptr<Stmt>>& stmts) = 0;

    template<typename... Stmts>
    std::shared_ptr<Seq> IRSeq(Stmts... stmts) {
        std::vector<std::shared_ptr<Stmt>> stmtsVec = {stmts...};
        return IRSeq(stmtsVec);
    }

    virtual std::shared_ptr<Temp> IRTemp(const std::string& name) = 0;
};

class NodeFactory_c : public NodeFactory {
public:
    std::shared_ptr<BinOp> IRBinOp(BinOp::OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) override;

    std::shared_ptr<Call> IRCall(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args) override;

    std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel) override;

    std::shared_ptr<CJump> IRCJump(std::shared_ptr<Expr> expr, const std::string& trueLabel, const std::string& falseLabel) override;

    std::shared_ptr<CompUnit> IRCompUnit(const std::string& name) override;

    std::shared_ptr<CompUnit> IRCompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions) override;

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

    std::shared_ptr<Seq> IRSeq(std::vector<std::shared_ptr<Stmt>>& stmts) override;

    std::shared_ptr<Temp> IRTemp(const std::string& name) override;
};

};
