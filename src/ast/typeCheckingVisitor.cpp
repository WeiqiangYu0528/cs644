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

std::map<DataType, ExpType> d2e = {
    {DataType::INT, ExpType::Integer},
    {DataType::SHORT, ExpType::Short},
    {DataType::CHAR, ExpType::Char},
    {DataType::BOOLEAN, ExpType::Boolean},
    {DataType::BYTE, ExpType::Byte},
};

TypeCheckingVisitor::TypeCheckingVisitor(std::shared_ptr<Scope> s) : scope(s), error(false), initialized(false), staticMethod(false) {
    // for testing only
    std::cout << scope->current->getClassOrInterfaceDecl()->id->name << std::endl;
}

void TypeCheckingVisitor::visit(std::shared_ptr<Program> n) {
    if (n->scope->supers.size() > 0) {
        for (std::shared_ptr<SymbolTable> superMethod : n->scope->supers) {
            // check if zero argument constructor is present
                std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(superMethod->getClassOrInterfaceDecl());            
                if (classDecl) {
                    bool isZeroArgConstructor = false;
                    for (auto& constructor : classDecl->declarations[2]) {
                        if (std::dynamic_pointer_cast<Constructor>(constructor)->formalParameters.size() == 0) {
                            isZeroArgConstructor = true;
                        }
                    }
                    if (!isZeroArgConstructor) {
                        std::cerr << "Error: No zero argument constructor found in super class" << std::endl;
                        error = true;
                        return;
                    }
                }
        }

    }
    n->classOrInterfaceDecl->accept(this);
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
        SetCurrentExpTypebyAmbiguousName(ambiguousName.typeNode);
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
    scope->current->beginScope();
    n->statement1->accept(this);
    auto type = GetExpType(n->exp);
    if(type != ExpType::Any && type != ExpType::Boolean) {
        std::cerr << "Error: Condition clause in if must have type boolean" << std::endl;
        error = true;
        return;
    }        
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
    auto type = GetExpType(n->exp);
    if(type != ExpType::Any && type != ExpType::Boolean) {
        std::cerr << "Error: While condition must be of type boolean" << std::endl;
        error = true;
        return;
    }
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<ForStatement> n) {
    scope->current->beginScope();
    Visitor::visit(n);    
    if (GetExpType(n->exp) == ExpType::Null) {
        error = true;
        std::cerr << "Error: For condition must be of type boolean" << std::endl;
    }
    scope->current->endScope();
}

void TypeCheckingVisitor::visit(std::shared_ptr<IdentifierExp> n) {
    visitIdentifierExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    std::shared_ptr<SymbolTable> st;
    if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->exp)) st = visitThisExp(thisExp);
    else if (auto parExp = std::dynamic_pointer_cast<ParExp>(n->exp)) st = visitParExp(parExp);
    else if (auto classexp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->exp)) st = visitClassInstanceCreationExp(classexp);
    if (!st) {
        std::cerr << "Error: Invalid field access" << std::endl;
        error = true;
        return;
    }
    auto field = st->getField(n->field->id->name);
    if (!field || field->isStatic) {
        std::cerr << "Error: Invalid field access" << std::endl;
        error = true;
        return;
    }
    SetCurrentExpTypebyAmbiguousName(field->type);
    // else {
    //     Visitor::visit(n);
    //     currentExpType = ExpType::Any;
    // }
}

void TypeCheckingVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::shared_ptr<Method> method;
    std::vector<argumentExp> arguments;
    for (auto& arg : n->arguments) {
        arg->accept(this);
        arguments.emplace_back(currentExpType, currentObjectTypeName, currentArrayDataType);
    }
    //primary method invocation
    if (n->primary) {
        std::shared_ptr<SymbolTable> st;
        if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->primary)) st = visitThisExp(thisExp);
        else if (auto classExp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->primary)) st = visitClassInstanceCreationExp(classExp);
        else if (auto parexp = std::dynamic_pointer_cast<ParExp>(n->primary)) st = visitParExp(parexp);
        if (!st) {
            std::cerr << "Error: Invalid method name " << n->primaryMethodName->id->name << std::endl;
            error = true;
            return;
        }
        auto methods = st->getMethod(n->primaryMethodName->id->name);
        method = getClosestMatchMethod(methods, arguments);
        if (!method) {
            std::cerr << "Error: Invalid method name " << n->primaryMethodName->id->name << std::endl;
            error = true;
            return;
        }
    }
    //normal method invocation
    if (n->ambiguousMethodName != nullptr) {
        std::string methodName {n->ambiguousMethodName->id->name};
        if (n->ambiguousName) {
            AmbiguousName ambiguousName = scope->reclassifyAmbiguousName(n->ambiguousName->id->name, n->ambiguousName->simple, initialized, staticMethod);
            if (ambiguousName.type == AmbiguousNamesType::TYPE) {
                auto methods = ambiguousName.symbolTable->getMethod(methodName);
                method = getClosestMatchMethod(methods, arguments);
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
            else if (ambiguousName.type == AmbiguousNamesType::EXPRESSION && ambiguousName.symbolTable) {
                auto methods = ambiguousName.symbolTable->getMethod(methodName);
                method = getClosestMatchMethod(methods, arguments);
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
            method = getClosestMatchMethod(methods, arguments);
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
    if (method) SetCurrentExpTypebyAmbiguousName(method->type);
    else currentExpType = ExpType::Any;
}

void TypeCheckingVisitor::visit(std::shared_ptr<Assignment> n) {
    std::string left_obj_name {"basic_type"}, right_obj_name {"basic_type"};
    DataType left_array_type, right_array_type;

    auto right_exp = n->right;
    auto casted_right_exp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(right_exp);
    if (casted_right_exp) {
        visit(casted_right_exp);
    }
    ExpType left_type;
    auto left_assign = std::dynamic_pointer_cast<IdentifierExp>(n->left);
    if (left_assign) {
        bool init{initialized};
        initialized = false;
        left_type = GetExpType(n->left);
        initialized = init;
    }
    else {
        left_type = GetExpType(n->left);
    }
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
    currentObjectTypeName = "String";
}

void TypeCheckingVisitor::visit(std::shared_ptr<PlusExp> n) {
    auto t1 = GetExpType(n->exp1), t2 = GetExpType(n->exp2);
    currentExpType = CalcExpType(ExpRuleType::ArithmeticOrBitwise, t1, t2);
    if (currentExpType == ExpType::Undefined)
        currentExpType = CalcExpType(ExpRuleType::StringPlus, t1, t2);
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
    visitClassInstanceCreationExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<NulLiteralExp> n) {
    currentExpType = ExpType::Null;
}

void TypeCheckingVisitor::visit(std::shared_ptr<ThisExp> n) {
    visitThisExp(n);
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
    if(castType == ExpType::Object && castObjectTypeName == "String")
        castType = ExpType::String; 
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
    visitParExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<InstanceOfExp> n) {
    auto lhsType = GetExpType(n->exp);
    auto lhsObjectTyeName = currentObjectTypeName;
    auto lhsArrayDataType = currentArrayDataType;

    SetCurrentExpTypebyAmbiguousName(n->type);
    auto rhsType = currentExpType;
    auto rhsObjectTyeName = currentObjectTypeName;
    auto rhsArrayDataType = currentArrayDataType;

    currentExpType = ExpType::Boolean;

    std::set<ExpType> s { ExpType::Integer, ExpType::Short, ExpType::Char, ExpType::Byte};
    if (s.contains(lhsType)) {
        error = true;
        std::cerr << "Error: Instanceof on simple types" << std::endl;
        return;
    }        

    
}

void TypeCheckingVisitor::visit(std::shared_ptr<ArrayAccessExp> n) {
    // not finished yet
    Visitor::visit(n);
    if (auto left = std::dynamic_pointer_cast<IdentifierExp>(n->array)) {
        auto ambiguousName = scope->reclassifyAmbiguousName(left->id->name, left->simple, initialized, staticMethod);
        SetCurrentExpTypebyAmbiguousName(ambiguousName.typeNode);
        if (currentArrayDataType == DataType::OBJECT) {
            currentExpType = ExpType::Object;
            if (currentObjectTypeName == "String")
                currentExpType = ExpType::String;
        }   
        else if (d2e.contains(currentArrayDataType)) 
            currentExpType = d2e[currentArrayDataType];
        else {
            error = true;
            std::cerr << "Error: Undefined Conversion from DataType to ExpType" << std::endl;
            return;
        }
    } 
    else {
        currentExpType = ExpType::Any;
    }    
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

void TypeCheckingVisitor::SetCurrentExpTypebyAmbiguousName(std::shared_ptr<Type> typeNode) {
    if (typeNode->type == DataType::INT)
        currentExpType = ExpType::Integer;
    else if (typeNode->type == DataType::CHAR)        
        currentExpType = ExpType::Char;
    else if (typeNode->type == DataType::BYTE)
        currentExpType = ExpType::Byte;                
    else if (typeNode->type == DataType::SHORT)
        currentExpType = ExpType::Short;
    else if (typeNode->type == DataType::BOOLEAN)        
        currentExpType = ExpType::Boolean;
    else if (typeNode->type == DataType::OBJECT) {
        currentObjectTypeName = std::dynamic_pointer_cast<IdentifierType>(typeNode)->id->name;
        if (currentObjectTypeName == "String") currentExpType = ExpType::String;
        else currentExpType = ExpType::Object;
    }
    else if (typeNode->type == DataType::ARRAY) {
        currentExpType = ExpType::Array;                
        auto dataTypeNode = std::dynamic_pointer_cast<ArrayType>(typeNode)->dataType;
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
        // Null can be assigned to any reference type
        else if (right_type == ExpType::Null) return;
        else if(right_type == ExpType::Object && checkIsSubclassByName(left_obj_name, right_obj_name))
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
              if (left_obj_name == "Object" || checkIsSubclassByName(left_obj_name, right_obj_name)) 
                return;
            }
            else if (left_array_type == right_array_type)
                 return;
        }
        else if (right_type == ExpType::Null) return;
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

std::shared_ptr<Constructor> TypeCheckingVisitor::getClosestMatchConstructor(std::vector<std::shared_ptr<Constructor>>& constructors, std::vector<argumentExp>& arguments) {
    std::shared_ptr<Constructor> closestMatch;
    for (auto constructor : constructors) {
        auto parameters = constructor->formalParameters;
        if (parameters.size() != arguments.size()) continue;
        bool match{true};
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (!isTypeCompatible(parameters[i]->type, arguments[i])) {
                match = false;
                break;
            }
        }
        if (match) {
            closestMatch = constructor;
            break;
        }
    }
    return closestMatch;
}

bool TypeCheckingVisitor::isTypeCompatible(std::shared_ptr<Type> dataType, argumentExp& argument) {
    if (dataType->type == DataType::INT && argument.expType == ExpType::Integer) return true;
    if (dataType->type == DataType::SHORT && argument.expType == ExpType::Short) return true;
    if (dataType->type == DataType::CHAR && argument.expType == ExpType::Char) return true;
    if (dataType->type == DataType::BYTE && argument.expType == ExpType::Byte) return true;
    if (dataType->type == DataType::BOOLEAN && argument.expType == ExpType::Boolean) return true;
    if (dataType->type == DataType::OBJECT && (argument.expType == ExpType::Object || argument.expType == ExpType::String)) {
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

std::shared_ptr<SymbolTable> TypeCheckingVisitor::visitClassInstanceCreationExp(std::shared_ptr<ClassInstanceCreationExp> n) {  
    std::string& cname {n->classType->id->name};
    std::shared_ptr<SymbolTable> st = scope->getNameInScope(cname, n->classType->simple);
    auto classDecl = std::dynamic_pointer_cast<ClassDecl>(st->getClassOrInterfaceDecl());
    if (classDecl && classDecl->isAbstract()) {
        std::cerr << "Error: Cannot instantiate abstract class " << cname << std::endl;
        error = true;
        return nullptr;
    }
    auto constructors = st->getConstructor();
    std::vector<argumentExp> arguments;
    for (auto& arg : n->arguments) {
        arg->accept(this);
        arguments.emplace_back(currentExpType, currentObjectTypeName, currentArrayDataType);
    }
    auto constructor = getClosestMatchConstructor(constructors, arguments);
    if (!constructor) {
        std::cerr << "Error: No matching constructor found for " << cname << std::endl;
        error = true;
        return nullptr;
    }
    currentExpType = ExpType::Object;
    currentObjectTypeName = cname;

    return error ? nullptr : st;
}

std::shared_ptr<SymbolTable> TypeCheckingVisitor::visitIdentifierExp(std::shared_ptr<IdentifierExp> n) {
    const std::string key {n->id->name};
    auto ambiguousName = scope->reclassifyAmbiguousName(key, n->simple, initialized, staticMethod);
    if (ambiguousName.type != AmbiguousNamesType::EXPRESSION) {
        std::cerr << "Error: " << key << " is not a valid name in the current scope" << std::endl;
        error = true;
        return nullptr;
    }
    SetCurrentExpTypebyAmbiguousName(ambiguousName.typeNode);
    return ambiguousName.symbolTable;
}

std::shared_ptr<SymbolTable> TypeCheckingVisitor::visitCastExp(std::shared_ptr<CastExp> n) {
    std::shared_ptr<SymbolTable> st;
    auto expType = GetExpType(n->exp);
    
    ExpType castType {ExpType::Undefined};
    std::string castObjectTypeName;
    DataType castArrayDataType;
    if (auto type = std::dynamic_pointer_cast<ArrayType>(n->type)) {
        castType = ExpType::Array;
        if (auto dataType = std::dynamic_pointer_cast<IdentifierType>(type->dataType)){
            castArrayDataType = DataType::OBJECT;
            castObjectTypeName = dataType->id->name;
            st = scope->getNameInScope(castObjectTypeName, dataType->simple);
        }
        else 
            castArrayDataType = type->dataType->type;
    }
    else if (auto type = std::dynamic_pointer_cast<IdentifierType>(n->type)) {
        castType = ExpType::Object;
        castObjectTypeName = type->id->name;
        st = scope->getNameInScope(castObjectTypeName, type->simple);
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
            return nullptr;
        }
    }
    if(castType == ExpType::Object && castObjectTypeName == "String")
        castType = ExpType::String; 
    // casting rules here
    if (!castingRules.contains({castType, expType})) {
        error = true;
        std::cerr << "Error: Undefined casting " << std::endl;
        return nullptr;
    }
    
    currentExpType = castType;
    currentObjectTypeName = castObjectTypeName;
    currentArrayDataType = castArrayDataType;

    return st;
}

std::shared_ptr<SymbolTable> TypeCheckingVisitor::visitParExp(std::shared_ptr<ParExp> n) {
    if (auto ie = std::dynamic_pointer_cast<IdentifierExp>(n->exp)) return visitIdentifierExp(ie);
    if (auto ce = std::dynamic_pointer_cast<CastExp>(n->exp)) return visitCastExp(ce);
    currentExpType = GetExpType(n->exp);
    if (currentExpType == ExpType::String)
        return scope->onDemandImported["String"];
    return nullptr;
}

std::shared_ptr<SymbolTable> TypeCheckingVisitor::visitThisExp(std::shared_ptr<ThisExp> n) {
    if (staticMethod) {
        std::cerr << "Error: Cannot use this in a static method" << std::endl;
        error = true;
        return nullptr;
    }
    return scope->current;
}

bool TypeCheckingVisitor::checkIsSubclassByName(std::string o1, std::string o2) {
    if(o1 == o2) return true;

    auto getTable = [&](const std::string& o) {
        return scope->packageMembers.contains(o) ? scope->packageMembers[o] :
            scope->onDemandImported.contains(o) ? scope->onDemandImported[o] :
            scope->singleImported.contains(o) ? scope->singleImported[o] : nullptr;
    };
    auto inSupers = [](const std::shared_ptr<SymbolTable> table, const std::vector<std::shared_ptr<SymbolTable>>& supers) {
        return std::find(supers.begin(), supers.end(), table) != supers.end();
    };

    auto o1Table = getTable(o1);
    auto o2Table = getTable(o2);
    // auto o1Supers = o1Table->getScope()->supers;
    auto o2Supers = o2Table->getScope()->supers;
    
    return inSupers(o1Table, o2Supers);
}