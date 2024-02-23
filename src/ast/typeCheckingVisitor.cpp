#include "typeCheckingVisitor.hpp"

TypeCheckingVisitor::TypeCheckingVisitor(std::shared_ptr<Scope> s) : scope(s), error(false), initialized(false) {
    std::cout << scope->current->getClassOrInterfaceDecl()->id->name << std::endl;
}

void TypeCheckingVisitor::visit(std::shared_ptr<FormalParameter> n) {
    const std::string key {n->variableName->name};
    scope->current->putVar(key, n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n) {
    const std::string key {n->id->name};
    if (n->exp) n->exp->accept(this);
    scope->current->putVar(key, n);
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
    fieldDecls.insert(n->fieldName->name);
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
    if (initialized && !fieldDecls.contains(key)) {
        std::cerr << "Error: The initializer of a non-static field must not use (i.e., read) by simple name (i.e., without an explicit this) itself or a non-static field declared later in the same class" << std::endl;
        error = true;
    }
    else if (!initialized & n->simple && !scope->isNameValidInScope(key)) {
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
        std::cout << "primary" << std::endl;
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
            // Find the last index of '.'
            size_t lastIndex = (identifier).find_last_of(".");
            std::string qualifiedName {identifier.begin(), identifier.begin() + lastIndex};
            if (initialized && scope->current->getField(qualifiedName) != nullptr && !fieldDecls.contains(qualifiedName)) {
                std::cerr << "Error: The initializer of a non-static field must not use (i.e., read) by simple name (i.e., without an explicit this) itself or a non-static field declared later in the same class" << std::endl;
                error = true;
            }
            else if (!initialized && !scope->isNameValidInScope(qualifiedName)) {
                std::string methodName {identifier.begin() + lastIndex + 1, identifier.end()};
                if (auto m = scope->getNameInScope(qualifiedName, false)) {
                    auto methods = m->getMethod(methodName);
                    if (std::find_if(methods.begin(), methods.end(), [](auto method) {
                        return method->isStatic;
                        }) == methods.end()) {
                        std::cerr << "Error: Invalid static method name" << std::endl;
                        error = true;
                    }
                } else {
                    error = true;
                    std::cerr << "Error: Invalid method name" << std::endl;
                }
           }
        }
    }
}

bool TypeCheckingVisitor::isError() const {
    return error;
}
