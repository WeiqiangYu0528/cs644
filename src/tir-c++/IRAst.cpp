#include "IRAst.hpp"

using namespace TIR;

BinOp::BinOp(OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) : left(left), right(right), type(type) {
}

BinOp::OpType BinOp::getOpType() const {
    return type;
}

std::shared_ptr<Expr> BinOp::getLeft() const {
    return left;
}

std::shared_ptr<Expr> BinOp::getRight() const {
    return right;
}

std::string BinOp::getLabel() const {
    return OpTypeToString.at(type);
}

std::shared_ptr<Node> BinOp::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> l = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), left));
    std::shared_ptr<Expr> r = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), right));
    
    if (l != left || r != right)
        return v->nodeFactory()->IRBinOp(type, l, r);

    return shared_from_this();
}

bool BinOp::isConstant() const {
    return left->isConstant() && right->isConstant();
}

Call::Call(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args) : target(target), args(args) {
}

std::shared_ptr<Expr> Call::getTarget() const {
    return target;
}

std::vector<std::shared_ptr<Expr>> Call::getArgs() const {
    return args;
}

int Call::getNumArgs() const {
    return args.size();
}

std::string Call::getLabel() const {
    return "CALL";
}

std::shared_ptr<Node> Call::visitChildren(std::shared_ptr<IRVisitor> v) {

    bool modified = false;

    std::shared_ptr<Expr> t = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));
    if (t != target) modified = true;

    std::vector<std::shared_ptr<Expr>> results(args.size());
    for (std::shared_ptr<Expr> arg : args) {
        std::shared_ptr<Expr> newExpr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), arg));
        if (newExpr != arg) modified = true;
        results.push_back(newExpr);
    }

    if (modified) return v->nodeFactory()->IRCall(t, results);

    return shared_from_this();
}

bool Call::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !v->inExpr();
}

Call_s::Call_s(std::shared_ptr<Name> target, const std::vector<std::shared_ptr<Temp>>& args) : target(target), args(args) {
}

std::shared_ptr<Name> Call_s::getTarget() const {
    return target;
}

std::vector<std::shared_ptr<Temp>> Call_s::getArgs() const {
    return args;
}

int Call_s::getNumArgs() const {
    return args.size();
}

std::string Call_s::getLabel() const {
    return "CALL_s";
}

std::shared_ptr<Node> Call_s::visitChildren(std::shared_ptr<IRVisitor> v) {

    bool modified = false;

    std::shared_ptr<Expr> t = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));
    if (t != target) modified = true;

    std::vector<std::shared_ptr<Expr>> results(args.size());
    for (std::shared_ptr<Expr> arg : args) {
        std::shared_ptr<Expr> newExpr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), arg));
        if (newExpr != arg) modified = true;
        results.push_back(newExpr);
    }

    if (modified) return v->nodeFactory()->IRCall(t, results);

    return shared_from_this();
}

bool Call_s::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !v->inExpr();
}

CJump::CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel) : CJump(cond, trueLabel, nullptr) {
}

CJump::CJump(std::shared_ptr<Expr> cond, const std::string& trueLabel, const std::string& falseLabel) : cond(cond), trueLabel(trueLabel), falseLabel(falseLabel) {
}

std::shared_ptr<Expr> CJump::getCond() const {
    return cond;
}

std::string CJump::getTrueLabel() const {
    return trueLabel;
}

std::string CJump::getFalseLabel() const {
    return falseLabel;
}

bool CJump::hasFalseLabel() const {
    return !falseLabel.empty();
}

std::string CJump::getLabel() const {
    return "CJUMP";
}

std::shared_ptr<Node> CJump::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), cond));

    if (expr != cond)
        return v->nodeFactory()->IRCJump(expr, trueLabel, falseLabel);

    return shared_from_this();
}

bool CJump::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !hasFalseLabel();
}

CompUnit::CompUnit(const std::string& name) : name(name) {
}

CompUnit::CompUnit(const std::string& name, std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions) : name(name), functions(functions) {
}

void CompUnit::appendFunc(std::shared_ptr<FuncDecl> func) {
    functions[func->name] = func;
}

void CompUnit::setStaticInitFunc(std::shared_ptr<FuncDecl> func) {
    this->staticInitFunc = func;
}

void CompUnit::setFields(std::vector<std::shared_ptr<Move>>& fields) {
    this->fields = fields;
}

void CompUnit::setFunctions(std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& functions) {
    this->functions = functions;
}

std::string CompUnit::getName() const {
    return name;
}

const std::unordered_map<std::string, std::shared_ptr<FuncDecl>>& CompUnit::getFunctions() const {
    return functions;
}

const std::vector<std::shared_ptr<Move>>& CompUnit::getFields() const {
    return fields;
}

std::shared_ptr<FuncDecl> CompUnit::getFunction(const std::string& name) const {
    return name == Configuration::STATIC_INIT_FUNC ? staticInitFunc : functions.at(name);
}

std::string CompUnit::getLabel() const {
    return "COMPUNIT";
}

std::shared_ptr<Node> CompUnit::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;

    std::unordered_map<std::string, std::shared_ptr<FuncDecl>> results;
    v->visit(shared_from_this(), staticInitFunc);
    for (auto [key, func] : functions) {
        std::shared_ptr<FuncDecl> newFunc = std::dynamic_pointer_cast<FuncDecl>(v->visit(shared_from_this(), func));
        if (newFunc != func) modified = true;
        results[newFunc->name] = newFunc;
    }

    if (modified) return v->nodeFactory()->IRCompUnit(name, results);

    return shared_from_this();
}

Const::Const(int value) : value(value) {
}

int Const::getValue() const {
    return value;
}

std::string Const::getLabel() const {
    return "CONST(" + std::to_string(value) + ")";
}

bool Const::isConstant() const {
    return true;
}

int Const::getConstant() const {
    return value;
}

ESeq::ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr) : stmt(stmt), expr(expr) {
}

ESeq::ESeq(std::shared_ptr<Stmt> stmt, std::shared_ptr<Expr> expr, bool replaceParent) : stmt(stmt), expr(expr) {
}

std::shared_ptr<Stmt> ESeq::getStmt() const {
    return stmt;
}

std::shared_ptr<Expr> ESeq::getExpr() const {
    return expr;
}

std::string ESeq::getLabel() const {
    return "ESEQ";
}

std::shared_ptr<Node> ESeq::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Stmt> s = std::dynamic_pointer_cast<Stmt>(v->visit(shared_from_this(), stmt));
    std::shared_ptr<Expr> e = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), expr));

    if (e != expr || s != stmt)
        return v->nodeFactory()->IRESeq(s, e);

    return shared_from_this();
}

bool ESeq::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return false;
}

Exp::Exp(std::shared_ptr<Expr> expr) : expr(expr) {
}

std::shared_ptr<Expr> Exp::getExpr() const {
    return expr;
}

std::string Exp::getLabel() const {
    return "EXP";
}

std::shared_ptr<Node> Exp::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> e = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), expr));

    if (e != expr) return v->nodeFactory()->IRExp(e);

    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> Exp::checkCanonicalEnter(
        std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterExp();
}

Jump::Jump(std::shared_ptr<Expr> expr) : target(expr) {
}

std::shared_ptr<Expr> Jump::getTarget() const {
    return target;
}

std::string Jump::getLabel() const {
    return "JUMP";
}

std::shared_ptr<Node> Jump::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> expr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));

    if (expr != target) return v->nodeFactory()->IRJump(expr);

    return shared_from_this();
}

Label::Label(const std::string& name) : name(name) {
}

std::string Label::getName() const {
    return name;
}

std::string Label::getLabel() const {
    return "LABEL(" + name + ")";
}

std::shared_ptr<InsnMapsBuilder> Label::buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) {
    v->addNameToCurrentIndex(name);
    return v;
}

Mem::Mem(std::shared_ptr<Expr> expr) : expr(expr) {}

std::shared_ptr<Expr> Mem::getExpr() const {
    return expr;
}

std::string Mem::getLabel() const {
    return "MEM";
}

std::shared_ptr<Node> Mem::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> newExpr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), expr));
    if (newExpr != expr) {
        return v->nodeFactory()->IRMem(newExpr);
    }
    return shared_from_this();
}

Move::Move(std::shared_ptr<Expr> target, std::shared_ptr<Expr> src, bool trash) : target(target), src(src) {}

std::shared_ptr<Expr> Move::getTarget() const {
    return target;
}

std::shared_ptr<Expr> Move::getSource() const {
    return src;
}

std::string Move::getLabel() const {
    return "MOVE";
}

std::shared_ptr<Node> Move::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Expr> newTarget = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), target));
    std::shared_ptr<Expr> newSrc = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), src));

    if (newTarget != target || newSrc != src)
        return v->nodeFactory()->IRMove(newTarget, newSrc);

    return shared_from_this();
}

Name::Name(const std::string& name) : name(name) {}

std::string Name::getName() const {
    return name;
}

std::string Name::getLabel() const {
    return "NAME(" + name + ")";
}

Temp::Temp(const std::string& name) : name(name) {}

std::string Temp::getName() const {
    return name;
}

std::string Temp::getLabel() const {
    return "TEMP(" + name + ")";
}

Return::Return(std::shared_ptr<Expr> ret) : ret(ret) {}

std::shared_ptr<Expr> Return::getRet() const {
    return ret;
}

std::string Return::getLabel() const {
    return "RETURN";
}

std::shared_ptr<Node> Return::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;
    std::shared_ptr<Expr> newExpr = std::dynamic_pointer_cast<Expr>(v->visit(shared_from_this(), ret));

    if (newExpr != ret) modified = true;
    std::shared_ptr<Expr> result = newExpr;

    if (modified) {
        return v->nodeFactory()->IRReturn(result);
    }

    return shared_from_this();
}

FuncDecl::FuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> body) : name(name), body(body), numParams(numParams) {
}

std::string FuncDecl::getName() const {
    return name;
}

std::shared_ptr<Stmt> FuncDecl::getBody() const {
    return body;
}

int FuncDecl::getNumParams() const {
    return numParams;
}

std::string FuncDecl::getLabel() const {
    return "FUNC " + name;
}

std::shared_ptr<Node> FuncDecl::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Stmt> stmt = std::dynamic_pointer_cast<Stmt>(v->visit(shared_from_this(), body));

    if (stmt != body) return v->nodeFactory()->IRFuncDecl(
            name, numParams, stmt
    );

    return shared_from_this();
}

std::shared_ptr<InsnMapsBuilder> FuncDecl::buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) {
    v->addNameToCurrentIndex(name);
    v->addInsn(shared_from_this());
    return v;
}

std::shared_ptr<Node> FuncDecl::buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) {
    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> Expr_c::checkCanonicalEnter(
            std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterExpr();
}

bool Expr_c::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return v->inExpr() || !v->inExp();
}

bool Expr_c::isConstant() const {
    return false;
}

int Expr_c::getConstant() const {
    throw std::runtime_error("Unsupported Operation Exception");
}

std::vector<std::shared_ptr<Stmt>> Seq::filterNulls(const std::vector<std::shared_ptr<Stmt>>& list) {
    std::vector<std::shared_ptr<Stmt>> filtered;
    std::copy_if(list.begin(), list.end(), std::back_inserter(filtered),
                [](const std::shared_ptr<Stmt> item) { return item != nullptr; });
    return filtered;
}

Seq::Seq(const std::vector<std::shared_ptr<Stmt>>& stmts, bool replaceParent)
    : stmts(filterNulls(stmts)), replaceParent(replaceParent) {}

const std::vector<std::shared_ptr<Stmt>>& Seq::getStmts() const {
    return stmts;
}

std::string Seq::getLabel() const {
    return "SEQ";
}

std::shared_ptr<Node> Seq::visitChildren(std::shared_ptr<IRVisitor> v) {
    bool modified = false;
    std::vector<std::shared_ptr<Stmt>> results;

    for (auto stmt : stmts) {
        auto newStmt = std::dynamic_pointer_cast<Stmt>(v->visit(shared_from_this(), stmt));
        if (newStmt != stmt) modified = true;
        results.emplace_back(newStmt);
    }

    if (modified) return v->nodeFactory()->IRSeq(results);

    return shared_from_this();
}

std::shared_ptr<CheckCanonicalIRVisitor> Seq::checkCanonicalEnter(std::shared_ptr<CheckCanonicalIRVisitor> v) {
    return v->enterSeq();
}

bool Seq::isCanonical(std::shared_ptr<CheckCanonicalIRVisitor> v) const {
    return !v->inSeq();
}

std::shared_ptr<BinOp> NodeFactory_c::IRBinOp(BinOp::OpType type, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right) {
    return std::make_shared<BinOp>(type, left, right);
}

std::shared_ptr<Call> NodeFactory_c::IRCall(std::shared_ptr<Expr> target, const std::vector<std::shared_ptr<Expr>>& args) {
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
