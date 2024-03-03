#include <unordered_set>
#include <cassert>
#include <type_traits>
#include "hierarchyVisitor.hpp"

HierarchyVisitor::HierarchyVisitor(std::shared_ptr<Scope> s) : scope(s), error(false) {
}

bool HierarchyVisitor::isError() const {
    return error;
}


void HierarchyVisitor::visit(std::shared_ptr<Program> n) {
    n->classOrInterfaceDecl->accept(this);
}

Modifiers stringToModifier(const std::string& str) {
    if (str == "public") return Modifiers::PUBLIC;
    else if (str == "protected") return Modifiers::PROTECTED;
    else if (str == "final") return Modifiers::FINAL;
    else if (str == "abstract") return Modifiers::ABSTRACT;
    else if (str == "static") return Modifiers::STATIC;
    else if (str == "native") return Modifiers::NATIVE;
    // else throw std::invalid_argument("Unknown modifier: " + str);
}

bool checkAbstractClass(std::shared_ptr<ClassDecl> n, std::shared_ptr<Scope> scope) {
    if (scope->current->getPackage() == "java.lang" || scope->current->getPackage() == "java.util" || scope->current->getPackage() == "java.io") {
        return true;
    }
    // Check if the class itself declares any abstract methods
    for (auto decl : n->declarations[1]) {
        auto method = std::dynamic_pointer_cast<Method>(decl);
        if (method) {
            for (auto modifier : method->modifiers) {
                if (modifier == Modifiers::ABSTRACT && (stringToModifier(n->modifier) != Modifiers::ABSTRACT)) {
                    std::cerr << "Error: Class " << n->id->name << " declares abstract method but is not declared as abstract" << std::endl;
                    return false;
                }
            }
        }
    }

    // If the class is abstract, it is allowed not to implement any methods from an interface
    if (n->modifier == "abstract") {
        return true;
    }

    // Check if the class inherits any abstract methods
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            std::shared_ptr<ClassDecl> parentClassDecl = std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl);
            if (parentClassDecl) {
                for (auto parentDecl : parentClassDecl->declarations[1]) {
                    auto parentMethod = std::dynamic_pointer_cast<Method>(parentDecl);
                    if (parentMethod) {
                        for (auto modifier : parentMethod->modifiers) {
                            if (modifier == Modifiers::ABSTRACT) {
                                bool isImplemented = false;
                                for (auto decl : n->declarations[1]) {
                                    auto method = std::dynamic_pointer_cast<Method>(decl);
                                    if (method && method->methodName->name == parentMethod->methodName->name) {
                                        isImplemented = true;
                                        break;
                                    }
                                }
                                if (!isImplemented) {
                                    std::cerr << "Error: Class " << n->id->name << " does not implement abstract method " << parentMethod->methodName->name << " from superclass" << std::endl;
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool checkSuperclassesForMethod(std::shared_ptr<ClassDecl> n, std::shared_ptr<Scope> scope, std::shared_ptr<Method> interfaceMethod) {
    for (auto decl : n->declarations[1]) {
        auto method = std::dynamic_pointer_cast<Method>(decl);
        if (method && method->methodName->name == interfaceMethod->methodName->name) {
            return true;
        }
    }

    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            std::shared_ptr<ClassDecl> parentClassDecl = std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl);
            if (parentClassDecl && checkSuperclassesForMethod(parentClassDecl, scope, interfaceMethod)) {
                return true;
            }
        }
    }

    return false;
}

bool checkInterfaceImplementation(std::shared_ptr<ClassDecl> n, std::shared_ptr<Scope> scope) {
    // If the class is abstract, it is allowed not to implement any methods from an interface
    if (n->modifier == "abstract") {
        return true;
    }

    // Check if the class or its superclasses implement all methods from its interfaces
    for (std::shared_ptr<IdentifierType> impl : n->implemented) {
        if (auto st = scope->getNameInScope(impl->id->name, impl->simple)) {
            std::shared_ptr<InterfaceDecl> interfaceDecl = std::dynamic_pointer_cast<InterfaceDecl>(st->getAst()->classOrInterfaceDecl);
            if (interfaceDecl) {
                for (auto interfaceMethod : interfaceDecl->methods) {
                    if (!checkSuperclassesForMethod(n, scope, interfaceMethod)) {
                        std::cerr << "Error: Class " << n->id->name << " and its superclasses do not implement method " << interfaceMethod->methodName->name << " from interface" << std::endl;
                        return false;
                    }
                }
            }
        }
    }

    // Check if the class that extends an abstract class implements all the abstract methods from the interfaces that the abstract class implements
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            std::shared_ptr<ClassDecl> parentClassDecl = std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl);
            if (parentClassDecl && parentClassDecl->modifier == "abstract") {
                for (std::shared_ptr<IdentifierType> impl : parentClassDecl->implemented) {
                    if (auto st = scope->getNameInScope(impl->id->name, impl->simple)) {
                        std::shared_ptr<InterfaceDecl> interfaceDecl = std::dynamic_pointer_cast<InterfaceDecl>(st->getAst()->classOrInterfaceDecl);
                        if (interfaceDecl) {
                            for (auto interfaceMethod : interfaceDecl->methods) {
                                bool isImplemented = false;
                                for (auto decl : n->declarations[1]) {
                                    auto method = std::dynamic_pointer_cast<Method>(decl);
                                    if (method && method->methodName->name == interfaceMethod->methodName->name) {
                                        isImplemented = true;
                                        break;
                                    }
                                }
                                if (!isImplemented) {
                                    std::cerr << "Error: Class " << n->id->name << " does not implement abstract method " << interfaceMethod->methodName->name << " from interface implemented by abstract superclass" << std::endl;
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

struct VectorStringHash {
    size_t operator()(const std::vector<std::string>& v) const {
        std::hash<std::string> hasher;
        size_t seed = 0;
        for (std::string s : v) {
            seed ^= hasher(s) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};
struct VectorDataTypeHash {
    size_t operator()(const std::vector<DataType>& v) const {
        std::hash<DataType> hasher;
        size_t seed = 0;
        for (DataType d : v) {
            seed ^= hasher(d) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};
std::unordered_map<DataType, DataType> arrayDataTypes = {
    {DataType::VOID, DataType::VOIDARRAY}, {DataType::INT, DataType::INTARRAY}, {DataType::BOOLEAN, DataType::BOOLEANARRAY},
    {DataType::CHAR, DataType::CHARARRAY}, {DataType::BYTE, DataType::BYTEARRAY}, {DataType::SHORT, DataType::SHORTARRAY},
    {DataType::LONG, DataType::LONGARRAY}, {DataType::FLOAT, DataType::FLOATARRAY}, {DataType::DOUBLE, DataType::DOUBLEARRAY},
    {DataType::OBJECT, DataType::OBJECTARRAY}
};

//Requires: T is one of Method, Constructor
template <typename T>
std::string hierarchyRuleSevenEight(std::vector<std::shared_ptr<T>>& methodsOrConstructors) {
    const bool isMethod = std::is_same<T, Method>::value;
    const bool isConstructor = std::is_same<T, Constructor>::value;
    static_assert(isMethod == true || isConstructor == true, "T must be either Method or Constructor");

    using hashableStringVectorSet = std::unordered_set<std::vector<std::string>, VectorStringHash>; 
    using hashableDataTypeVectorMap =  std::unordered_map<std::vector<DataType>, hashableStringVectorSet, VectorDataTypeHash>;
    std::unordered_map<std::string, hashableDataTypeVectorMap> alreadyDeclared; //maybe replace with Boost multiindex
    for (auto methodOrConstructor : methodsOrConstructors) {
        //create indexing key and value
        std::string methodOrConstructorName;
        std::vector<DataType> paramTypes{};
        std::vector<std::string> objectNames{};
        //create methodOrConstructorName
        if constexpr (std::is_same<T, Method>::value) methodOrConstructorName = methodOrConstructor->methodName->name;
        else if constexpr (std::is_same<T, Constructor>::value) methodOrConstructorName = methodOrConstructor->constructorName->name;
        //populate paramTypes and objectNames. Special cases for ARRAY and/or OBJECT
        for (auto fp : methodOrConstructor->formalParameters) {
            DataType d = fp->type->type;
            if (d == DataType::ARRAY) {
                std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(fp->type);
                assert(arrayType != nullptr);
                d = arrayDataTypes[arrayType->dataType->type];
                if (d == DataType::OBJECTARRAY) {
                    std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(arrayType->dataType);
                    assert(idType != nullptr);
                    objectNames.push_back(idType->id->name);
                }
            } else if (d == DataType::OBJECT) {
                std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(fp->type);
                assert(idType != nullptr);
                objectNames.push_back(idType->id->name);
            }
            paramTypes.push_back(d);
        }
        //check for repeated methods/constructors
        if (alreadyDeclared.contains(methodOrConstructorName) && alreadyDeclared[methodOrConstructorName].contains(paramTypes) 
        && alreadyDeclared[methodOrConstructorName][paramTypes].contains(objectNames)) {
            return methodOrConstructorName;
        }
        alreadyDeclared[methodOrConstructorName][paramTypes].insert(objectNames);
    }
    return "";
}

// Custom hash function for shared_ptr<A>
struct SharedPtrHash {
    size_t operator()(const std::shared_ptr<SymbolTable>& ptr) const {
        return std::hash<SymbolTable*>()(ptr.get());
    }
};

// Custom equality function for shared_ptr<A>
struct SharedPtrEqual {
    bool operator()(const std::shared_ptr<SymbolTable>& lhs, const std::shared_ptr<SymbolTable>& rhs) const {
        return lhs.get() == rhs.get();
    }
};

void HierarchyVisitor::visit(std::shared_ptr<ClassDecl> n) {
    //Rule #1: Class must not extend an interface
    //Rule #4: Class must not extend final class
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            //Rule #1
            if (std::dynamic_pointer_cast<InterfaceDecl>(st->getAst()->classOrInterfaceDecl)) {
                std::cerr << "Error: Class " << n->id->name << " extends Interface " << ext->id->name << std::endl;
                error = true;
                return;
            }
            //Rule #4
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl);
            assert(classDecl != nullptr); //Rule #1 already ensures that you can't extend an interface, so this cast should succeed
            if (classDecl->modifier == "final") {
                std::cerr << "Error: Class " << n->id->name << " extends final Class " << classDecl->id->name << std::endl;
                error = true;
                return;
            }

        }
    }
    //Rule #2: Class must not implement a class
    //Rule #3: An interface must not be repeated in a class's implements clause
    std::unordered_set<std::shared_ptr<SymbolTable>, SharedPtrHash, SharedPtrEqual> newImplementedInterfaces;
    for (std::shared_ptr<IdentifierType> impl : n->implemented) {
        //Rule #2
        if (auto st = scope->getNameInScope(impl->id->name, impl->simple)) {
            if (std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl)) {
                std::cerr << "Error: Class " << n->id->name << " implements Class " << impl->id->name << std::endl;
                error = true;
                return;
            }
        }
        //Rule #3
        if (newImplementedInterfaces.contains(scope->getNameInScope(impl->id->name, impl->simple))) {
            std::cerr << "Error: Class " << n->id->name << " implements Interface " << impl->id->name << " more than once" << std::endl;
            error = true;
            return;
        } else {
            newImplementedInterfaces.insert(scope->getNameInScope(impl->id->name, impl->simple));
        }
    }
    //Rule #7: Class must not declare two methods with the same name and parameter types
    std::vector<std::shared_ptr<Method>> methodVector = {};
    for (auto decl : n->declarations[1]) methodVector.push_back(std::dynamic_pointer_cast<Method>(decl));
    std::string repeatedMethodSignature = hierarchyRuleSevenEight<Method>(methodVector);
    if (repeatedMethodSignature != "") {
        std::cerr << "Error: Class " << n->id->name << " declares multiple versions of method \"" << repeatedMethodSignature << "\" with the same parameter types" << std::endl;
        error = true;
        return;
    }
    //Rule #8: Class must not declare two constructors with the same parameter types
    std::vector<std::shared_ptr<Constructor>> constructorVector = {};
    for (auto decl : n->declarations[2]) constructorVector.push_back(std::dynamic_pointer_cast<Constructor>(decl));
    std::string repeatedConstructorSignature = hierarchyRuleSevenEight<Constructor>(constructorVector);
    if (repeatedConstructorSignature != "") {
        std::cerr << "Error: Class " << n->id->name << " declares multiple versions of constructor \"" << repeatedConstructorSignature << "\" with the same parameter types" << std::endl;
        error = true;
        return;
    }

    // Rule #10:
    if (!checkAbstractClass(n, scope) || !checkInterfaceImplementation(n, scope)) {
        error = true;
        return;
    }

    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            std::shared_ptr<ClassDecl> parentClassDecl = std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl);
            if (parentClassDecl) {
                // Check if any of the methods in the subclass are protected and override a public method from the superclass
                for (auto decl : n->declarations[1]) {
                    auto method = std::dynamic_pointer_cast<Method>(decl);
                    if (method && !method->isPublic()) {
                        for (auto parentDecl : parentClassDecl->declarations[1]) {
                            auto parentMethod = std::dynamic_pointer_cast<Method>(parentDecl);
                            if (parentMethod && parentMethod->methodName->name == method->methodName->name && parentMethod->isPublic()) {
                                // Check if the parameter types are the same
                                if (parentMethod->formalParameters.size() == method->formalParameters.size()) {
                                    bool isSameParameters = true;
                                    for (int i = 0; i < parentMethod->formalParameters.size(); i++) {
                                        if (parentMethod->formalParameters[i]->type->type != method->formalParameters[i]->type->type) {
                                            isSameParameters = false;
                                            break;
                                        }
                                    }
                                    if (isSameParameters) {
                                        std::cerr << "Error: Protected method " << method->methodName->name << " in class " << n->id->name << " cannot override public method from superclass" << std::endl;
                                        error = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            std::shared_ptr<ClassDecl> parentClassDecl = std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl);
            if (parentClassDecl) {
                // Check if any of the methods in the subclass are instance methods and override a static method from the superclass
                for (auto decl : n->declarations[1]) {
                    auto method = std::dynamic_pointer_cast<Method>(decl);
                    if (method && !method->isStatic()) {
                        for (auto parentDecl : parentClassDecl->declarations[1]) {
                            auto parentMethod = std::dynamic_pointer_cast<Method>(parentDecl);
                            if (parentMethod && parentMethod->methodName->name == method->methodName->name && parentMethod->isStatic()) {
                                // Check if the parameter types are the same
                                if (parentMethod->formalParameters.size() == method->formalParameters.size()) {
                                    bool isSameParameters = true;
                                    for (int i = 0; i < parentMethod->formalParameters.size(); i++) {
                                        if (parentMethod->formalParameters[i]->type->type != method->formalParameters[i]->type->type) {
                                            isSameParameters = false;
                                            break;
                                        }
                                    }
                                    if (isSameParameters) {
                                        std::cerr << "Error: Instance method " << method->methodName->name << " in class " << n->id->name << " cannot override static method from superclass" << std::endl;
                                        error = true;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (std::shared_ptr<MemberDecl> m : n->declarations[1]) {
        m->accept(this);
    }
}

void HierarchyVisitor::visit(std::shared_ptr<InterfaceDecl> n) {
    //Rule #3: Interfaces must not be repeated in an interface's extends clause
    //Rule #5: Interface must not extend a class
    std::unordered_set<std::shared_ptr<SymbolTable>, SharedPtrHash, SharedPtrEqual> newExtendedInterfaces;
    for (std::shared_ptr<IdentifierType> ext : n->extended) {
        //Rule #5
        if (auto st = scope->getNameInScope(ext->id->name, ext->simple)) {
            //try to cast to ClassDecl
            if (std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl)) {
                std::cerr << "Error: Interface " << n->id->name << " extends Class " << ext->id->name << std::endl;
                error = true;
                return; 
            }
        }
        //Rule #3
        if (newExtendedInterfaces.contains(scope->getNameInScope(ext->id->name, ext->simple))) {
            std::cerr << "Error: Class " << n->id->name << " implements Interface " << ext->id->name << " more than once" << std::endl;
            error = true;
            return;
        } else {
            newExtendedInterfaces.insert(scope->getNameInScope(ext->id->name, ext->simple));
        }
    }
    //Rule #7: Interface must not declare two methods with the same name and parameter types
    std::string repeatedMethodSignature = hierarchyRuleSevenEight<Method>(n->methods);
    if (repeatedMethodSignature != "") {
        std::cerr << "Error: Interface " << n->id->name << " declares multiple versions of method \"" << repeatedMethodSignature << "\" with the same parameter types" << std::endl;
        error = true;
        return;
    }

    for (std::shared_ptr<MemberDecl> m : n->methods) {
        m->accept(this);
    }
}

void HierarchyVisitor::visit(std::shared_ptr<Method> n) 
{
    const std::string key {n->methodName->name};

    if (scope->current->getPackage() == "java.lang" || scope->current->getPackage() == "java.util" || scope->current->getPackage() == "java.io")
    {
        return;
    }

    auto processContainer = [&](const auto& container)
    {
        for (const auto& entry : container) {
            const auto& symbolTableHere = entry.second;
            if (symbolTableHere->getMethod(key) && entry.first != scope->current->getPackage()) {
                for (const auto& node : symbolTableHere->mtable[key]) {
                    auto methodNode = std::dynamic_pointer_cast<Method>(node);
                    if (!methodNode) continue; // Ensure methodNode is valid before accessing its members
                    // Rule 13
                    if (!Type::isSameType(methodNode->returnType, n->returnType)) {
                        std::cerr << "Error: Return type of " << key << " in supermethod and current method is not the same." << std::endl;
                        error = true;
                        break;
                    }
                    // Rule 15
                    if (methodNode->isFinal()) {
                        std::cerr << "Error: Method " << key << " can not override final method" << std::endl;
                        error = true;
                        break;
                    }
                    // Rule 14
                    if (methodNode->isPublic() && !n->isPublic())
                    {
                        std::cerr << "Error: PROTECTED Method " << key << " can not override PUBLIC method" << std::endl;
                        error = true;
                        break;
                    }
                }
            }
        }
    };

    processContainer(scope->onDemandImported);
    processContainer(scope->singleImported);
    for(auto& symbolTable : scope->supers) {
        if (symbolTable != nullptr && symbolTable->getMethod(key)) {
            for (const auto& node : symbolTable->mtable[key]) {
                auto methodNode = std::dynamic_pointer_cast<Method>(node);
                if (!methodNode) continue; // Ensure methodNode is valid before accessing its members
                // Rule 13
                if (!Type::isSameType(methodNode->returnType, n->returnType)) {
                    std::cerr << "Error: Return type of " << key << " in supermethod and current method is not the same." << std::endl;
                    error = true;
                    break;
                }
            }
        }
    }
    Visitor::visit(n);
}