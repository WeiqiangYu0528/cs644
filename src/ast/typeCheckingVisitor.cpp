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

TypeCheckingVisitor::TypeCheckingVisitor(std::shared_ptr<Scope> s) : scope(s), error(false) {
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
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<Method> n) {
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    if (n->isStatic) scope->current->beginScope(ScopeType::STATIC);
    Visitor::visit(n);
    scope->current->endScope(ScopeType::STATIC);
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<Field> n) {
    if (n->initializer) {
        // initialized scope
        scope->current->beginScope(ScopeType::FIELDINITIALIZER);
        if (n->isStatic) scope->current->beginScope(ScopeType::STATIC);
        n->initializer->accept(this);
        scope->current->endScope(ScopeType::STATIC);
        scope->current->endScope(ScopeType::FIELDINITIALIZER);
    }
    scope->current->setFieldDecl(n->fieldName->name);
}

void TypeCheckingVisitor::visit(std::shared_ptr<BlockStatement> n) {
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<IfStatement> n) {
    // n->exp->accept(this);
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    n->statement1->accept(this);
    auto type = GetExpType(n->exp);
    if(type != ExpType::Any && type != ExpType::Boolean) {
        std::cerr << "Error: Condition clause in if must have type boolean" << std::endl;
        error = true;
        return;
    }        
    scope->current->endScope(ScopeType::LOCALVARIABLE);
    if (n->statement2) 
    {
        scope->current->beginScope(ScopeType::LOCALVARIABLE);
        n->statement2->accept(this);
        scope->current->endScope(ScopeType::LOCALVARIABLE);
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<WhileStatement> n) {
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);
    auto type = GetExpType(n->exp);
    if(type != ExpType::Any && type != ExpType::Boolean) {
        std::cerr << "Error: While condition must be of type boolean" << std::endl;
        error = true;
        return;
    }
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<ForStatement> n) {
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);    
    if (GetExpType(n->exp) == ExpType::Null) {
        error = true;
        std::cerr << "Error: For condition must be of type boolean" << std::endl;
    }
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<IdentifierExp> n) {
    visitIdentifierExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<FieldAccessExp> n) {
    visitFieldAccessExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    visitMethodInvocation(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<Assignment> n) {
    std::string left_obj_name {"basic_type"}, right_obj_name {"basic_type"};
    DataType left_array_type, right_array_type;

    auto right_exp = n->right;
    auto casted_right_exp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(right_exp);
    if (casted_right_exp) {
        visit(casted_right_exp);
    }
    if (auto ie = std::dynamic_pointer_cast<IdentifierExp>(n->left)) {
        scope->current->beginScope(ScopeType::ASSIGNMENT);
    }
    ExpType left_type = GetExpType(n->left);
    scope->current->endScope(ScopeType::ASSIGNMENT);
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
    visitStringLiteralExp(n);
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
    if (currentExpType == ExpType::String) currentObjectTypeName = "String";
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
    visitCastExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<NewArrayExp> n) {
    visitNewArrayExp(n);
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
    visitArrayAccessExp(n);
}

ExpType TypeCheckingVisitor::CalcExpType(ExpRuleType exp, ExpType lhs_type, ExpType rhs_type) {
    if (lhs_type == ExpType::Any || rhs_type == ExpType::Any) return ExpType::Any;
    if (lhs_type == ExpType::Undefined || rhs_type == ExpType::Undefined) {
        return ExpType::Undefined;
    }
    if (exp == ExpRuleType::StringPlus && (lhs_type == ExpType::String || rhs_type == ExpType::String)) 
        return ExpType::String;
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
        if ((left_obj_name == "java.lang.Cloneable" || left_obj_name == "java.io.Serializable" || left_obj_name == "Cloneable" || left_obj_name == "Serializable") && right_type == ExpType::Array)
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

AmbiguousName TypeCheckingVisitor::visitClassInstanceCreationExp(std::shared_ptr<ClassInstanceCreationExp> n) {  
    std::string& cname {n->classType->id->name};
    AmbiguousName ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(cname, n->classType->simple));
    auto classDecl = std::dynamic_pointer_cast<ClassDecl>(ambiguousName.symbolTable->getClassOrInterfaceDecl());
    if (classDecl && classDecl->isAbstract()) {
        std::cerr << "Error: Cannot instantiate abstract class " << cname << std::endl;
        error = true;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    auto constructors = ambiguousName.symbolTable->getConstructor();
    std::vector<argumentExp> arguments;
    for (auto& arg : n->arguments) {
        arg->accept(this);
        arguments.emplace_back(currentExpType, currentObjectTypeName, currentArrayDataType);
    }
    auto constructor = getClosestMatchConstructor(constructors, arguments);
    if (!constructor) {
        std::cerr << "Error: No matching constructor found for " << cname << std::endl;
        error = true;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    currentExpType = ExpType::Object;
    currentObjectTypeName = cname;

    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitIdentifierExp(std::shared_ptr<IdentifierExp> n) {
    const std::string key {n->id->name};
    auto ambiguousName = scope->reclassifyAmbiguousName(key, n->simple);
    if (ambiguousName.type != AmbiguousNamesType::EXPRESSION) {
        std::cerr << "Error: " << key << " is not a valid name in the current scope" << std::endl;
        error = true;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    SetCurrentExpTypebyAmbiguousName(ambiguousName.typeNode);
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitCastExp(std::shared_ptr<CastExp> n) {
    AmbiguousName ambiguousName;
    auto expType = GetExpType(n->exp);
    
    ExpType castType {ExpType::Undefined};
    std::string castObjectTypeName;
    DataType castArrayDataType;
    if (auto type = std::dynamic_pointer_cast<ArrayType>(n->type)) {
        castType = ExpType::Array;
        if (auto dataType = std::dynamic_pointer_cast<IdentifierType>(type->dataType)){
            castArrayDataType = DataType::OBJECT;
            castObjectTypeName = dataType->id->name;
            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(castObjectTypeName, dataType->simple));
            ambiguousName.typeNode = type;
        }
        else {
            castArrayDataType = type->dataType->type;
            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
            ambiguousName.typeNode = type;
        }
    }
    else if (auto type = std::dynamic_pointer_cast<IdentifierType>(n->type)) {
        castType = ExpType::Object;
        castObjectTypeName = type->id->name;
        ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(castObjectTypeName, type->simple));
        ambiguousName.typeNode = type;
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
        if (d2e.contains(n->type->type)) {
            castType = d2e[n->type->type];
            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
            ambiguousName.typeNode = n->type;
        }
        else {
            error = true;
            std::cerr << "Error: Undefined casting " << std::endl;
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;
        }
    }
    if(castType == ExpType::Object && castObjectTypeName == "String")
        castType = ExpType::String; 
    // casting rules here
    if (!castingRules.contains({castType, expType})) {
        error = true;
        std::cerr << "Error: Undefined casting " << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    
    currentExpType = castType;
    currentObjectTypeName = castObjectTypeName;
    currentArrayDataType = castArrayDataType;

    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitParExp(std::shared_ptr<ParExp> n) {
    if (auto ie = std::dynamic_pointer_cast<IdentifierExp>(n->exp)) return visitIdentifierExp(ie);
    if (auto ce = std::dynamic_pointer_cast<CastExp>(n->exp)) return visitCastExp(ce);
    if (auto me = std::dynamic_pointer_cast<MethodInvocation>(n->exp)) return visitMethodInvocation(me);
    currentExpType = GetExpType(n->exp);
    if (currentExpType == ExpType::String)
        return AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->onDemandImported["String"]);
    return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
}

AmbiguousName TypeCheckingVisitor::visitThisExp(std::shared_ptr<ThisExp> n) {
    if (scope->current->getScopeType(ScopeType::STATIC)) {
        std::cerr << "Error: Cannot use this in a static method" << std::endl;
        error = true;
        return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
    }
    currentExpType = ExpType::Any;
    return AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->current);  
}

AmbiguousName TypeCheckingVisitor::visitFieldAccessExp(std::shared_ptr<FieldAccessExp> n) {
    AmbiguousName ambiguousName;
    std::string fieldName{n->field->id->name};
    if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->exp)) ambiguousName = visitThisExp(thisExp);
    else if (auto parExp = std::dynamic_pointer_cast<ParExp>(n->exp)) ambiguousName = visitParExp(parExp);
    else if (auto classexp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->exp)) ambiguousName = visitClassInstanceCreationExp(classexp);
    else if (auto fieldexp = std::dynamic_pointer_cast<FieldAccessExp>(n->exp)) ambiguousName = visitFieldAccessExp(fieldexp);
    else if (auto methodexp = std::dynamic_pointer_cast<MethodInvocation>(n->exp)) ambiguousName = visitMethodInvocation(methodexp);
    else if (auto stringexp = std::dynamic_pointer_cast<StringLiteralExp>(n->exp)) ambiguousName = visitStringLiteralExp(stringexp);
    else if (auto newarrexp = std::dynamic_pointer_cast<NewArrayExp>(n->exp)) ambiguousName = visitNewArrayExp(newarrexp);
    else if (auto arraccessexp = std::dynamic_pointer_cast<ArrayAccessExp>(n->exp)) ambiguousName = visitArrayAccessExp(arraccessexp);

    if (ambiguousName.type == AmbiguousNamesType::ERROR || (ambiguousName.symbolTable == nullptr && (ambiguousName.typeNode == nullptr || ambiguousName.getDataType() != DataType::ARRAY))) {
        std::cerr << "Error: Invalid field access" << std::endl;
        error = true;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    if (ambiguousName.typeNode && ambiguousName.getDataType() == DataType::ARRAY && fieldName == "length") {
        currentExpType = ExpType::Integer;
        ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
        ambiguousName.typeNode = std::make_shared<IntType>();
        return ambiguousName;
    }
    auto field = ambiguousName.symbolTable->getField(fieldName);
    if (!field || field->isStatic) {
        std::cerr << "Error: Invalid field access" << std::endl;
        error = true;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    SetCurrentExpTypebyAmbiguousName(field->type);
    std::shared_ptr<SymbolTable> fieldSt;
    auto ie = std::dynamic_pointer_cast<IdentifierType>(field->type);
    if (ie) fieldSt = ambiguousName.symbolTable->getScope()->getNameInScope(ie->id->name, ie->simple);
    ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, fieldSt);
    ambiguousName.typeNode = field->type;
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitMethodInvocation(std::shared_ptr<MethodInvocation> n) {
    AmbiguousName ambiguousName;
    std::shared_ptr<Method> method;
    std::string methodName;
    std::vector<argumentExp> arguments;
    for (auto& arg : n->arguments) {
        arg->accept(this);
        arguments.emplace_back(currentExpType, currentObjectTypeName, currentArrayDataType);
    }
    //primary method invocation
    if (n->primary) {
        methodName = n->primaryMethodName->id->name;
        if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->primary)) ambiguousName = visitThisExp(thisExp);
        else if (auto classExp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->primary)) ambiguousName = visitClassInstanceCreationExp(classExp);
        else if (auto parexp = std::dynamic_pointer_cast<ParExp>(n->primary)) ambiguousName = visitParExp(parexp);
        else if (auto fieldexp = std::dynamic_pointer_cast<FieldAccessExp>(n->primary)) ambiguousName = visitFieldAccessExp(fieldexp);
        else if (auto methodexp = std::dynamic_pointer_cast<MethodInvocation>(n->primary)) ambiguousName = visitMethodInvocation(methodexp);
        else if (auto stringexp = std::dynamic_pointer_cast<StringLiteralExp>(n->primary)) ambiguousName = visitStringLiteralExp(stringexp);
        else if (auto newarrexp = std::dynamic_pointer_cast<NewArrayExp>(n->primary)) ambiguousName = visitNewArrayExp(newarrexp);
        else if (auto arraccessexp = std::dynamic_pointer_cast<ArrayAccessExp>(n->primary)) ambiguousName = visitArrayAccessExp(arraccessexp);
        if (ambiguousName.type == AmbiguousNamesType::ERROR || ambiguousName.symbolTable == nullptr) {
            std::cerr << "Error: Invalid method name " << methodName << std::endl;
            error = true;
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;
        }
        auto methods = ambiguousName.symbolTable->getMethod(methodName);
        method = getClosestMatchMethod(methods, arguments);
        if (!method) {
            std::cerr << "Error: Invalid method name " << methodName << std::endl;
            error = true;
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;
        }
    }
    //normal method invocation
    if (n->ambiguousMethodName != nullptr) {
        methodName = n->ambiguousMethodName->id->name;
        if (n->ambiguousName) {
            //complex method invocation a.b.c()
            ambiguousName = scope->reclassifyAmbiguousName(n->ambiguousName->id->name, n->ambiguousName->simple);
            if (ambiguousName.type == AmbiguousNamesType::TYPE) {
                auto methods = ambiguousName.symbolTable->getMethod(methodName);
                method = getClosestMatchMethod(methods, arguments);
                if (!method) {
                    std::cerr << "Error: Invalid method name " << methodName << std::endl;
                    error = true;
                    ambiguousName.type = AmbiguousNamesType::ERROR;
                    return ambiguousName;
                }
                if (!method->isStatic) {
                    std::cerr << "Error: no static method found for " << methodName << std::endl;
                    error = true;
                    ambiguousName.type = AmbiguousNamesType::ERROR;
                    return ambiguousName;
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
                    ambiguousName.type = AmbiguousNamesType::ERROR;
                    return ambiguousName;
                }
                if (method->isStatic) {
                    std::cerr << "Error: no non-static method found for " << methodName << std::endl;
                    error = true;
                    ambiguousName.type = AmbiguousNamesType::ERROR;
                    return ambiguousName;
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
                ambiguousName.type = AmbiguousNamesType::ERROR;   
                return ambiguousName;
            }
        }
        else {
            //simple method invocation, e.g. a()
            if (scope->current->getScopeType(ScopeType::STATIC)) {
                std::cerr << "Error: Cannot call non-static method in a static method" << std::endl;
                error = true;
                ambiguousName.type = AmbiguousNamesType::ERROR;
                return ambiguousName;
            }
            std::vector<std::shared_ptr<Method>> methods = scope->current->getMethod(methodName);
            method = getClosestMatchMethod(methods, arguments);
            if (!method) {
                std::cerr << "Error: Invalid non-static method name " << methodName << std::endl;
                error = true;
                ambiguousName.type = AmbiguousNamesType::ERROR;
                return ambiguousName;
            }
            if (method->isStatic) {
                std::cerr << "Error: no non-static method found for " << methodName << std::endl;
                error = true;
                ambiguousName.type = AmbiguousNamesType::ERROR;
                return ambiguousName;
            }
            ambiguousName.symbolTable = scope->current;
        }
    }
    if (method) SetCurrentExpTypebyAmbiguousName(method->type);
    else currentExpType = ExpType::Any;

    std::shared_ptr<SymbolTable> methodSt;
    auto ie = std::dynamic_pointer_cast<IdentifierType>(method->type);
    if (ie) methodSt = ambiguousName.symbolTable->getScope()->getNameInScope(ie->id->name, ie->simple);
    ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, methodSt);
    ambiguousName.typeNode = method->type;
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitStringLiteralExp(std::shared_ptr<StringLiteralExp> n) {
    currentExpType = ExpType::String;
    currentObjectTypeName = "String";
    AmbiguousName ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(currentObjectTypeName, true));
    ambiguousName.typeNode = std::make_shared<IdentifierType>(std::make_shared<Identifier>(currentObjectTypeName), true);
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitNewArrayExp(std::shared_ptr<NewArrayExp> n) {
    AmbiguousName ambiguousName;
    auto indexType = GetExpType(n->exp);
    std::set<ExpType> s { ExpType::Integer, ExpType::Short, ExpType::Char, ExpType::Byte};
    if (!s.contains(indexType)) {
        error = true;
        std::cerr << "Error: Array index must have numeric type" << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }

    currentExpType = ExpType::Array;
    currentArrayDataType = n->type->dataType->type;
    if(auto type = std::dynamic_pointer_cast<IdentifierType>(n->type->dataType)){
        currentObjectTypeName = type->id->name;
    }

    ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
    ambiguousName.typeNode = n->type;
}

AmbiguousName TypeCheckingVisitor::visitArrayAccessExp(std::shared_ptr<ArrayAccessExp> n) {
    AmbiguousName ambiguousName;
    // not finished yet
    auto indexType = GetExpType(n->index);
    std::set<ExpType> s { ExpType::Integer, ExpType::Short, ExpType::Char, ExpType::Byte};
    if (!s.contains(indexType)) {
        error = true;
        std::cerr << "Error: Array index must have numeric type" << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    
    if (auto left = std::dynamic_pointer_cast<IdentifierExp>(n->array)) {
        ambiguousName = scope->reclassifyAmbiguousName(left->id->name, left->simple);
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
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;
        }
    } 
    else {
        currentExpType = ExpType::Any;
    }    
    return ambiguousName;
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
