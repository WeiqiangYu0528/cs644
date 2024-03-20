#include "FunctionDecl.hpp"

FuncDecl::FuncDecl(const std::string& name, int numParams, Stmt& body) : name(name), body(body), numParams(numParams) {
}

std::string FuncDecl::name() const {
    return name;
}

Stmt FuncDecl::body() const {
    return body;
}

int FuncDecl::getNumParams() const {
    return numParams;
}

std::string FuncDecl::label() const {
    return "FUNC " + name;
}

Node FuncDecl::visitChildren(IRVisitor& v) {
    Stmt stmt = (Stmt) v.visit(this, body);

    if (stmt != body) return v.nodeFactory().IRFuncDecl(
            name, numParams, stmt
    );

    return this;
}

InsnMapsBuilder FuncDecl::buildInsnMapsEnter(InsnMapsBuilder& v) {
    v.addNameToCurrentIndex(name);
    v.addInsn(this);
    return v;
}

Node FuncDecl::buildInsnMaps(InsnMapsBuilder& v) {
    return this;
}
