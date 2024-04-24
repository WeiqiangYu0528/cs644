#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "PackageTrie.hpp"
#include "parser.h"
#include "contextVisitor.hpp"
#include "symbolTable.hpp"
#include "typeCheckingVisitor.hpp"
#include "typeLinkingVisitor.hpp"
#include "hierarchyVisitor.hpp"
#include "transformVisitor.hpp"
#include "IRAst.hpp"
#include "Simulator.hpp"
#include "canonicalVisitor.hpp"
#include "utils.h"
#include "IRCfgVisitor.hpp"
#include <Tiling.hpp>
#include "cfgVisitor.hpp"
#include "subtypeTable.hpp"

std::unordered_map<DataType, DataType> arrayDataTypes = {
    {DataType::VOID, DataType::VOIDARRAY}, {DataType::INT, DataType::INTARRAY}, {DataType::BOOLEAN, DataType::BOOLEANARRAY}, {DataType::CHAR, DataType::CHARARRAY}, {DataType::BYTE, DataType::BYTEARRAY}, {DataType::SHORT, DataType::SHORTARRAY}, {DataType::LONG, DataType::LONGARRAY}, {DataType::FLOAT, DataType::FLOATARRAY}, {DataType::DOUBLE, DataType::DOUBLEARRAY}, {DataType::OBJECT, DataType::OBJECTARRAY}};

std::string extractFilename(const std::string &filePath)
{
    size_t start = filePath.rfind('/') + 1; // Find the last '/' character
    size_t end = filePath.rfind('.');       // Find the last '.' character
    if (start == std::string::npos || end == std::string::npos || end <= start)
    {
        return ""; // Return an empty string if the format is not as expected
    }
    return filePath.substr(start, end - start); // Extract the substring representing the filename
}

bool DFS(std::string current, std::unordered_set<std::string> &visited, std::unordered_set<std::string> &pathVisited,
         std::unordered_map<std::string, std::vector<std::string>> &edges)
{
    visited.insert(current);
    pathVisited.insert(current);
    for (std::string neighbour : edges[current])
    {
        if (pathVisited.contains(neighbour))
            return true;
        else if (visited.contains(neighbour))
            continue;
        else if (DFS(neighbour, visited, pathVisited, edges))
            return true;
    }
    pathVisited.erase(current);
    return false;
}

void generateAssemblyFile(const std::string &filename, const std::vector<std::string> &assemblyCodes)
{
    std::ofstream outFile("output/" + filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    for (const auto &line : assemblyCodes)
    {
        outFile << line << "\n";
    }

    outFile.close();
}

template <typename T>
bool detectCycle(std::vector<std::shared_ptr<T>> decls)
{
    // prepare the graph
    std::vector<std::string> vertices;
    std::unordered_map<std::string, std::vector<std::string>> edges;
    for (auto decl : decls)
    {
        vertices.push_back(decl->id->name);                 // add all classes/interfaces to vector of vertices
        edges[decl->id->name] = std::vector<std::string>{}; // initialize edges vector for each class/interface
        for (auto ext : decl->extended)
        {                                                   // go through neighbours
            edges[decl->id->name].push_back(ext->id->name); // add them to the list of edges
        }
    }
    // perform DFS
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> pathVisited;
    for (std::string vertex : vertices)
    {
        if (!edges.contains(vertex))
            continue;
        if (visited.contains(vertex))
            continue;
        else
        {
            bool result = DFS(vertex, visited, pathVisited, edges);
            if (result)
                return result;
        }
    }
    return false;
}

std::pair<DataType, std::string> getTypeValuePair(const std::shared_ptr<Type> &type)
{
    DataType typeValue = type->type;
    std::string objectName = "";
    if (typeValue == DataType::OBJECT)
    {
        std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(type);
        assert(idType != nullptr);
        objectName = idType->id->name;
    }
    else if (typeValue == DataType::ARRAY)
    {
        std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(type);
        assert(arrayType != nullptr);
        typeValue = arrayDataTypes[arrayType->dataType->type];
        if (typeValue == DataType::OBJECTARRAY)
        {
            std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(arrayType->dataType);
            assert(idType != nullptr);
            objectName = idType->id->name;
        }
    }
    return std::make_pair(typeValue, objectName);
}

struct PairStringVectorHash
{
    size_t operator()(const std::pair<std::string, std::vector<std::pair<DataType, std::string>>> &pair) const
    {
        std::hash<std::string> stringHasher;
        size_t seed = stringHasher(pair.first);
        for (const auto &innerPair : pair.second)
        {
            seed ^= std::hash<DataType>{}(innerPair.first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= stringHasher(innerPair.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
}; 

void populateIMTables(std::shared_ptr<Program> n, bool &error)
{
    assert(n->scope->current->imtablesPopulated == false);
    n->scope->current->imtablesPopulated = true;
    auto &iscmtable = n->scope->current->getISCMTable();
    auto &isimtable = n->scope->current->getISIMTable();

    // signature is pair<methodName, list of pair<paramType, objectName>> where objectName == "" for paramType != OBJECT, OBJECTARRAY
    // mtableSignaturesToReturnTypes maps signatures to return types, which is a pair<paramType, objectName> as above
    std::unordered_map<std::pair<std::string, std::vector<std::pair<DataType, std::string>>>, std::pair<DataType, std::string>, PairStringVectorHash>
        mtableSignaturesToReturnTypes;

    // for entries in our mtable,
    for (auto &p : n->scope->current->getMTable())
    {
        std::string methodName = p.first;
        // for each element in entry.second
        for (auto &method : p.second)
        {
            // make pair<mname, paramtypes> and map it to the method's return type
            std::vector<std::pair<DataType, std::string>> paramTypes{};
            for (auto &fp : method->formalParameters)
            {
                paramTypes.push_back(getTypeValuePair(fp->type));
            }
            std::pair<std::string, std::vector<std::pair<DataType, std::string>>> signature = std::make_pair(methodName, paramTypes);
            mtableSignaturesToReturnTypes[signature] = getTypeValuePair(method->type);
        }
    }

    auto processTable = [&](const auto &table, auto &writeTable, const bool defaultAddToTable, bool &error)
    {
        for (auto p : table)
        {
            std::string methodName = p.first;
            for (auto method : p.second)
            {
                std::vector<std::pair<DataType, std::string>> paramTypes{};
                for (std::shared_ptr<FormalParameter> fp : method->formalParameters)
                {
                    paramTypes.push_back(getTypeValuePair(fp->type));
                }
                auto signature = std::make_pair(methodName, paramTypes);
                auto returnType = getTypeValuePair(method->type);
                if (mtableSignaturesToReturnTypes.contains(signature))
                {
                    if (mtableSignaturesToReturnTypes[signature] != returnType)
                    {
                        std::cerr << "Error: Method is inherited with identical signature but different return type from existing method" << std::endl;
                        error = true;
                        return;
                    }
                }
                else if (!defaultAddToTable)
                    writeTable[methodName].push_back(method);
                if (defaultAddToTable)
                    writeTable[methodName].push_back(method);
            }
        }
    };

    // if we're an InterfaceDecl
    std::shared_ptr<InterfaceDecl> idecl = std::dynamic_pointer_cast<InterfaceDecl>(n->classOrInterfaceDecl);
    if (idecl != nullptr)
    {
        // for supers
        for (std::shared_ptr<SymbolTable> superTable : n->scope->supers)
        {
            if (!superTable)
                continue; // THIS SHOULD NEVER TRIGGER
            // recursively populate super imtable
            if (superTable->imtablesPopulated == false)
                populateIMTables(superTable->getAst(), error);
            if (error)
                return;
            // add extended interface methods to isimtable unless overridden
            std::shared_ptr<InterfaceDecl> sidecl = std::dynamic_pointer_cast<InterfaceDecl>(superTable->getClassOrInterfaceDecl());
            if (sidecl != nullptr)
            {
                processTable(superTable->getMTable(), isimtable, false, error);
                processTable(superTable->getISIMTable(), isimtable, false, error);
            } // otherwise, most likely an interface is extending a class, which will be caught later in hierarchy checking
        }
    }
    else
    {
        // assert we're a ClassDecl
        std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(n->classOrInterfaceDecl);
        assert(cdecl != nullptr);
        // for supers
        for (std::shared_ptr<SymbolTable> superTable : n->scope->supers)
        {
            if (!superTable)
                continue; // THIS SHOULD NEVER TRIGGER
            // recursively populate super imtable
            if (superTable->imtablesPopulated == false)
                populateIMTables(superTable->getAst(), error);
            if (error)
                return;
            // if super is an InterfaceDecl, add extended superinterface methods to isimtable unless there's an error
            std::shared_ptr<InterfaceDecl> sidecl = std::dynamic_pointer_cast<InterfaceDecl>(superTable->getClassOrInterfaceDecl());
            if (sidecl != nullptr)
            {
                processTable(superTable->getMTable(), isimtable, true, error);
                processTable(superTable->getISIMTable(), isimtable, true, error);
            }
            else
            {
                // add extended superclass methods to iscmtable unless overridden
                std::shared_ptr<ClassDecl> scdecl = std::dynamic_pointer_cast<ClassDecl>(superTable->getClassOrInterfaceDecl());
                assert(scdecl != nullptr);
                processTable(superTable->getMTable(), iscmtable, false, error);
                processTable(superTable->getISCMTable(), iscmtable, false, error);
            }
        }
    }
}

std::string removeClassName(std::string methodSignature, std::string className) {
    if (methodSignature.length() < className.length() || methodSignature.compare(0, className.length(), className) != 0) {
        throw std::runtime_error("Tried to remove class name from method signature without that class name: Inside removeClassName");
    } else {
        methodSignature.erase(0, className.length());
    }
    return methodSignature;
}

void populateVtableMethods(std::shared_ptr<Program> n, bool &error) {
    std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(n->classOrInterfaceDecl);
    if (!cdecl) return;

    assert(n->scope->current->vtableMethodsPopulated == false);
    n->scope->current->vtableMethodsPopulated = true;

    std::vector<std::shared_ptr<Method>> &vtableMethods = n->scope->current->getVtableMethods();
    std::unordered_map<std::string, int> signatureIndexMap;

    assert(vtableMethods.size() == 0);

    //find superclass
    for (const auto& superTable : n->scope->supers) {
        std::shared_ptr<ClassDecl> scdecl = std::dynamic_pointer_cast<ClassDecl>(superTable->getAst()->classOrInterfaceDecl);
        if (!scdecl) continue;

        //ensure the parent's vtableMethods is populated
        if (superTable->vtableMethodsPopulated == false) {
            populateVtableMethods(superTable->getAst(), error);
        }

        //copy super's methods to our vector
        std::vector<std::shared_ptr<Method>> &superVtableMethods = superTable->getVtableMethods();
        vtableMethods.insert(vtableMethods.end(), superVtableMethods.begin(), superVtableMethods.end());

        //now vtableMethods consists entirely of its superclass's vtableMethods
        break;
    }

    //vtableMethods has been populated with our superclass's methods, if any exist
    //populate our signatureIndexMap
    for (int i = 0; i < vtableMethods.size(); ++i) {
        std::shared_ptr<Method> method = vtableMethods[i];
        std::string methodSignature = removeClassName(method->getSignature(), method->className);
        assert(!signatureIndexMap.contains(methodSignature));
        signatureIndexMap[methodSignature] = i;
    }

    //look at our methods. if the method exists in our vtableMethods, override it. Else, add it and add new entry.
    for (const auto& memberDecl : cdecl->declarations[1]) {
        std::shared_ptr<Method> method = std::dynamic_pointer_cast<Method>(memberDecl);
        if (!method || method->isStatic) continue; //ignore static methods

        std::string methodSignature = removeClassName(method->getSignature(), method->className);

        if (signatureIndexMap.contains(methodSignature)) {
            vtableMethods[signatureIndexMap[methodSignature]] = method;
        } else {
            vtableMethods.push_back(method);
            signatureIndexMap[methodSignature] = vtableMethods.size() - 1;
        }
    }      
}

void populateVtableFields(std::shared_ptr<Program> n, bool &error) {

    std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(n->classOrInterfaceDecl);
    if (!cdecl) return;

    assert(n->scope->current->vtableFieldsPopulated == false);
    n->scope->current->vtableFieldsPopulated = true;
    auto &vtableFields = n->scope->current->getVtableFields();

    std::unordered_set<std::string> fieldNames;

    //get and set current vtableFields
    for (auto memberDecl : cdecl->declarations[0]) {
        std::shared_ptr<Field> field = std::dynamic_pointer_cast<Field>(memberDecl);
        if (!field || field->isStatic) continue;
        vtableFields.push_back(field);
        assert(!(fieldNames.contains(field->fieldName->name))); //we shouldn't have any duplicate fields
        fieldNames.insert(field->fieldName->name);
    }

    //for each parent
    for (std::shared_ptr<SymbolTable> superTable : n->scope->supers) {
        std::shared_ptr<ClassDecl> scdecl = std::dynamic_pointer_cast<ClassDecl>(superTable->getAst()->classOrInterfaceDecl);
        if (!scdecl) continue;

        //ensure the parent's vtableFields is populated
        if (superTable->vtableFieldsPopulated == false)
            populateVtableFields(superTable->getAst(), error);
        
        //get that parent's vtableFields
        auto &superVtableFields = superTable->getVtableFields();
        for (auto superField : superVtableFields) {
            assert(!(superField->isStatic));
            //if it's a duplicate, skip it
            if (fieldNames.contains(superField->fieldName->name)) continue;
            else {
                //otherwise, add it to our vtableFields
                vtableFields.push_back(superField);
                fieldNames.insert(superField->fieldName->name);
            }
        }
        return; //we can only extend at most one class
    }
}

/*
//require that subtypeTable doesn't already have an entry for n->classOrInterfaceDecl
void populateSubtypeTable2(std::shared_ptr<Program> n, bool &error) {
    std::shared_ptr<ClassOrInterfaceDecl> decl = n->classOrInterfaceDecl;
    if (subtypeTable.contains(decl)) return;

    subtypeTable.insert({decl, {}});

    for (auto superTable : n->scope->supers) {
        std::shared_ptr<ClassOrInterfaceDecl> superDecl = superTable->getClassOrInterfaceDecl();

        //if the superDecl is already in our set, do a check and then continue
        if (subtypeTable.at(decl).contains(superDecl)) {
            //confirm that superDecl's entry in subtypeTable already exists, otherwise error
            if (!subtypeTable.contains(superDecl)) {
                std::cerr << "Error: Subtype Table Building failed, supertype's entry not filled in" << std::endl;
                error = true;
                return;
            } else continue;
        } else {
            //otherwise, add it and its ancestors to our set
            populateSubtypeTable(superTable->getAst(), error);
            if (error) return;
            subtypeTable.at(decl).insert(superDecl);
            for (std::shared_ptr<ClassOrInterfaceDecl> ancestorDecl : subtypeTable.at(superDecl)) {
                if (subtypeTable.at(decl).contains(ancestorDecl)) continue;
                subtypeTable.at(decl).insert(ancestorDecl);
            }
        }
    }
}*/

/*
//require that subtypeTable doesn't already have an entry for n->classOrInterfaceDecl
void populateStringSubtypeTable(std::shared_ptr<Program> n, bool &error) {
    std::shared_ptr<ClassOrInterfaceDecl> decl = n->classOrInterfaceDecl;
    std::string fullClassName = /*n->package->id->name + "." +*/// decl->id->name;
 /*   if (subtypeTable.contains(fullClassName)) return;

    subtypeTable.insert({fullClassName, {}});
    subtypeTable.at(fullClassName).insert("Object");

    for (auto superTable : n->scope->supers) {
        std::shared_ptr<ClassOrInterfaceDecl> superDecl = superTable->getClassOrInterfaceDecl();
        //std::string fullSuperClassName = /*superTable->getPackage() + "." +*/ //superTable->getClassName();
       // std::string fullSuperClassName = /*superTable->getPackage() + "." +*/ //superTable->getAst()->classOrInterfaceDecl->id->name;

        //if the superDecl is already in our set, do a check and then continue
       /* if (subtypeTable.at(fullClassName).contains(fullSuperClassName)) {
            //confirm that superDecl's entry in subtypeTable already exists, otherwise error
            if (!subtypeTable.contains(fullSuperClassName)) {
                std::cerr << "Error: Subtype Table Building failed, supertype's entry not filled in" << std::endl;
                error = true;
                return;
            } else continue;
        } else {
            //otherwise, add it and its ancestors to our set
            populateSubtypeTable(superTable->getAst(), error);
            if (error) return;
            subtypeTable.at(fullClassName).insert(fullSuperClassName);
            for (std::string fullAncestorClassName : subtypeTable.at(fullSuperClassName)) {
                if (subtypeTable.at(fullClassName).contains(fullAncestorClassName)) continue;
                subtypeTable.at(fullClassName).insert(fullAncestorClassName);
            }
        }
    }
}*/

//require that subtypeTable doesn't already have an entry for n->classOrInterfaceDecl
void populateSubtypeTable(std::shared_ptr<Program> n, bool &error) {
    std::shared_ptr<ClassOrInterfaceDecl> decl = n->classOrInterfaceDecl;
    std::string fullClassName = /*n->package->id->name + "." +*/ decl->id->name;
    std::shared_ptr<SymbolTable> table = n->scope->current;
    if (subtypeTable.contains(table)) return;

    subtypeTable.insert({table, {}});
    /////subtypeTable.at(st).insert("Object");

    for (auto superTable : n->scope->supers) {
        //if the superDecl is already in our set, do a check and then continue
        if (subtypeTable.at(table).contains(superTable)) {
            //confirm that superTable's entry in subtypeTable already exists, otherwise error
            if (!subtypeTable.contains(superTable)) {
                std::cerr << "Error: Subtype Table Building failed, supertype's entry not filled in" << std::endl;
                error = true;
                return;
            } else continue;
        } else {
            //otherwise, add it and its ancestors to our set
            populateSubtypeTable(superTable->getAst(), error);
            if (error) return;
            subtypeTable.at(table).insert(superTable);
            for (std::shared_ptr<SymbolTable> ancestorTable : subtypeTable.at(superTable)) {
                if (subtypeTable.at(table).contains(ancestorTable)) continue;
                subtypeTable.at(table).insert(ancestorTable);
            }
        }
    }
}





int main(int argc, char *argv[])
{
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>>
        tables;
    std::shared_ptr<PackageTrie> pkgTrie = std::make_shared<PackageTrie>();
    std::vector<std::shared_ptr<Program>> asts;
    bool error{false};
    for (int i = 1; i < argc; ++i)
    {
        std::ifstream inputFile;
        inputFile.open(argv[i]);
        if (!inputFile.is_open())
        {
            std::cerr << "Error: Unable to open file " << argv[i] << std::endl;
            return 1;
        }
        std::string filename{extractFilename(argv[i])};
        yy::MyLexer lexer(*static_cast<std::istream *>(&inputFile), filename);
        Ast ast;
        yy::parser parser(lexer, ast);
        int ret = parser.parse();
        if (ret != 0)
        {
            error = true;
            break;
        }
        if (std::shared_ptr<Program> program = ast.getAst())
        {
            // PrintVisitor visitor;
            ContextVisitor cvisitor{};
            program->accept(&cvisitor);
            auto [pkg, cdecl] = program->getQualifiedName();
            std::string qualifiedName{pkg + "." + cdecl};
            auto symbolTable = cvisitor.getSymbolTable();
            if (cvisitor.isError() || !pkgTrie->insert(qualifiedName, symbolTable))
            {
                std::cerr << "Error: Environment Building failed" << std::endl;
                error = true;
                break;
            }
            program->scope = std::make_shared<Scope>(symbolTable, pkgTrie);
            symbolTable->setScope(program->scope);
            asts.push_back(program);
        }
    }

    // partition class and interface declarations into two vectors, and populate each scope's supers as well
    std::vector<std::shared_ptr<ClassDecl>> classDecls;
    std::vector<std::shared_ptr<InterfaceDecl>> interfaceDecls;

    for (auto program : asts)
    {
        if (!program)
            continue;
        if (program->classOrInterfaceDecl)
        {
            // Check if this program holds a ClassDecl
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr)
                // if yes, add to classDecls but also add implemented list to supers
                classDecls.push_back(classDecl);
            else
            {
                // otherwise, ensure that program holds an InterfaceDecl, and add to interfaceDecls
                std::shared_ptr<InterfaceDecl> interfaceDecl = std::dynamic_pointer_cast<InterfaceDecl>(program->classOrInterfaceDecl);
                assert(interfaceDecl != nullptr);
                interfaceDecls.push_back(interfaceDecl);
            }
        }
    }

    bool classCycle = detectCycle<ClassDecl>(classDecls);
    bool interfaceCycle = detectCycle<InterfaceDecl>(interfaceDecls);
    if (classCycle)
    {
        std::cerr << "Error: Detected a cycle among the classes" << std::endl;
        std::cerr << "Error: Hierarchy Checking failed" << std::endl;
        error = true;
    }
    else if (interfaceCycle)
    {
        std::cerr << "Error: Detected a cycle among the interfaces" << std::endl;
        std::cerr << "Error: Hierarchy Checking failed" << std::endl;
        error = true;
    }

    //reset
    subtypeTable.clear(); //just in case
    for (auto ast : asts) 
    {
        populateSubtypeTable(ast, error);
    }


    if (!error)
    {
        for (auto ast : asts)
        {
            TypeLinkingVisitor tvisitor{ast->scope};
            ast->accept(&tvisitor);
            if (tvisitor.isError())
            {
                std::cerr << "Error: Type linking failed" << std::endl;
                error = true;
                break;
            }
        }
    }

    // populate supers
    for (auto program : asts)
    {
        if (!program)
            continue;
        if (program->classOrInterfaceDecl)
        {

            // Add extended list to supers
            for (std::shared_ptr<IdentifierType> idType : program->classOrInterfaceDecl->extended)
                program->scope->supers.push_back(program->scope->getNameInScope(idType->id->name, idType->simple));

            // Check if this program holds a ClassDecl
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr)
                // if yes, add implemented list to supers
                for (std::shared_ptr<IdentifierType> idType : classDecl->implemented)
                    program->scope->supers.push_back(program->scope->getNameInScope(idType->id->name, idType->simple));
        }
    }

    if (!error)
    {
        for (auto ast : asts)
            if (ast->scope->current->imtablesPopulated == false)
                populateIMTables(ast, error);
    }

    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
        {
            HierarchyVisitor hvisitor(program->scope);
            program->accept(&hvisitor);
            if (hvisitor.isError())
            {
                std::cerr << "Error: Hierarchy Checking failed" << std::endl;
                error = true;
                break;
            }
        }
    }

    if (!error)
    {
        // Rule 9: Class/interface can't contain (declare or inherit) two methods with same signature but diff return types
        for (auto ast : asts)
        {
            // std::unordered_map<std::vector<std::pair<DataType, std::string>>, std::pair<DataType, std::string>, PairVectorHash> mapSignatureToReturnType; //stores signatures seen thus far
            std::unordered_map<std::pair<std::string, std::vector<std::pair<DataType, std::string>>>, std::pair<DataType, std::string>, PairStringVectorHash> mapSignatureToReturnType;
            auto processTable = [&](const auto &table, bool &error)
            {
                assert(!error);
                for (const auto &p : table)
                {
                    std::string methodName = p.first;
                    for (std::shared_ptr<Method> method : p.second)
                    {
                        std::vector<std::pair<DataType, std::string>> paramTypes{};
                        for (auto fp : method->formalParameters)
                        {
                            paramTypes.push_back(getTypeValuePair(fp->type));
                        }
                        auto signature = std::make_pair(methodName, paramTypes);
                        auto returnType = getTypeValuePair(method->type);
                        if (mapSignatureToReturnType.contains(signature))
                        {
                            if (mapSignatureToReturnType[signature] != returnType)
                            {
                                std::cerr << "Error: Class/Interface contains two methods with same signature but diff return types" << std::endl;
                                error = true;
                                return;
                            }
                        }
                        else
                            mapSignatureToReturnType[signature] = returnType;
                    }
                }
            };
            // if its an interface,
            std::shared_ptr<InterfaceDecl> idecl = std::dynamic_pointer_cast<InterfaceDecl>(ast->classOrInterfaceDecl);
            if (idecl != nullptr)
            {
                processTable(ast->scope->current->getMTable(), error); // check declared methods
                if (error)
                    break;
                processTable(ast->scope->current->getISIMTable(), error); // check inherited from superinterfaces
                if (error)
                    break;
            }
            else
            {
                // assert its a class
                std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(ast->classOrInterfaceDecl);
                assert(cdecl != nullptr);
                processTable(ast->scope->current->getMTable(), error); // check declared
                if (error)
                    break;
                processTable(ast->scope->current->getISCMTable(), error); // check inherited from superclass
                if (error)
                    break;
                processTable(ast->scope->current->getISIMTable(), error); // check inherited from superinterface
                if (error)
                    break;
            }
        }
    }
    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
        {
            TypeCheckingVisitor tcvisitor(program->scope);
            program->accept(&tcvisitor);
            if (tcvisitor.isError())
            {
                std::cerr << "Error: Type Checking failed" << std::endl;
                error = true;
                break;
            }
        }
    }
    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
        {
            // std::cout << program->scope->current->getClassOrInterfaceDecl()->id->name << std::endl;
            CFGVisitor cfgvisitor(program->scope);
            program->accept(&cfgvisitor);
            // break;
        }
    }

    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
            if (program->scope->current->vtableFieldsPopulated == false)
                populateVtableFields(program, error);
    }

    if (!error)
    {
        for (std::shared_ptr<Program> program : asts)
            if (program->scope->current->vtableMethodsPopulated == false)
                populateVtableMethods(program, error);
    }

    if (!error)
    {
        std::shared_ptr<TIR::NodeFactory_c> nodeFactory = std::make_shared<TIR::NodeFactory_c>();
        std::vector<std::string> staticFields;
        std::vector<std::string> vtables;
        std::vector<std::shared_ptr<TIR::Stmt>> staticFieldStmts;
        std::vector<std::vector<std::string>> staticMethods;
        std::vector<std::shared_ptr<TIR::CompUnit>> compUnits;
        for (std::shared_ptr<Program> program : asts)
        {
            std::cout << program->getQualifiedName().second << std::endl;
            TransformVisitor tvisitor(program->scope, nodeFactory, staticFields, staticFieldStmts, staticMethods);
            program->accept(&tvisitor);
            std::shared_ptr<TIR::CompUnit> cu = tvisitor.getCompUnit();
            compUnits.push_back(cu);
            vtables.push_back(cu->getName() + "_vtable");
        }
        staticFieldStmts.push_back(nodeFactory->IRReturn(nodeFactory->IRConst(0)));
        compUnits[0]->appendFunc(nodeFactory->IRFuncDecl(compUnits[0]->getName() + TIR::Configuration::STATIC_INIT_FUNC, 0, nodeFactory->IRSeq(staticFieldStmts)));
        for (size_t i = 0; i < compUnits.size(); ++i) {
            std::shared_ptr<TIR::CompUnit> compUnit = compUnits[i];
            std::shared_ptr<SymbolTable> st = asts[i]->scope->current;
            try
            {
                std::shared_ptr<CanonicalVisitor> cvisitor = std::make_shared<CanonicalVisitor>();
                cvisitor->visit(compUnit);
            }
            catch (std::exception &e)
            {
                std::cout << "Error in CanonicalVisitor" << std::endl;
            }

            try
            {
                std::shared_ptr<TIR::CfgVisitor> cfv = std::make_shared<TIR::CfgVisitor>();
                cfv->visit(compUnit);
                std::shared_ptr<TIR::CheckCanonicalIRVisitor> ccv = std::make_shared<TIR::CheckCanonicalIRVisitor>();
                std::cout << "Canonical? " << (ccv->visit(compUnit) ? "Yes" : "No") << std::endl;
            }
            catch (std::exception &e)
            {
                std::cout << "Error in CFG" << std::endl;
            }

            {
                // generate .s file for compUnit
                std::vector<std::string> assemblyCodes;
                assemblyCodes.push_back("extern __malloc");
                assemblyCodes.push_back("extern __exception");
                assemblyCodes.push_back("extern NATIVEjava.io.OutputStream.nativeWrite");

                if (i != 0) {
                    for (const auto& field : staticFields) {
                        assemblyCodes.push_back("extern " + field);
                    }
                }
                for (size_t j = 0; j < staticMethods.size(); ++j) {
                    if (j == i) continue;
                    for (const auto& method : staticMethods[j]) {
                        assemblyCodes.push_back("extern " + method);
                    }
                }
                if (i == 0) {
                    for (const auto& field : staticFields) {
                        assemblyCodes.push_back("global " + field);
                    }
                }
                for (size_t k = 0; k < vtables.size(); ++k) {
                    if (k == i) continue;
                    assemblyCodes.push_back("extern " + vtables[k]);
                }
                assemblyCodes.push_back("\nglobal " + compUnit->getName()+ "_vtable");
                // static data
                assemblyCodes.push_back("section .data");
                std::vector<std::shared_ptr<Method>>& methods = st->getVtableMethods();
                size_t methodSize{methods.size()};
                std::string vtableDecl = compUnit->getName()+ "_vtable: dd ";
                bool validVTable{false};
                for (size_t i = 0; i < methodSize; ++i) {
                    if (!methods[i]->isNative && !methods[i]->isAbstract) {
                        vtableDecl += methods[i]->getSignature();
                        if (i != methodSize - 1) vtableDecl += ", ";
                        validVTable = true;
                    }
                }
                if (!validVTable) vtableDecl += "0";
                assemblyCodes.push_back(vtableDecl);
                if (i == 0) {
                    for (const auto& field : staticFields)
                    {
                        assemblyCodes.push_back(field + ": dd 0");
                    }
                }
                assemblyCodes.push_back("\nsection .text");
                Tiling tiler(st, staticFields, vtables);
                if (i == 0) {
                    assemblyCodes.push_back("global _start");
                    assemblyCodes.push_back("_start:"); // Entry point label
                    assemblyCodes.push_back("call " + compUnit->getName() + TIR::Configuration::STATIC_INIT_FUNC);
                    assemblyCodes.push_back("call " + compUnit->getName() + "_test_int");
                    assemblyCodes.push_back("mov ebx, eax");
                    assemblyCodes.push_back("mov eax, 1");
                    assemblyCodes.push_back("int 0x80");
                }

                for (auto &[funcName, funcDecl] : compUnit->getFunctions())
                {
                    tiler.currentStackOffset = 0;
                    tiler.tempToStackOffset.clear();
                    // std::cout << funcName << " " << funcDecl->numTemps << std::endl;
                    assemblyCodes.push_back("\nglobal " + funcDecl->getName());
                    assemblyCodes.push_back(funcDecl->getName() + ":");
                    assemblyCodes.push_back("push ebp");
                    assemblyCodes.push_back("mov ebp, esp");
                    assemblyCodes.push_back("sub esp, " + std::to_string(4 * funcDecl->numTemps));
                    for (int i = 0; i < funcDecl->getNumParams(); i++) {
                        tiler.tempToStackOffset[Configuration::ABSTRACT_ARG_PREFIX + std::to_string(i)] = 4 * (i + 2);
                    }

                    for (auto stmt : std::dynamic_pointer_cast<Seq>(funcDecl->getBody())->getStmts())
                    {
                        tiler.tileStmt(stmt, assemblyCodes);
                    }

                }

                assemblyCodes.push_back("\nzerodivisionlabel:");
                assemblyCodes.push_back("call __exception");
                generateAssemblyFile(compUnit->getName() + ".s", assemblyCodes);
            }
        }
    }

    if (error)
        return 42;
    else
        return 0;
}
