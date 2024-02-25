#include "typeCheckingVisitor.hpp"

TypeCheckingVisitor::TypeCheckingVisitor(std::shared_ptr<Scope> s) : scope(s), error(false), initialized(false) {
    // for testing only
    std::cout << scope->current->getClassOrInterfaceDecl()->id->name << std::endl;
}

void TypeCheckingVisitor::visit(std::shared_ptr<FormalParameter> n) {
    const std::string key {n->variableName->name};
    scope->current->putVar(key, n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    const std::string key {n->id->name};
    scope->current->putVar(key, n);
    if (n->exp) n->exp->accept(this);
}

void TypeCheckingVisitor::visit(std::shared_ptr<Constructor> n) {
    scope->current->beginScope();
    Visitor::visit(n);
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<Method> n) {
    scope->current->beginScope();
    Visitor::visit(n);
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<Field> n) {
    if (n->initializer) {
        // initialized scope
        initialized = true;
        n->initializer->accept(this);
        initialized = false;
    }
    scope->current->setFieldDecl(n->fieldName->name);
}

void TypeCheckingVisitor::visit(std::shared_ptr<BlockStatement> n) {
    scope->current->beginScope();
    Visitor::visit(n);
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<IfStatement> n) {
    n->exp->accept(this);
    scope->current->beginScope();
    n->statement1->accept(this);
    scope->current->endScope();
    if (n->statement2) 
    {
        scope->current->beginScope();
        n->statement2->accept(this);
        scope->current->endScope();
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<WhileStatement> n) {
    scope->current->beginScope();
    Visitor::visit(n);
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<ForStatement> n) {
    scope->current->beginScope();
    Visitor::visit(n);
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<IdentifierExp> n) {
    const std::string key {n->id->name};
    if (scope->reclassifyAmbiguousName(key, n->simple, initialized).type != AmbiguousNamesType::EXPRESSION) {
        std::cerr << "Error: " << key << " is not a valid name in the current scope" << std::endl;
        error = true;
    }
    Visitor::visit(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    std::cout << "field access" << std::endl;
    Visitor::visit(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<MethodInvocation> n) {
      if (n->primary != nullptr) {
        n->primary->accept(this);
    }
    if (n->methodName != nullptr) {
        std::string identifier {n->methodName->id->name};
        if (n->methodName->simple) {
            if (scope->current->getMethod(identifier).empty()) {
                std::cerr << "Error: Invalid non-static method name " << identifier << std::endl;
                error = true;
            }
        } 
        else {
            if (scope->reclassifyAmbiguousName(identifier, false, initialized).type == AmbiguousNamesType::ERROR) {
                std::cerr << "Error: Invalid non-static method name " << identifier << std::endl;
                error = true;
            }
        }
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<Assignment> n) {
    // if (auto left = std::dynamic_pointer_cast<IdentifierExp>(n->left)) {
    //     const std::string key {left->id->name};
    // }
    // n->left->accept(this);
    // n->right->accept(this);
    Visitor::visit(n);
}

bool TypeCheckingVisitor::isError() const {
    return error;
}
