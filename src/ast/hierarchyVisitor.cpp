#include <unordered_set>
#include <cassert>
#include "hierarchyVisitor.hpp"

HierarchyVisitor::HierarchyVisitor(std::shared_ptr<Scope> s) : scope(s), error(false) {
}

bool HierarchyVisitor::isError() const {
    return error;
}


void HierarchyVisitor::visit(std::shared_ptr<Program> n) {
    n->classOrInterfaceDecl->accept(this);
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
    std::unordered_set<std::string> implementedInterfaces;
    for (std::shared_ptr<IdentifierType> impl : n->implemented) {
        //Rule #2
        if (auto st = scope->getNameInScope(impl->id->name, impl->simple)) {
            //cast to ClassDecl
            if (std::dynamic_pointer_cast<ClassDecl>(st->getAst()->classOrInterfaceDecl)) {
                std::cerr << "Error: Class " << n->id->name << " implements Class " << impl->id->name << std::endl;
                error = true;
                return;
            }
        }
        //Rule #3
        if (implementedInterfaces.contains(impl->id->name)) {
            std::cerr << "Error: Class " << n->id->name << " implements Interface " << impl->id->name << " more than once" << std::endl;
            error = true;
            return;    
        } else {
            implementedInterfaces.insert(impl->id->name);
        }
    }
    //Rule #7: Class must not declare two methods with the same name and parameter types
    using hashableStringVectorSet = std::unordered_set<std::vector<std::string>, VectorStringHash>; 
    using hashableDataTypeVectorMap =  std::unordered_map<std::vector<DataType>, hashableStringVectorSet, VectorDataTypeHash>;
    std::unordered_map<std::string, hashableDataTypeVectorMap> methods; //maybe replace with Boost multiindex
    for (auto decl : n->declarations[1]) {
        //cast to Method ptr
        std::shared_ptr<Method> method = std::dynamic_pointer_cast<Method>(decl);
        assert(method != nullptr);
        //create indexing key and value
        std::string methodName = method->methodName->name;
        std::vector<DataType> paramTypes{};
        std::vector<std::string> objectNames{};
        for (auto fp : method->formalParameters) {
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
        //check for repeated methods
        if (methods.contains(methodName) && methods[methodName].contains(paramTypes) && methods[methodName][paramTypes].contains(objectNames)) {
            std::cerr << "Error: Class " << n->id->name << " declares two versions of method \"" << methodName << "\" with the same parameter types" << std::endl;
            error = true;
            return;
        }
        methods[methodName][paramTypes].insert(objectNames);
    }
}

void HierarchyVisitor::visit(std::shared_ptr<InterfaceDecl> n) {

    //Rule #3: Interfaces must not be repeated in an interface's extends clause
    //Rule #5: Interface must not extend a class
    std::unordered_set<std::string> extendedInterfaces;
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
        if (extendedInterfaces.contains(ext->id->name)) {
            std::cerr << "Error: Interface " << n->id->name << " extends Interface " << ext->id->name << " more than once" << std::endl;
            error = true;
            return; 
        } else {
            extendedInterfaces.insert(ext->id->name);
        }
    }
    //Rule #7: Interface must not declare two methods with the same name and parameter types
    using hashableStringVectorSet = std::unordered_set<std::vector<std::string>, VectorStringHash>; 
    using hashableDataTypeVectorMap =  std::unordered_map<std::vector<DataType>, hashableStringVectorSet, VectorDataTypeHash>;
    std::unordered_map<std::string, hashableDataTypeVectorMap> methods; //maybe replace with Boost multiindex
    for (auto method : n->methods) {
        //create indexing key and value
        std::string methodName = method->methodName->name;
        std::vector<DataType> paramTypes{};
        std::vector<std::string> objectNames{};
        for (auto fp : method->formalParameters) {
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
        //check for repeated methods
        if (methods.contains(methodName) && methods[methodName].contains(paramTypes) && methods[methodName][paramTypes].contains(objectNames)) {
            std::cerr << "Error: Interface " << n->id->name << " declares two versions of method \"" << methodName << "\" with the same parameter types" << std::endl;
            error = true;
            return;
        }
        methods[methodName][paramTypes].insert(objectNames);
    }
}