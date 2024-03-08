#include "contextVisitor.hpp"

ContextVisitor::ContextVisitor() : symbolTable(std::make_shared<SymbolTable>()), error(false) {
}

void ContextVisitor::visit(std::shared_ptr<FormalParameter> n) {
    const std::string key {n->variableName->name};
    if (symbolTable->getVar(key)) {
        std::cerr << "Error: Local variable " << key << " redeclared" << std::endl;
        error = true;
    } else {
        symbolTable->putVar(key, n);
    }
    Visitor::visit(n);
}

void ContextVisitor::visit(std::shared_ptr<Constructor> n) {
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
    symbolTable->putConstuctor(n);
}

void ContextVisitor::visit(std::shared_ptr<Method> n) {
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
    symbolTable->putMethod(n->methodName->name, n);
}

void ContextVisitor::visit(std::shared_ptr<Field> n) {
    const std::string key {n->fieldName->name};
    if (symbolTable->getField(key)) {
        std::cerr << "Error: Field " << key << " already declared" << std::endl;
        error = true;
    } else {
        symbolTable->putField(key, n);
    }
    Visitor::visit(n);
}

void ContextVisitor::visit(std::shared_ptr<BlockStatement> n) {
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
}

void ContextVisitor::visit(std::shared_ptr<IfStatement> n) {
    n->exp->accept(this);
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    n->statement1->accept(this);
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    if (n->statement2) 
    {
        n->statement2->accept(this);
    }
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
}

void ContextVisitor::visit(std::shared_ptr<WhileStatement> n) {
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
}

void ContextVisitor::visit(std::shared_ptr<ForStatement> n) {
    symbolTable->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    symbolTable->endScope(ScopeType::LOCALVARIABLE);
}

void ContextVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    const std::string key {n->id->name};
    if (symbolTable->getVar(key)) {
        std::cerr << "Error: Local variable " << key << " redeclared" << std::endl;
        error = true;
    } else {
        symbolTable->putVar(key, n);
    }
    Visitor::visit(n);
}

void ContextVisitor::visit(std::shared_ptr<Program> n) {
    symbolTable->setAst(n);
    std::string pkg = n->package->id ? n->package->id->name : "";
    symbolTable->setPackage(pkg);
    symbolTable->setClassOrInterfaceDecl(n->classOrInterfaceDecl);
    Visitor::visit(n);
}

bool ContextVisitor::isError() const {
    return error;
}

std::shared_ptr<SymbolTable> ContextVisitor::getSymbolTable() const {
    return symbolTable;
}
