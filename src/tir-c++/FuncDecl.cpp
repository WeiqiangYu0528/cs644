#include "FuncDecl.hpp"

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
    std::shared_ptr<Stmt> stmt = std::dynamic_pointer_cast<Stmt>(v.visit(shared_from_this(), body));

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
