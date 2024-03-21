#include "FuncDecl.hpp"

FuncDecl::FuncDecl(const std::string& name, int numParams, std::shared_ptr<Stmt> body) : name(name), body(body), numParams(numParams) {
}

std::string FuncDecl::name() const {
    return name;
}

std::shared_ptr<Stmt> FuncDecl::body() const {
    return body;
}

int FuncDecl::getNumParams() const {
    return numParams;
}

std::string FuncDecl::label() const {
    return "FUNC " + name;
}

std::shared_ptr<Node> FuncDecl::visitChildren(std::shared_ptr<IRVisitor> v) {
    std::shared_ptr<Stmt> stmt = (Stmt) v.visit(this, body);

    if (stmt != body) return v->nodeFactory().IRFuncDecl(
            name, numParams, stmt
    );

    return this;
}

std::shared_ptr<InsnMapsBuilder> FuncDecl::buildInsnMapsEnter(std::shared_ptr<InsnMapsBuilder> v) {
    v->addNameToCurrentIndex(name);
    v->addInsn(this);
    return v;
}

std::shared_ptr<Node> FuncDecl::buildInsnMaps(std::shared_ptr<InsnMapsBuilder> v) {
    return this;
}
