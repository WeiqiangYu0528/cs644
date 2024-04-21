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
        auto left_type = currentExpInfo;
        auto right_type = GetExpInfo(n->exp);
        AssignmentTypeCheckingLogic(left_type, right_type);         
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<Constructor> n) {
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    methodTypeInfo.expType = ExpType::Undefined;
    Visitor::visit(n);
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<Method> n) {
    n->className = scope->current->getClassOrInterfaceDecl()->id->name;
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    if (n->isStatic) scope->current->beginScope(ScopeType::STATIC);
    SetCurrentExpTypebyAmbiguousName(n->type);
    methodTypeInfo = currentExpInfo;
    Visitor::visit(n);
    scope->current->endScope(ScopeType::STATIC);
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<Field> n) {
    if (n->initializer) {
        // initialized scope
        scope->current->beginScope(ScopeType::FIELDINITIALIZER);
        if (n->isStatic) scope->current->beginScope(ScopeType::STATIC);
        SetCurrentExpTypebyAmbiguousName(n->type);
        auto decl_type = currentExpInfo;
        auto init_type = GetExpInfo(n->initializer);
        AssignmentTypeCheckingLogic(decl_type, init_type);
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
    auto type = GetExpInfo(n->exp);
    if(type.expType != ExpType::Any && type.expType != ExpType::Boolean) {
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
    auto type = GetExpInfo(n->exp);
    if(type.expType != ExpType::Any && type.expType != ExpType::Boolean) {
        std::cerr << "Error: While condition must be of type boolean" << std::endl;
        error = true;
        return;
    }
    scope->current->endScope(ScopeType::LOCALVARIABLE);
}

void TypeCheckingVisitor::visit(std::shared_ptr<ForStatement> n) {
    scope->current->beginScope(ScopeType::LOCALVARIABLE);
    Visitor::visit(n);    
    if (GetExpInfo(n->exp).expType == ExpType::Null) {
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
    visitAssignment(n);
}

bool TypeCheckingVisitor::isError() const {
    return error;
}


void TypeCheckingVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) {
    currentExpInfo.expType = ExpType::Integer;
}

void TypeCheckingVisitor::visit(std::shared_ptr<CharLiteralExp> n) {
    currentExpInfo.expType = ExpType::Char;
}

void TypeCheckingVisitor::visit(std::shared_ptr<BoolLiteralExp> n) {
    currentExpInfo.expType = ExpType::Boolean;
}

void TypeCheckingVisitor::visit(std::shared_ptr<StringLiteralExp> n) {
    visitStringLiteralExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<PlusExp> n) {
    visitBinaryOpExp(n, ExpRuleType::Arithmetic, ExpRuleType::StringPlus);
}

void TypeCheckingVisitor::visit(std::shared_ptr<MinusExp> n) {
    visitBinaryOpExp<MinusExp>(n, ExpRuleType::Arithmetic);
}

void TypeCheckingVisitor::visit(std::shared_ptr<TimesExp> n) {
    visitBinaryOpExp<TimesExp>(n, ExpRuleType::Arithmetic);
}

void TypeCheckingVisitor::visit(std::shared_ptr<DivideExp> n) {
    visitBinaryOpExp<DivideExp>(n, ExpRuleType::Arithmetic);    
}

void TypeCheckingVisitor::visit(std::shared_ptr<ModuloExp> n) {
    visitBinaryOpExp<ModuloExp>(n, ExpRuleType::Arithmetic);       
}

void TypeCheckingVisitor::visit(std::shared_ptr<NotExp> n) {
    currentExpInfo = GetExpInfo(n->exp);
    // if (currentExpInfo.expType == ExpType::Any) return;
    if (currentExpInfo.expType != ExpType::Boolean) {
        std::cerr << "Error: Invalid NotExp Type " << std::endl;
        error = true;
    }
}


void TypeCheckingVisitor::visit(std::shared_ptr<NegExp> n) {
    currentExpInfo = GetExpInfo(n->exp);
    if(currentExpInfo.expType != ExpType::Char 
        && currentExpInfo.expType != ExpType::Integer 
        && currentExpInfo.expType != ExpType::Short) {
        std::cerr << "Error: Invalid NegExp Type " << std::endl;
        error = true;
    }
}

void TypeCheckingVisitor::visit(std::shared_ptr<LessExp> n) {
    visitBinaryOpExp<LessExp>(n, ExpRuleType::Comparison);  
}

void TypeCheckingVisitor::visit(std::shared_ptr<GreaterExp> n) {
    visitBinaryOpExp<GreaterExp>(n, ExpRuleType::Comparison); 
}

void TypeCheckingVisitor::visit(std::shared_ptr<LessEqualExp> n) {
    visitBinaryOpExp<LessEqualExp>(n, ExpRuleType::Comparison); 
}

void TypeCheckingVisitor::visit(std::shared_ptr<GreaterEqualExp> n) {
    visitBinaryOpExp<GreaterEqualExp>(n, ExpRuleType::Comparison);     
}

void TypeCheckingVisitor::visit(std::shared_ptr<EqualExp> n) {
    visitBinaryOpExp(n, ExpRuleType::Equality, ExpRuleType::Comparison);
}

void TypeCheckingVisitor::visit(std::shared_ptr<NotEqualExp> n) {
    visitBinaryOpExp(n, ExpRuleType::Equality, ExpRuleType::Comparison);
}

void TypeCheckingVisitor::visit(std::shared_ptr<AndExp> n) {
    //error = true;
    //std::cerr << "Error: Bitwise operations not allowed" << std::endl;
    visitBinaryOpExp<AndExp>(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<XorExp> n) {
    // error = true;
    // std::cerr << "Error: Bitwise operations not allowed" << std::endl;    
    visitBinaryOpExp<XorExp>(n);    
}

void TypeCheckingVisitor::visit(std::shared_ptr<OrExp> n) {
    // error = true;
    // std::cerr << "Error: Bitwise operations not allowed" << std::endl;  
    visitBinaryOpExp<OrExp>(n);       
}


void TypeCheckingVisitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    visitBinaryOpExp<ConditionalAndExp>(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    visitBinaryOpExp<ConditionalOrExp>(n);

}

void TypeCheckingVisitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    visitClassInstanceCreationExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<NulLiteralExp> n) {
    currentExpInfo.expType = ExpType::Null;
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
    auto lhs = GetExpInfo(n->exp);
    SetCurrentExpTypebyAmbiguousName(n->type);
    auto rhs = currentExpInfo;
    // check subclass?
    currentExpInfo.expType = ExpType::Boolean;

    std::set<ExpType> s { ExpType::Integer, ExpType::Short, ExpType::Char, ExpType::Byte};
    if (s.contains(lhs.expType)) {
        error = true;
        std::cerr << "Error: Instanceof on simple types" << std::endl;
        return;
    }        

    
}

void TypeCheckingVisitor::visit(std::shared_ptr<ArrayAccessExp> n) {
    visitArrayAccessExp(n);
}

void TypeCheckingVisitor::visit(std::shared_ptr<ReturnStatement> n) {
    if(n->exp) {
        auto ret = GetExpInfo(n->exp);
        // if (methodTypeInfo.expType != ExpType::Void)
        AssignmentTypeCheckingLogic(methodTypeInfo, ret);
        currentExpInfo = ret;
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
    if (!typeNode) return;
    if (typeNode->type == DataType::INT)
        currentExpInfo.expType = ExpType::Integer;
    else if (typeNode->type == DataType::CHAR)        
        currentExpInfo.expType = ExpType::Char;
    else if (typeNode->type == DataType::BYTE)
        currentExpInfo.expType = ExpType::Byte;                
    else if (typeNode->type == DataType::SHORT)
        currentExpInfo.expType =  ExpType::Short;
    else if (typeNode->type == DataType::BOOLEAN)        
        currentExpInfo.expType =  ExpType::Boolean;
    else if (typeNode->type == DataType::VOID)        
        currentExpInfo.expType =  ExpType::Void;        
    else if (typeNode->type == DataType::OBJECT) {
        currentExpInfo.objectName = std::dynamic_pointer_cast<IdentifierType>(typeNode)->id->name;
        if (currentExpInfo.objectName == "String") currentExpInfo.expType = ExpType::String;
        else currentExpInfo.expType = ExpType::Object;
    }
    else if (typeNode->type == DataType::ARRAY) {
        currentExpInfo.expType = ExpType::Array;                
        auto dataTypeNode = std::dynamic_pointer_cast<ArrayType>(typeNode)->dataType;
        currentExpInfo.arrayType = dataTypeNode->type;
        if (currentExpInfo.arrayType == DataType::OBJECT) {
            currentExpInfo.objectName = std::dynamic_pointer_cast<IdentifierType>(dataTypeNode)->id->name;
        }                
    }
    else
        currentExpInfo.expType = ExpType::Undefined;
}

AmbiguousName TypeCheckingVisitor::AssignmentTypeCheckingLogic(argumentExp& left_type, argumentExp& right_type) {
    AmbiguousName ambiguousName;
    if(left_type.expType == ExpType::Any || right_type.expType == ExpType::Any) return ambiguousName;

    if(left_type.expType  == ExpType::Object) {
        if (checkIsSameSymbolTable("java.lang.Object", left_type.objectName) && (right_type.expType == ExpType::Object 
                                                    || right_type.expType == ExpType::Array 
                                                    || right_type.expType == ExpType::String 
                                                    || right_type.expType == ExpType::Null)) {
            std::string obj{"Object"};
            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(obj, true));
            ambiguousName.typeNode = std::make_shared<IdentifierType>(std::make_shared<Identifier>(obj), true);
            return ambiguousName;
        }
        if ((checkIsSameSymbolTable("java.lang.Cloneable", left_type.objectName)
            || checkIsSameSymbolTable("java.io.Serializable", left_type.objectName)) && right_type.expType == ExpType::Array)
            return ambiguousName;
        // Null can be assigned to any reference type
        else if (right_type.expType == ExpType::Null) return ambiguousName;
        else if(right_type.expType == ExpType::Object && checkIsSubclassByName(left_type.objectName, right_type.objectName)) {
            bool simple = std::find(left_type.objectName.begin(), left_type.objectName.end(), '.') == left_type.objectName.end();
            ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(left_type.objectName, simple));
            ambiguousName.typeNode = std::make_shared<IdentifierType>(std::make_shared<Identifier>(left_type.objectName), simple);
            return ambiguousName;
        }
        else {
            error = true;
            std::cerr << "Error: Invalid Assignment Type: Invalid Assignment to Object" << std::endl;
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;
        }
    }
    else if (left_type.expType == ExpType::Array) {
        if (right_type.expType == ExpType::Array) {
            if (left_type.arrayType == DataType::OBJECT && right_type.arrayType == DataType::OBJECT) {
              if (checkIsSameSymbolTable("java.lang.Object", left_type.objectName) || checkIsSubclassByName(left_type.objectName, right_type.objectName)) 
              {
                bool simple = std::find(left_type.objectName.begin(), left_type.objectName.end(), '.') == left_type.objectName.end();
                ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(left_type.objectName, simple));
                ambiguousName.typeNode = std::make_shared<ArrayType>(std::make_shared<IdentifierType>(std::make_shared<Identifier>(left_type.objectName), simple));
                return ambiguousName;
              }
            }
            else if (left_type.arrayType == right_type.arrayType) {
                ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
                if (left_type.arrayType == DataType::INT) {
                    ambiguousName.typeNode = std::make_shared<ArrayType>(std::make_shared<IntType>());
                }
                return ambiguousName;
            }
        }
        else if (right_type.expType == ExpType::Null) return ambiguousName;
        error = true;
        std::cerr << "Error: Invalid Assignment Type: Invalid Assignment to Array" << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    else if (!assginmentRules.contains({left_type.expType, right_type.expType})) {
        error = true;
        std::cerr << "Error: Invalid Assignment Type: " << expTypeString[(int)left_type.expType] << " <=== " << expTypeString[(int)right_type.expType] << std::endl; 
        ambiguousName.type = AmbiguousNamesType::ERROR;

    }
    return ambiguousName;
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
        arguments.emplace_back(currentExpInfo);
    }
    auto constructor = getClosestMatchConstructor(constructors, arguments);
    if (!constructor) {
        std::cerr << "Error: No matching constructor found for " << cname << std::endl;
        error = true;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    currentExpInfo.expType = ExpType::Object;
    currentExpInfo.objectName = cname;
    n->constructor = constructor;
    n->exprs.emplace_back(Expression::LOCAL, "tmp", ambiguousName.symbolTable);
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitIdentifierExp(std::shared_ptr<IdentifierExp> n) {
    const std::string key {n->id->name};
    auto ambiguousName = scope->reclassifyAmbiguousName(key, n->simple, &(n->exprs));
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
    auto expInfo = GetExpInfo(n->exp);
    
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
    if (!castingRules.contains({castType, expInfo.expType})) {
        error = true;
        std::cerr << "Error: Undefined casting " << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }
    if (castType == ExpType::Object || castType == ExpType::String) {
        if (!checkIsSubclassByName(expInfo.objectName, castObjectTypeName) && !checkIsSubclassByName(castObjectTypeName, expInfo.objectName)) {
            error = true;
            std::cerr << "Error: Cast to incompatible type" << std::endl;
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;            
        }
    }
    
    currentExpInfo = argumentExp {castType, castObjectTypeName, castArrayDataType};

    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitParExp(std::shared_ptr<ParExp> n) {
    if (auto ie = std::dynamic_pointer_cast<IdentifierExp>(n->exp)) return visitIdentifierExp(ie);
    if (auto ce = std::dynamic_pointer_cast<CastExp>(n->exp)) return visitCastExp(ce);
    if (auto me = std::dynamic_pointer_cast<MethodInvocation>(n->exp)) return visitMethodInvocation(me);
    if (auto cice = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->exp)) return visitClassInstanceCreationExp(cice);
    if (auto as = std::dynamic_pointer_cast<Assignment>(n->exp)) return visitAssignment(as);
    currentExpInfo = GetExpInfo(n->exp);
    if (currentExpInfo.expType == ExpType::String)
        return AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->onDemandImported["String"]);
    return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
}

AmbiguousName TypeCheckingVisitor::visitThisExp(std::shared_ptr<ThisExp> n) {
    if (scope->current->getScopeType(ScopeType::STATIC)) {
        std::cerr << "Error: Cannot use this in a static method" << std::endl;
        error = true;
        return AmbiguousName(AmbiguousNamesType::ERROR, nullptr);
    }
    currentExpInfo.expType = ExpType::Object;
    currentExpInfo.objectName = scope->current->getAst()->classOrInterfaceDecl->id->name;
    return AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->current);  
}

AmbiguousName TypeCheckingVisitor::visitFieldAccessExp(std::shared_ptr<FieldAccessExp> n) {
    AmbiguousName ambiguousName;
    std::string fieldName{n->field->id->name};
    if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->exp)) {
        ambiguousName = visitThisExp(thisExp);
        n->exprs.emplace_back(Expression::LOCAL, "this", scope->current);
    }
    else if (auto parExp = std::dynamic_pointer_cast<ParExp>(n->exp)) ambiguousName = visitParExp(parExp);
    else if (auto classexp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->exp)) {
        ambiguousName = visitClassInstanceCreationExp(classexp);
        n->exprs = classexp->exprs;
    }
    else if (auto fieldexp = std::dynamic_pointer_cast<FieldAccessExp>(n->exp)) {
        ambiguousName = visitFieldAccessExp(fieldexp);
        n->exprs = fieldexp->exprs;
    }
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
        currentExpInfo.expType = ExpType::Integer;
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
    n->exprs.emplace_back(Expression::NON_STATIC_FIELD, fieldName, fieldSt);
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitMethodInvocation(std::shared_ptr<MethodInvocation> n) {
    AmbiguousName ambiguousName;
    std::shared_ptr<Method> method;
    std::string methodName;
    std::vector<argumentExp> arguments;
    for (auto& arg : n->arguments) {
        arg->accept(this);
        arguments.emplace_back(currentExpInfo);
    }
    //primary method invocation
    if (n->primary) {
        methodName = n->primaryMethodName->id->name;
        if (auto thisExp = std::dynamic_pointer_cast<ThisExp>(n->primary)) {
            ambiguousName = visitThisExp(thisExp);
            n->exprs.emplace_back(Expression::LOCAL, "this", scope->current);
        }
        else if (auto classExp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(n->primary)) {
            ambiguousName = visitClassInstanceCreationExp(classExp);
            n->exprs = classExp->exprs;
        }
        else if (auto parexp = std::dynamic_pointer_cast<ParExp>(n->primary)) ambiguousName = visitParExp(parexp);
        else if (auto fieldexp = std::dynamic_pointer_cast<FieldAccessExp>(n->primary)) {
            ambiguousName = visitFieldAccessExp(fieldexp);
            n->exprs = fieldexp->exprs;
        }
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
            ambiguousName = scope->reclassifyAmbiguousName(n->ambiguousName->id->name, n->ambiguousName->simple, &(n->exprs));
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
    if (method) {
        SetCurrentExpTypebyAmbiguousName(method->type);
        n->method = method;
    }
    else currentExpInfo.expType = ExpType::Any;

    std::shared_ptr<SymbolTable> methodSt;
    auto ie = std::dynamic_pointer_cast<IdentifierType>(method->type);
    if (ie) methodSt = ambiguousName.symbolTable->getScope()->getNameInScope(ie->id->name, ie->simple);
    ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, methodSt);
    ambiguousName.typeNode = method->type;
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitStringLiteralExp(std::shared_ptr<StringLiteralExp> n) {
    currentExpInfo.expType = ExpType::String;
    currentExpInfo.objectName = "String";
    AmbiguousName ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, scope->getNameInScope(currentExpInfo.objectName, true));
    ambiguousName.typeNode = std::make_shared<IdentifierType>(std::make_shared<Identifier>(currentExpInfo.objectName), true);
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitNewArrayExp(std::shared_ptr<NewArrayExp> n) {
    AmbiguousName ambiguousName;
    auto indexTypeInfo = GetExpInfo(n->exp);
    std::set<ExpType> s { ExpType::Integer, ExpType::Short, ExpType::Char, ExpType::Byte};
    if (!s.contains(indexTypeInfo.expType)) {
        error = true;
        std::cerr << "Error: Array index must have numeric type" << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }

    currentExpInfo.expType = ExpType::Array;
    currentExpInfo.arrayType = n->type->dataType->type;
    if(auto type = std::dynamic_pointer_cast<IdentifierType>(n->type->dataType)){
        currentExpInfo.objectName = type->id->name;
    }

    ambiguousName = AmbiguousName(AmbiguousNamesType::EXPRESSION, nullptr);
    ambiguousName.typeNode = n->type;
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitArrayAccessExp(std::shared_ptr<ArrayAccessExp> n) {
    AmbiguousName ambiguousName;
    // not finished yet

    auto indexType = GetExpInfo(n->index);
    std::set<ExpType> s { ExpType::Integer, ExpType::Short, ExpType::Char, ExpType::Byte};
    if (!s.contains(indexType.expType)) {
        error = true;
        std::cerr << "Error: Array index must have numeric type" << std::endl;
        ambiguousName.type = AmbiguousNamesType::ERROR;
        return ambiguousName;
    }


    auto processExpType = [&](auto& ambiguousName, auto& currentExpInfo, auto& d2e, bool& error) {
        SetCurrentExpTypebyAmbiguousName(ambiguousName.typeNode);
        if (currentExpInfo.arrayType == DataType::OBJECT) {
            currentExpInfo.expType = ExpType::Object;
            if (currentExpInfo.objectName == "String")
                currentExpInfo.expType = ExpType::String;
        }   
        else if (d2e.contains(currentExpInfo.arrayType))
            currentExpInfo.expType = d2e[currentExpInfo.arrayType];
        else {
            error = true;
            std::cerr << "Error: Undefined Conversion from DataType to ExpType" << std::endl;
            ambiguousName.type = AmbiguousNamesType::ERROR;
            return ambiguousName;
        }
        return ambiguousName;
    };

    if (auto left = std::dynamic_pointer_cast<IdentifierExp>(n->array)) {
        ambiguousName = scope->reclassifyAmbiguousName(left->id->name, left->simple, &(left->exprs));
        return processExpType(ambiguousName, currentExpInfo, d2e, error);
    } 
    else if (auto left = std::dynamic_pointer_cast<ParExp>(n->array)) {
        ambiguousName = visitParExp(left);
        return processExpType(ambiguousName, currentExpInfo, d2e, error);
    }
    else {
        currentExpInfo.expType = ExpType::Any;
    }    
    return ambiguousName;
}

AmbiguousName TypeCheckingVisitor::visitAssignment(std::shared_ptr<Assignment> n) {
    AmbiguousName ambiguousName;
    auto right_exp = n->right;
    auto casted_right_exp = std::dynamic_pointer_cast<ClassInstanceCreationExp>(right_exp);
    if (casted_right_exp) {
        visit(casted_right_exp);
    }
    if (auto ie = std::dynamic_pointer_cast<IdentifierExp>(n->left)) {
        scope->current->beginScope(ScopeType::ASSIGNMENT);
    }
    auto left_type = GetExpInfo(n->left);
    scope->current->endScope(ScopeType::ASSIGNMENT);
        
    auto right_type = GetExpInfo(n->right);

    ambiguousName = AssignmentTypeCheckingLogic(left_type, right_type);
    currentExpInfo = left_type;
    return ambiguousName;
}

bool TypeCheckingVisitor::checkIsSubclassByName(std::string o1, std::string o2) {
    auto getTable = [&](const std::string& o) {
        return scope->packageMembers.contains(o) ? scope->packageMembers[o] :
            scope->onDemandImported.contains(o) ? scope->onDemandImported[o] :
            scope->singleImported.contains(o) ? scope->singleImported[o] : nullptr;
    };
    auto inSupers = [](const std::shared_ptr<SymbolTable> table, const std::vector<std::shared_ptr<SymbolTable>>& supers) {
        return std::find(supers.begin(), supers.end(), table) != supers.end();
    };

    auto objTable = getSymbolTableByClassName("java.lang.Object");
    auto o1Table = getTable(o1);
    auto o2Table = getTable(o2);
    if(o2Table == objTable) return true;
    if(o1Table == o2Table) return true;
    // auto o1Supers = o1Table->getScope()->supers;
    auto o2Supers = o2Table->getScope()->supers;
    
    return inSupers(o1Table, o2Supers);
}


template<typename BinOpExp>
void TypeCheckingVisitor::visitBinaryOpExp(std::shared_ptr<BinOpExp> n, ExpRuleType rule_type1, ExpRuleType rule_type2) {
    auto t1 = GetExpInfo(n->exp1);
    auto t2 = GetExpInfo(n->exp2);
    if (typeid(BinOpExp) == typeid(ConditionalAndExp) 
        || typeid(BinOpExp) == typeid(ConditionalOrExp)
        || typeid(BinOpExp) == typeid(AndExp)
        || typeid(BinOpExp) == typeid(OrExp)
        || typeid(BinOpExp) == typeid(XorExp)
        ) {
        if (t1.expType == ExpType::Any || t2.expType == ExpType::Any) return;
        if (t1.expType != ExpType::Boolean || t2.expType != ExpType::Boolean) {
            error = true;
            std::cerr << "Error: Invalid " << typeid(BinOpExp).name() << " Type" << std::endl;
        }
        currentExpInfo.expType = ExpType::Boolean;
    }
    else {
        currentExpInfo.expType = CalcExpType(rule_type1, t1.expType, t2.expType);
        if (currentExpInfo.expType == ExpType::Undefined && rule_type2 != ExpRuleType::Undefined)
            currentExpInfo.expType = CalcExpType(rule_type2, t1.expType, t2.expType);
        if (currentExpInfo.expType == ExpType::Undefined) {
            error = true;
            std::cerr << "Error: Invalid " << typeid(BinOpExp).name() << " Type" << std::endl;
        }
        if (currentExpInfo.expType == ExpType::String) {
            currentExpInfo.objectName = "String";
        }
    }
}

std::shared_ptr<SymbolTable> TypeCheckingVisitor::getSymbolTableByClassName(std::string name) {
    auto ret = scope->getNameInScope(name, true);
    if (ret == nullptr) ret = scope->getNameInScope(name, false);
    return ret;
}

bool TypeCheckingVisitor::checkIsSameSymbolTable(std::string o1, std::string o2) {
    auto left = getSymbolTableByClassName(o1);
    auto right = getSymbolTableByClassName(o2);
    if(left == right && left == nullptr) return false;
    return (left == right);
}
