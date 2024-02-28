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
    auto ambiguousName = scope->reclassifyAmbiguousName(key, n->simple, initialized);
    if (ambiguousName.type != AmbiguousNamesType::EXPRESSION) {
        std::cerr << "Error: " << key << " is not a valid name in the current scope" << std::endl;
        error = true;
    }
    Visitor::visit(n);

    switch (ambiguousName.typeNode->type)
    {
    case DataType::INT:
        currentExpType = ExpType::Integer;
        break;        
    case DataType::CHAR:
        currentExpType = ExpType::Char;
        break;  
    case DataType::SHORT:
        currentExpType = ExpType::Short;
        break; 
    case DataType::BOOLEAN:
        currentExpType = ExpType::Boolean;
        break;           
    case DataType::OBJECT:
        currentExpType = ExpType::Object;    
        ambiguousName.typeNode->accept(this); 
        break;        
    default:
        currentExpType = ExpType::Undefined;
        break;
    }
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
    std::string left_obj_name {""}, right_obj_name {""};
    
    auto left_type = GetExpType(n->left);
    if (left_type == ExpType::Object)
        left_obj_name = currentObjectTypeName;
    auto right_type = GetExpType(n->right);
    if (right_type == ExpType::Object)
        right_obj_name = currentObjectTypeName;
    
    if(left_type == ExpType::Object && right_type == ExpType::Object) {
        if (left_obj_name != right_obj_name) {
            error = true;
            std::cerr << "Error: Invalid Assignment Type" << std::endl;
        }
    }
    

    std::cout << "debug: Assignment: " << (int)left_type << " " << (int)right_type << std::endl;
    std::cout << "debug: Assignment: " << left_obj_name << " = " << right_obj_name << std::endl;
}

bool TypeCheckingVisitor::isError() const {
    return error;
}

void TypeCheckingVisitor::visit(std::shared_ptr<IdentifierType> n) {
    currentObjectTypeName = n->id->name;
}

void TypeCheckingVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) {
    currentExpType = ExpType::Integer;
}

void TypeCheckingVisitor::visit(std::shared_ptr<CharLiteralExp> n) {
    currentExpType = ExpType::Char;
}

void TypeCheckingVisitor::visit(std::shared_ptr<PlusExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Arithmetic, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid PlusExp Type " << std::endl;
        error = true;
    }    
}

void TypeCheckingVisitor::visit(std::shared_ptr<MinusExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Arithmetic, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid MinusExp Type " << std::endl;
        error = true;
    }    
}

void TypeCheckingVisitor::visit(std::shared_ptr<TimesExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Arithmetic, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid TimesExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<DivideExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Arithmetic, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid DivideExp Type " << std::endl;
        error = true;
    }
}


void TypeCheckingVisitor::visit(std::shared_ptr<LessExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid LessExp Type " << std::endl;
        error = true;
    }
}


void TypeCheckingVisitor::visit(std::shared_ptr<GreaterExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid GreaterExp Type " << std::endl;
        error = true;
    }
}




// void TypeCheckingVisitor::visit(std::shared_ptr<StringLiteralExp> n) {
//     currentExpType = ExpType::String;
// }
