#include "typeCheckingVisitor.hpp"

std::string expTypeString[] =
{
    "Int",
    "Short",
    "Char",
    "Byte",
    "Boolean",
    "Object",
    "Array",
    "String",
    "Null",
    "Undefined",
    "Any"
};


TypeCheckingVisitor::TypeCheckingVisitor(std::shared_ptr<Scope> s) : scope(s), error(false), initialized(false), staticMethod(false) {
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
    if (n->exp) {
        auto ambiguousName = scope->reclassifyAmbiguousNameByLocal(key);
        SetCurrentExpTypebyAmbiguousName(ambiguousName);
        std::string left_obj_name {"basic_type"}, right_obj_name {"basic_type"};
        DataType left_array_type, right_array_type;
    
        auto left_type = currentExpType;
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
            
        auto right_type = GetExpType(n->exp);
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

        AssignmentTypeCheckingLogic(
            left_type, right_type,
            left_obj_name, right_obj_name,
            left_array_type, right_array_type
        );         
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<Constructor> n) {
    scope->current->beginScope();
    Visitor::visit(n);
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<Method> n) {
    scope->current->beginScope();
    if (n->isStatic) staticMethod = true;
    Visitor::visit(n);
    staticMethod = false;
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
    // n->exp->accept(this);
    auto type = GetExpType(n->exp);
    if(type != ExpType::Boolean) {
        std::cerr << "Error: Condition clause in if must have type boolean" << std::endl;
        error = true;
        return;
    }

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
    auto type = GetExpType(n->exp);
    if(type != ExpType::Boolean) {
        std::cerr << "Error: While condition must be of type boolean" << std::endl;
        error = true;
        return;
    }
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
        return;
    }
    SetCurrentExpTypebyAmbiguousName(ambiguousName);
}

void TypeCheckingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->exp)) {
        if (staticMethod) {
            std::cerr << "Error: Cannot use this in a static method" << std::endl;
            error = true;
            return;
        }
        bool init{initialized};
        initialized = false;
        n->field->accept(this);
        initialized = init;
    } else {
        Visitor::visit(n);
    }
    currentExpType = ExpType::Any;
}

void TypeCheckingVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::vector<argumentExp> arguments;
    for (auto& arg : n->arguments) {
        arg->accept(this);
        std::cout << static_cast<int>(currentExpType) << " " << currentObjectTypeName<< " " << static_cast<int>(currentArrayDataType) << std::endl;
        arguments.emplace_back(currentExpType, currentObjectTypeName, currentArrayDataType);
    }
    //primary method invocation
    if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->primary)) {
        if (staticMethod) {
            std::cerr << "Error: Cannot use this in a static method" << std::endl;
            error = true;
            return;
        }
        // n->primary->accept(this);
    }
    //normal method invocation
    if (n->ambiguousMethodName != nullptr) {
        std::string methodName {n->ambiguousMethodName->id->name};
        if (n->ambiguousName) {
            AmbiguousName ambiguousName = scope->reclassifyAmbiguousName(n->ambiguousName->id->name, n->ambiguousName->simple, initialized);
            if (ambiguousName.type == AmbiguousNamesType::TYPE) {
                auto methods = ambiguousName.symbolTable->getMethod(methodName);
                auto method = getClosestMatchMethod(methods, arguments);
                if (!method) {
                    std::cerr << "Error: Invalid method name " << methodName << std::endl;
                    error = true;
                    return;
                }
                if (!method->isStatic) {
                    std::cerr << "Error: no static method found for " << methodName << std::endl;
                    error = true;
                    return;
                }
                // if (!staticMethod) {
                //     std::cerr << "Error: Cannot call static method in a non-static method" << std::endl;
                //     error = true;
                //     return;
                // }
            }
            else if (ambiguousName.type == AmbiguousNamesType::EXPRESSION) {
                auto methods = ambiguousName.symbolTable->getMethod(methodName);
                auto method = getClosestMatchMethod(methods, arguments);
                if (!method) {
                    std::cerr << "Error: Invalid method name " << methodName << std::endl;
                    error = true;
                    return;
                }
                if (method->isStatic) {
                    std::cerr << "Error: no non-static method found for " << methodName << std::endl;
                    error = true;
                    return;
                }
                // if (staticMethod) {
                //     std::cerr << "Error: Cannot call non-static method in a static method" << std::endl;
                //     error = true;
                //     return;
                // }
            }
            else {
                std::cerr << "Error: Invalid ambiguous name " << n->ambiguousName->id->name << std::endl;
                error = true;     
                return;
            }
        }
        else {
            if (staticMethod) {
                std::cerr << "Error: Cannot call non-static method in a static method" << std::endl;
                error = true;
                return;
            }
            std::vector<std::shared_ptr<Method>> methods = scope->current->getMethod(methodName);
            auto method = getClosestMatchMethod(methods, arguments);
            if (!method) {
                std::cerr << "Error: Invalid non-static method name " << methodName << std::endl;
                error = true;
                return;
            }
            if (method->isStatic) {
                std::cerr << "Error: no non-static method found for " << methodName << std::endl;
                error = true;
                return;
            }
        }
    }
    currentExpType = ExpType::Any;
}

void TypeCheckingVisitor::visit(std::shared_ptr<Assignment> n) {
    std::string left_obj_name {"basic_type"}, right_obj_name {"basic_type"};
    DataType left_array_type, right_array_type;

    auto right_exp = n->right;
    auto casted_right_exp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(right_exp);

    if (casted_right_exp) {
        visit(casted_right_exp);
    }
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

    AssignmentTypeCheckingLogic(
        left_type, right_type,
        left_obj_name, right_obj_name,
        left_array_type, right_array_type
    );
    currentExpType = left_type;
    currentObjectTypeName = left_obj_name;
    currentArrayDataType = left_array_type;
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
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, GetExpType(n->exp1), GetExpType(n->exp2));
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid ModuloExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<NotExp> n) {
    currentExpType = GetExpType(n->exp);
    if (currentExpType == ExpType::Any) return;
    if (currentExpType != ExpType::Boolean) {
        std::cerr << "Error: Invalid NotExp Type " << std::endl;
        error = true;
    }
}


void TypeCheckingVisitor::visit(std::shared_ptr<NegExp> n) {
    currentExpType = GetExpType(n->exp);
    if(currentExpType != ExpType::Char && currentExpType != ExpType::Integer && currentExpType != ExpType::Short) {
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
    auto type1 = GetExpType(n->exp1);
    auto type2 = GetExpType(n->exp2);
    currentExpType = CalcExpType(ExpRuleType::Equality, type1, type2);
    if (currentExpType == ExpType::Undefined) 
        currentExpType = CalcExpType(ExpRuleType::Comparison, type1, type2);
    if(currentExpType == ExpType::Undefined) {
        std::cerr << "Error: Invalid EqualExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<NotEqualExp> n) {
    auto type1 = GetExpType(n->exp1);
    auto type2 = GetExpType(n->exp2);
    currentExpType = CalcExpType(ExpRuleType::Equality, type1, type2);
    if (currentExpType == ExpType::Undefined) 
        currentExpType = CalcExpType(ExpRuleType::Comparison, type1, type2);
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
    if (left_type == ExpType::Any || right_type == ExpType::Any) return;
    if (left_type != ExpType::Boolean || right_type != ExpType::Boolean) {
        std::cerr << "Error: Invalid ConditionalAndExp Type " << std::endl;
        error = true;
    }
    currentExpType = ExpType::Boolean;
}

void TypeCheckingVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    auto left_type = GetExpType(n->exp1);
    auto right_type = GetExpType(n->exp2);
    if (left_type == ExpType::Any || right_type == ExpType::Any) return;
    if (left_type != ExpType::Boolean || right_type != ExpType::Boolean) {
        std::cerr << "Error: Invalid ConditionalOrExp Type " << std::endl;
        error = true;
    }
    currentExpType = ExpType::Boolean;
}

void TypeCheckingVisitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    currentExpType = ExpType::Object;
    currentObjectTypeName = n->classType->id->name;
    auto& methods = scope->onDemandImported;
    for (auto& [name, symbolTable] : methods) {
        auto classDecl = std::dynamic_pointer_cast<ClassDecl>(symbolTable->getClassOrInterfaceDecl());
        if (classDecl && classDecl->isAbstract() && currentObjectTypeName == classDecl->id->name) {
            std::cerr << "Error: Cannot instantiate abstract class " << n->classType->id->name << std::endl;
            error = true;
            break;
        }
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<NulLiteralExp> n) {
    currentExpType = ExpType::Null;
}

void TypeCheckingVisitor::visit(std::shared_ptr<ThisExp> n) {
    currentExpType = ExpType::Any;
}

void TypeCheckingVisitor::visit(std::shared_ptr<CastExp> n) {
    auto expType = GetExpType(n->exp);
    
    ExpType castType {ExpType::Undefined};
    std::string castObjectTypeName;
    DataType castArrayDataType;
    if (auto type = std::dynamic_pointer_cast<ArrayType>(n->type)) {
        castType = ExpType::Array;
        if (auto dataType = std::dynamic_pointer_cast<IdentifierType>(type->dataType)){
            castArrayDataType = DataType::OBJECT;
            castObjectTypeName = dataType->id->name;
        }
        else 
            castArrayDataType = type->dataType->type;
    }
    else if (auto type = std::dynamic_pointer_cast<IdentifierType>(n->type)) {
        castType = ExpType::Object;
        castObjectTypeName = type->id->name;
    }
    else
    {
       std::map<DataType, ExpType> d2e = {
            {DataType::INT, ExpType::Integer},
            {DataType::SHORT, ExpType::Short},
            {DataType::CHAR, ExpType::Char},
            {DataType::BOOLEAN, ExpType::Boolean},
            {DataType::BYTE, ExpType::Byte},
        };
        if (d2e.contains(n->type->type)) 
            castType = d2e[n->type->type];
        else {
            error = true;
            std::cerr << "Error: Undefined casting " << std::endl;
            return;
        }
    }
    
    // casting rules here
    if (!castingRules.contains({castType, expType})) {
        error = true;
        std::cerr << "Error: Undefined casting " << std::endl;
        return;
    }
    
    currentExpType = castType;
    currentObjectTypeName = castObjectTypeName;
    currentArrayDataType = castArrayDataType;
}

void TypeCheckingVisitor::visit(std::shared_ptr<NewArrayExp> n) {
    currentExpType = ExpType::Array;

    if(auto type = std::dynamic_pointer_cast<IdentifierType>(n->type)){
        currentArrayDataType = DataType::OBJECT;
        currentObjectTypeName = type->id->name;
    }
    else
        currentArrayDataType = n->type->type;
}

void TypeCheckingVisitor::visit(std::shared_ptr<ParExp> n) {
    currentExpType = GetExpType(n->exp);
}

void TypeCheckingVisitor::visit(std::shared_ptr<InstanceOfExp> n) {
    currentExpType = ExpType::Any;
}

ExpType TypeCheckingVisitor::CalcExpType(ExpRuleType exp, ExpType lhs_type, ExpType rhs_type) {
    if (lhs_type == ExpType::Any || rhs_type == ExpType::Any) return ExpType::Any;
    if (lhs_type == ExpType::Undefined || rhs_type == ExpType::Undefined) {
        return ExpType::Undefined;
    }
    if (typeOperationRules.contains({exp, lhs_type, rhs_type}))
        return typeOperationRules[{exp, lhs_type, rhs_type}];
    else if(typeOperationRules.contains({exp, rhs_type, lhs_type}))
        return typeOperationRules[{exp, rhs_type, lhs_type}];                
    else
        return ExpType::Undefined;
}

void TypeCheckingVisitor::SetCurrentExpTypebyAmbiguousName(AmbiguousName& ambiguousName) {
    if (ambiguousName.typeNode->type == DataType::INT)
        currentExpType = ExpType::Integer;
    else if (ambiguousName.typeNode->type == DataType::CHAR)        
        currentExpType = ExpType::Char;
    else if (ambiguousName.typeNode->type == DataType::BYTE)
        currentExpType = ExpType::Byte;                
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

void TypeCheckingVisitor::AssignmentTypeCheckingLogic(ExpType left_type, ExpType right_type, 
    std::string left_obj_name, std::string right_obj_name, DataType left_array_type, DataType right_array_type) {
    if(left_type == ExpType::Any || right_type == ExpType::Any) return;

    if(left_type == ExpType::Object) {
        if (left_obj_name == "Object" && (right_type == ExpType::Object || right_type == ExpType::Array || right_type == ExpType::String || right_type == ExpType::Null))
            return;
        else if(right_type == ExpType::Object && left_obj_name == right_obj_name)
                return;
        else {
            error = true;
            std::cerr << "Error: Invalid Assignment Type: Invalid Assignment to Object" << std::endl;
            return;
        }
    }
    else if (left_type == ExpType::Array) {
        if (right_type == ExpType::Array) {
            if (left_array_type == DataType::OBJECT && right_array_type == DataType::OBJECT) {
              if (left_obj_name == "Object" || left_obj_name == right_obj_name) 
                return;
            }
            else if (left_array_type == right_array_type)
                 return;
        }
        error = true;
        std::cerr << "Error: Invalid Assignment Type: Invalid Assignment to Array" << std::endl;
        return;
    }
    else if (!assginmentRules.contains({left_type, right_type})) {
        error = true;
        std::cerr << "Error: Invalid Assignment Type: " << expTypeString[(int)left_type] << " <=== " << expTypeString[(int)right_type] << std::endl;              
    }
}

std::shared_ptr<Method> TypeCheckingVisitor::getClosestMatchMethod(std::vector<std::shared_ptr<Method>>& methods, std::vector<argumentExp>& arguments) {
    std::shared_ptr<Method> closestMatch;
    for (auto method : methods) {
        auto parameters = method->formalParameters;
        if (parameters.size() != arguments.size()) continue;
        bool match{true};
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (!isTypeCompatible(parameters[i]->type, arguments[i])) {
                std::cout << static_cast<int>(parameters[i]->type->type) << " " << static_cast<int>(arguments[i].expType) << std::endl;
                match = false;
                break;
            }
        }
        if (match) {
            closestMatch = method;
            break;
        }
    }
    return closestMatch;
}

bool TypeCheckingVisitor::isTypeCompatible(std::shared_ptr<Type> dataType, argumentExp& argument) {
    std::cout << static_cast<int>(argument.expType) << " " << argument.objectName << " " << static_cast<int>(argument.arrayType) << std::endl;
    if (dataType->type == DataType::INT && argument.expType == ExpType::Integer) return true;
    if (dataType->type == DataType::SHORT && argument.expType == ExpType::Short) return true;
    if (dataType->type == DataType::CHAR && argument.expType == ExpType::Char) return true;
    if (dataType->type == DataType::BYTE && argument.expType == ExpType::Byte) return true;
    if (dataType->type == DataType::BOOLEAN && argument.expType == ExpType::Boolean) return true;
    if (dataType->type == DataType::OBJECT && (argument.expType == ExpType::Object || argument.expType == ExpType::String)) {
        std::cout << std::dynamic_pointer_cast<IdentifierType>(dataType)->id->name << " " << argument.objectName << std::endl;
        return std::dynamic_pointer_cast<IdentifierType>(dataType)->id->name == argument.objectName;
    }
    if (dataType->type == DataType::ARRAY && argument.expType == ExpType::Array) {
        auto arrayType = std::dynamic_pointer_cast<ArrayType>(dataType)->dataType;
        if (auto it = std::dynamic_pointer_cast<IdentifierType>(arrayType)) {
            return it->id->name == argument.objectName;
        }
        return arrayType->type  == argument.arrayType;
    }
    return false;
}
