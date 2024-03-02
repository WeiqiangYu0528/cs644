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

    if (ambiguousName.typeNode->type == DataType::INT)
        currentExpType = ExpType::Integer;
    else if (ambiguousName.typeNode->type == DataType::CHAR)        
        currentExpType = ExpType::Char;
    else if (ambiguousName.typeNode->type == DataType::SHORT)
        currentExpType = ExpType::Short;
    else if (ambiguousName.typeNode->type == DataType::BOOLEAN)        
        currentExpType = ExpType::Boolean;
    else if (ambiguousName.typeNode->type == DataType::OBJECT) {
        currentExpType = ExpType::Object;    
        currentObjectTypeName = std::dynamic_pointer_cast<IdentifierType>(ambiguousName.typeNode)->id->name;
    }
    else if (ambiguousName.typeNode->type == DataType::ARRAY) {
        currentExpType = ExpType::Array;                
        auto dataTypeNode = std::dynamic_pointer_cast<ArrayType>(ambiguousName.typeNode)->dataType;
        currentArrayDataType = dataTypeNode->type;
        if (currentArrayDataType == DataType::OBJECT) {
            currentObjectTypeName = std::dynamic_pointer_cast<IdentifierType>(dataTypeNode)->id->name;
        }                
    }
    else
        currentExpType = ExpType::Undefined;
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
    std::string left_obj_name {"basic_type"}, right_obj_name {"basic_type"};
    DataType left_array_type, right_array_type;

    auto left_type = GetExpType(n->left);
    if (left_type == ExpType::Object) {
        left_obj_name = currentObjectTypeName;
        if (left_obj_name == "String")
            left_type = ExpType::String;
    }
    else if (left_type == ExpType::Array) {
        left_array_type = currentArrayDataType;
        if (left_array_type == DataType::OBJECT)
            left_obj_name = currentObjectTypeName;
    }
        
    auto right_type = GetExpType(n->right);
    if (right_type == ExpType::Object) {
        right_obj_name = currentObjectTypeName;
        if (right_obj_name == "String")
            right_type = ExpType::String;        
    }

    else if (right_type == ExpType::Array) {
        right_array_type = currentArrayDataType;
        if (right_array_type == DataType::OBJECT)
            right_obj_name = currentObjectTypeName;
    }

    if(left_type == ExpType::Object && right_type == ExpType::Object) {
        if (left_obj_name != right_obj_name) {
            error = true;
            std::cerr << "Error: Invalid Assignment Type: assignment between different object types" << std::endl;
        }
    }
    else if ((left_type != ExpType::Object && right_type == ExpType::Object) || (left_type == ExpType::Object && right_type != ExpType::Object)) {
        error = true;
        std::cerr << "Error: Invalid Assignment Type: assignment between object and non-object type" << std::endl;
    }
    else if (left_type == ExpType::Array && right_type == ExpType::Array) {
        if (!(left_array_type == right_array_type && left_obj_name == right_obj_name)) {
            error = true;
            std::cerr << "Error: Invalid Assignment Type: assignment between different array types" << std::endl;
        }
    } 
    else if ((left_type != ExpType::Array && right_type == ExpType::Array) || (left_type == ExpType::Array && right_type != ExpType::Array)) {
        error = true;
        std::cerr << "Error: Invalid Assignment Type: assignment between array and non-array type" << std::endl;
    }
    else if((left_type == ExpType::String && right_type != ExpType::String) || (left_type != ExpType::String && right_type == ExpType::String)) {
        error = true;
        std::cerr << "Error: Invalid Assignment Type: assignment between string and non-string type" << std::endl;        
    }
    else if (!assginmentRules.contains({left_type, right_type})) {
        error = true;
        if (left_type == ExpType::Boolean || right_type == ExpType::Boolean)
            std::cerr << "Error: Invalid Assignment Type: assignment between int/char/short and boolean" << std::endl;
        else
            std::cerr << "Error: Invalid Assignment Type: narrowing conversion" << std::endl;
    }
    
}

bool TypeCheckingVisitor::isError() const {
    return error;
}


void TypeCheckingVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) {
    currentExpType = ExpType::Integer;
}

void TypeCheckingVisitor::visit(std::shared_ptr<CharLiteralExp> n) {
    currentExpType = ExpType::Char;
}

void TypeCheckingVisitor::visit(std::shared_ptr<BoolLiteralExp> n) {
    currentExpType = ExpType::Boolean;
}

void TypeCheckingVisitor::visit(std::shared_ptr<StringLiteralExp> n) {
    currentExpType = ExpType::String;
}

void TypeCheckingVisitor::visit(std::shared_ptr<PlusExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid PlusExp Type " << std::endl;
        error = true;
    }    
}

void TypeCheckingVisitor::visit(std::shared_ptr<MinusExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid MinusExp Type " << std::endl;
        error = true;
    }    
}

void TypeCheckingVisitor::visit(std::shared_ptr<TimesExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid TimesExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<DivideExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid DivideExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<ModuloExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid ModuloExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<NotExp> n) {
    auto exp_type = GetExpType(n->exp);
    if (exp_type != ExpType::Boolean) {
        std::cerr << "Error: NotExp Type " << std::endl;
        error = true;
    }
}


void TypeCheckingVisitor::visit(std::shared_ptr<NegExp> n) {
    auto exp_type = GetExpType(n->exp);
    if(exp_type != ExpType::Char && exp_type != ExpType::Integer && exp_type != ExpType::Short) {
        std::cerr << "Error: Invalid NegExp Type " << std::endl;
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

void TypeCheckingVisitor::visit(std::shared_ptr<LessEqualExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid LessEqualExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<GreaterEqualExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid GreaterEqualExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<EqualExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid EqualExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<NotEqualExp> n) {
    currentExpType = CalcExpType(ExpRuleType::Comparison, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid NotEqualExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<AndExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid AndExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<XorExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid XorExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<OrExp> n) {
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid OrExp Type " << std::endl;
        error = true;
    }
}


void TypeCheckingVisitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    auto left_type = GetExpType(n->exp1);
    auto right_type = GetExpType(n->exp2);
    if (left_type != ExpType::Boolean || right_type != ExpType::Boolean) {
        std::cerr << "Error: Invalid ConditionalAndExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    auto left_type = GetExpType(n->exp1);
    auto right_type = GetExpType(n->exp2);
    if (left_type != ExpType::Boolean || right_type != ExpType::Boolean) {
        std::cerr << "Error: Invalid ConditionalOrExp Type " << std::endl;
        error = true;
    }
}



// void TypeCheckingVisitor::visit(std::shared_ptr<StringLiteralExp> n) {
//     currentExpType = ExpType::String;
// }
