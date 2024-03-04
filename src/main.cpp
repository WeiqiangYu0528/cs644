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

std::unordered_map<DataType, DataType> arrayDataTypes = {
        {DataType::VOID, DataType::VOIDARRAY}, {DataType::INT, DataType::INTARRAY}, {DataType::BOOLEAN, DataType::BOOLEANARRAY},
        {DataType::CHAR, DataType::CHARARRAY}, {DataType::BYTE, DataType::BYTEARRAY}, {DataType::SHORT, DataType::SHORTARRAY},
        {DataType::LONG, DataType::LONGARRAY}, {DataType::FLOAT, DataType::FLOATARRAY}, {DataType::DOUBLE, DataType::DOUBLEARRAY},
        {DataType::OBJECT, DataType::OBJECTARRAY}
        };

std::string extractFilename(const std::string& filePath) {
    size_t start = filePath.rfind('/') + 1; // Find the last '/' character
    size_t end = filePath.rfind('.');       // Find the last '.' character
    if (start == std::string::npos || end == std::string::npos || end <= start) {
        return ""; // Return an empty string if the format is not as expected
    }
    return filePath.substr(start, end - start); // Extract the substring representing the filename
}

bool DFS(std::string current, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& pathVisited, 
std::unordered_map<std::string, std::vector<std::string>>& edges) {
    visited.insert(current);
    pathVisited.insert(current);
    for (std::string neighbour : edges[current]) {
        if (pathVisited.contains(neighbour)) return true;
        else if (visited.contains(neighbour)) continue;
        else if (DFS(neighbour, visited, pathVisited, edges)) return true;
    }
    pathVisited.erase(current);
    return false;
}

template <typename T> 
bool detectCycle(std::vector<std::shared_ptr<T>> decls) {
    //prepare the graph
    std::vector<std::string> vertices;
    std::unordered_map<std::string, std::vector<std::string>> edges;
    for (auto decl : decls) {
        vertices.push_back(decl->id->name); //add all classes/interfaces to vector of vertices
        edges[decl->id->name] = std::vector<std::string>{}; //initialize edges vector for each class/interface
        for (auto ext : decl->extended) { //go through neighbours
            edges[decl->id->name].push_back(ext->id->name); //add them to the list of edges
        }
    }
    //perform DFS
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> pathVisited;
    for (std::string vertex : vertices) {
        if (!edges.contains(vertex)) continue;
        if (visited.contains(vertex)) continue;
        else {
            bool result = DFS(vertex, visited, pathVisited, edges);
            if (result) return result;
        }
    }
    return false;
}

std::pair<DataType, std::string> getTypeValuePair(const std::shared_ptr<Type>& type) {
    DataType typeValue = type->type;
    std::string objectName = "";
    if (typeValue == DataType::OBJECT) {
        std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(type);
        assert(idType != nullptr);
        objectName = idType->id->name;
    } else if (typeValue == DataType::ARRAY) {
        std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(type);
        assert(arrayType != nullptr);
        typeValue = arrayDataTypes[arrayType->dataType->type];
        if (typeValue == DataType::OBJECTARRAY) {
            std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(arrayType->dataType);
            assert(idType != nullptr);
            objectName = idType->id->name;
        }
    }
    return std::make_pair(typeValue, objectName);
}

struct PairStringVectorHash {
    size_t operator()(const std::pair<std::string, std::vector<std::pair<DataType, std::string>>>& pair) const {
        std::hash<std::string> stringHasher;
        size_t seed = stringHasher(pair.first);
        for (const auto& innerPair : pair.second) {
            seed ^= std::hash<DataType>{}(innerPair.first) + 0x9e3779b9 + (seed<<6) + (seed>>2);
            seed ^= stringHasher(innerPair.second) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};

void populateIMTables(std::shared_ptr<Program> n, bool& error) {

    assert(n->scope->current->imtablesPopulated == false);
    n->scope->current->imtablesPopulated = true;
    auto& iscmtable = n->scope->current->getISCMTable();
    auto& isimtable = n->scope->current->getISIMTable();


    //signature is pair<methodName, list of pair<paramType, objectName>> where objectName == "" for paramType != OBJECT, OBJECTARRAY
    //mtableSignaturesToReturnTypes maps signatures to return types, which is a pair<paramType, objectName> as above
    std::unordered_map<std::pair<std::string, std::vector<std::pair<DataType, std::string>>>, std::pair<DataType, std::string>, PairStringVectorHash> 
        mtableSignaturesToReturnTypes; 
    
    //for entries in our mtable,
    for (auto& p : n->scope->current->getMTable()) {
        std::string methodName = p.first;
        //for each element in entry.second
        for (auto& method : p.second) {
            //make pair<mname, paramtypes> and map it to the method's return type
            std::vector<std::pair<DataType, std::string>> paramTypes{};
            for (auto& fp : method->formalParameters) {
                paramTypes.push_back(getTypeValuePair(fp->type));
            }
            std::pair<std::string, std::vector<std::pair<DataType, std::string>>> signature = std::make_pair(methodName, paramTypes);
            mtableSignaturesToReturnTypes[signature] = getTypeValuePair(method->type);
        }
    }   

    auto processTable = [&](const auto& table, auto& writeTable, const bool defaultAddToTable, bool& error) {
        for (auto p : table) {
            std::string methodName = p.first;
            for (auto method : p.second) {
                std::vector<std::pair<DataType, std::string>> paramTypes{};
                for (std::shared_ptr<FormalParameter> fp : method->formalParameters) {
                    paramTypes.push_back(getTypeValuePair(fp->type));
                }
                auto signature = std::make_pair(methodName, paramTypes);
                auto returnType = getTypeValuePair(method->type);
                if (mtableSignaturesToReturnTypes.contains(signature)) {
                    if (mtableSignaturesToReturnTypes[signature] != returnType) {
                        std::cerr << "Error: Method is inherited with identical signature but different return type from existing method" << std::endl;
                        error = true;
                        return;
                    }
                } else if (!defaultAddToTable) writeTable[methodName].push_back(method);
                if (defaultAddToTable) writeTable[methodName].push_back(method);
            }
        }
    };

    //if we're an InterfaceDecl
    std::shared_ptr<InterfaceDecl> idecl = std::dynamic_pointer_cast<InterfaceDecl>(n->classOrInterfaceDecl);
    if (idecl != nullptr) {
        //for supers
        for (std::shared_ptr<SymbolTable> superTable : n->scope->supers) {
            if (!superTable) continue; //THIS SHOULD NEVER TRIGGER
            //recursively populate super imtable
            if (superTable->imtablesPopulated == false) populateIMTables(superTable->getAst(), error);
            if (error) return;
            //add extended interface methods to isimtable unless overridden
            std::shared_ptr<InterfaceDecl> sidecl = std::dynamic_pointer_cast<InterfaceDecl>(superTable->getClassOrInterfaceDecl());
            assert(sidecl != nullptr);
            processTable(superTable->getMTable(), isimtable, false, error);
            processTable(superTable->getISIMTable(), isimtable, false, error);
        }
    }
    else {
        //assert we're a ClassDecl
        std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(n->classOrInterfaceDecl);
        assert(cdecl != nullptr);
        //for supers
        for (std::shared_ptr<SymbolTable> superTable : n->scope->supers) {
            if (!superTable) continue; //THIS SHOULD NEVER TRIGGER
            //recursively populate super imtable
            if (superTable->imtablesPopulated == false) populateIMTables(superTable->getAst(), error);
            if (error) return;
            //if super is an InterfaceDecl, add extended superinterface methods to isimtable unless there's an error
            std::shared_ptr<InterfaceDecl> sidecl = std::dynamic_pointer_cast<InterfaceDecl>(superTable->getClassOrInterfaceDecl());
            if (sidecl != nullptr) {
                processTable(superTable->getMTable(), isimtable, true, error);
                processTable(superTable->getISIMTable(), isimtable, true, error);
            } else {
                //add extended superclass methods to iscmtable unless overridden
                std::shared_ptr<ClassDecl> scdecl = std::dynamic_pointer_cast<ClassDecl>(n->classOrInterfaceDecl);
                assert(scdecl != nullptr);
                processTable(superTable->getMTable(), iscmtable, false, error);
                processTable(superTable->getISCMTable(), iscmtable, false, error);
            }
        }
    }




/*

    //for each super's table:
    for (std::shared_ptr<SymbolTable> superTable : n->scope->supers) {
        //recursively populate super imtable
        if (superTable->imtablesPopulated == false) populateIMTables(superTable->getAst(), error);
        //add each entry in the super's imtable to the corresponding entry in the current imtable
        for (auto& pair : superTable->imtable) 
            imtable[pair.first].insert(imtable[pair.first].end(), pair.second.begin(), pair.second.end());
        //do the same with the super's mtable
        for (auto& pair : superTable->getMTable()) 
            imtable[pair.first].insert(imtable[pair.first].end(), pair.second.begin(), pair.second.end());
    }
    */
}

int main(int argc, char* argv[])
{
    // std::unordered_map<std::string, 
    //                    std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> tables;
    std::shared_ptr<PackageTrie> pkgTrie = std::make_shared<PackageTrie>();
    std::vector<std::shared_ptr<Program>> asts;
    bool error {false};
    for (int i = 1; i < argc; ++i) {
        std::ifstream inputFile;
        inputFile.open(argv[i]);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Unable to open file " << argv[i] << std::endl;
            return 1;
        }
        std::string filename{extractFilename(argv[i])};
        yy::MyLexer lexer(*static_cast<std::istream*>(&inputFile), filename);
        Ast ast;
        yy::parser parser(lexer, ast);
        int ret = parser.parse();
        if (ret != 0) {
            error = true;
            break;
        }
        if (std::shared_ptr<Program> program = ast.getAst()) {
            // PrintVisitor visitor;
            ContextVisitor cvisitor{};
            program->accept(&cvisitor);
            auto [pkg, cdecl] = program->getQualifiedName();
            std::string qualifiedName {pkg + "." + cdecl};
            auto symbolTable = cvisitor.getSymbolTable();
            if (cvisitor.isError() || !pkgTrie->insert(qualifiedName, symbolTable)) {
                std::cerr << "Error: Environment Building failed" << std::endl;
                error = true;
                break;
            }
            program->scope = std::make_shared<Scope>(symbolTable, pkgTrie);
            symbolTable->setScope(program->scope);
            asts.push_back(program);
        }
    }

    //partition class and interface declarations into two vectors, and populate each scope's supers as well
    std::vector<std::shared_ptr<ClassDecl>> classDecls;
    std::vector<std::shared_ptr<InterfaceDecl>> interfaceDecls;

    for (auto program : asts) {
        if (!program) continue;
        if (program->classOrInterfaceDecl) {
            //Check if this program holds a ClassDecl
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr)
                //if yes, add to classDecls but also add implemented list to supers
                classDecls.push_back(classDecl);
            else {
                //otherwise, ensure that program holds an InterfaceDecl, and add to interfaceDecls
                std::shared_ptr<InterfaceDecl> interfaceDecl = std::dynamic_pointer_cast<InterfaceDecl>(program->classOrInterfaceDecl);
                assert(interfaceDecl != nullptr);
                interfaceDecls.push_back(interfaceDecl);
            }
        }
    }

    bool classCycle = detectCycle<ClassDecl>(classDecls);
    bool interfaceCycle = detectCycle<InterfaceDecl>(interfaceDecls);
    if (classCycle) {
        std::cerr << "Error: Detected a cycle among the classes" << std::endl; 
        std::cerr << "Error: Hierarchy Checking failed" << std::endl;
        error = true;
    } else if (interfaceCycle) {
        std::cerr << "Error: Detected a cycle among the interfaces" << std::endl; 
        std::cerr << "Error: Hierarchy Checking failed" << std::endl;
        error = true;
    }

    if (!error) {
        for (auto ast : asts) {
            TypeLinkingVisitor tvisitor{ast->scope};
            ast->accept(&tvisitor);
            if (tvisitor.isError()) {
                std::cerr << "Error: Type linking failed" << std::endl;
                error = true;
                break;
            }
        }
    }

    //populate supers
    for (auto program : asts) {
        if (!program) continue;
        if (program->classOrInterfaceDecl) {

            //Add extended list to supers
            for (std::shared_ptr<IdentifierType> idType : program->classOrInterfaceDecl->extended)
                program->scope->supers.push_back(program->scope->getNameInScope(idType->id->name, idType->simple));
            
            //Check if this program holds a ClassDecl
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr) 
                //if yes, add implemented list to supers
                for (std::shared_ptr<IdentifierType> idType : classDecl->implemented) 
                    program->scope->supers.push_back(program->scope->getNameInScope(idType->id->name, idType->simple));
        }
    }

    if (!error) {
        for (std::shared_ptr<Program> program : asts) {
            HierarchyVisitor hvisitor(program->scope);
            program->accept(&hvisitor);
            if (hvisitor.isError()) {
                std::cerr << "Error: Hierarchy Checking failed" << std::endl;
                error = true;
                break;
            }
        }
    }


    if (!error) {
        for (auto ast : asts)
            if (ast->scope->current->imtablesPopulated == false)
                populateIMTables(ast, error);
    }

    if (!error) {  
        /* 
        struct PairVectorHash {
            size_t operator()(const std::pair<std::vector<DataType>, std::vector<std::string>>& p) const {
                std::hash<int> dataTypeHasher; // Hash function for DataType
                std::hash<std::string> stringHasher; // Hash function for std::string
                size_t seed = 0;
                for (const DataType& d : p.first)
                    seed ^= dataTypeHasher(static_cast<int>(d)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                for (const std::string& s : p.second)
                    seed ^= stringHasher(s) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                return seed;
            }
        };
        */

       /*
        struct PairVectorHash {
            size_t operator()(const std::vector<std::pair<DataType, std::string>>& vec) const {
                std::hash<int> dataTypeHasher; // Hash function for DataType
                std::hash<std::string> stringHasher; // Hash function for std::string
                size_t seed = 0;
                for (const auto& pair : vec) {
                    seed ^= dataTypeHasher(static_cast<int>(pair.first)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                    seed ^= stringHasher(pair.second) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
                return seed;
            }
        };*/

        //Rule 9: Class/interface can't contain (declare or inherit) two methods with same signature but diff return types

        for (auto ast : asts) {
            //std::unordered_map<std::vector<std::pair<DataType, std::string>>, std::pair<DataType, std::string>, PairVectorHash> mapSignatureToReturnType; //stores signatures seen thus far
            std::unordered_map<std::pair<std::string, std::vector<std::pair<DataType, std::string>>>, std::pair<DataType, std::string>, PairStringVectorHash> mapSignatureToReturnType;
            auto processTable = [&](const auto& table, bool& error) {
                assert(!error);
                for (const auto& p : table) {
                    std::string methodName = p.first;
                    for (std::shared_ptr<Method> method : p.second) {
                        std::vector<std::pair<DataType, std::string>> paramTypes{};
                        for (auto fp : method->formalParameters) {
                            paramTypes.push_back(getTypeValuePair(fp->type));
                        }
                        auto signature = std::make_pair(methodName, paramTypes);
                        auto returnType = getTypeValuePair(method->type);
                        if (mapSignatureToReturnType.contains(signature)) {
                            if (mapSignatureToReturnType[signature] != returnType) {
                                std::cerr << "Error: Class/Interface contains two methods with same signature but diff return types" << std::endl;
                                error = true;
                                return;
                            }
                        } else mapSignatureToReturnType[signature] = returnType;
                    }
                }
            };
            //if its an interface,
            std::shared_ptr<InterfaceDecl> idecl = std::dynamic_pointer_cast<InterfaceDecl>(ast->classOrInterfaceDecl);
            if (idecl != nullptr) {
                //go through all methods in our mtable and isimtable
                /*
                    get the method's signature and return type
                    if the signature is in mapSignatureToReturnType,
                        if the return type is different, give an error
                        else do nothing
                    else add to mapSignatureToReturnType
                */
               processTable(ast->scope->current->getMTable(), error);
               if (error) break;
               processTable(ast->scope->current->getISIMTable(), error);
               if (error) break;
            }

            else {
                //assert its a class
                std::shared_ptr<ClassDecl> cdecl = std::dynamic_pointer_cast<ClassDecl>(ast->classOrInterfaceDecl);
                assert(cdecl != nullptr);
                //go through all methods in our mtable and iscmtable
                /*  
                    get the method's signature and return type
                    if the signature is already in the map
                        if the return type is different, give an error
                        else do nothing
                    else add to map sig to return type
                */
               processTable(ast->scope->current->getMTable(), error);
               if (error) break;
               processTable(ast->scope->current->getISCMTable(), error);
               if (error) break;
            }

        }



/*
        for (auto ast : asts) {
        //get vector of strings
            std::unordered_set<std::string> methodNames; //to avoid double checks for the same method name if included in mtable and imtable
            for (const auto& mtableEntry : ast->scope->current->getMTable())
                methodNames.insert(mtableEntry.first);
            //for (const auto& imtableEntry : ast->scope->current->getIMTable())
            //    methodNames.insert(imtableEntry.first);
            for (const auto& iscmtableEntry : ast->scope->current->getISCMTable())
                methodNames.insert(iscmtableEntry.first);
            for (const auto& isimtableEntry : ast->scope->current->getISIMTable())
                methodNames.insert(isimtableEntry.first);

            for (std::string s : methodNames) {
                std::unordered_map<std::pair<std::vector<DataType>, std::vector<std::string>>, 
                std::pair<DataType, std::string>, PairVectorHash> mp;

                std::vector<std::shared_ptr<Method>> methodPtrs;
                for (auto methodPtr : ast->scope->current->getMTable()[s]) methodPtrs.push_back(methodPtr);
                //for (auto methodPtr : ast->scope->current->getIMTable()[s]) methodPtrs.push_back(methodPtr);
                for (auto methodPtr : ast->scope->current->getISCMTable()[s]) methodPtrs.push_back(methodPtr);
                for (auto methodPtr : ast->scope->current->getISIMTable()[s]) methodPtrs.push_back(methodPtr);
                
                for (auto methodPtr : methodPtrs) {
                    std::pair<DataType, std::string> returnTypePair;
                    std::string returnObject = "";
                    DataType d = methodPtr->type->type;
                    if (d == DataType::ARRAY) {
                        std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(methodPtr->type);
                        assert(arrayType != nullptr);
                        d = arrayDataTypes[arrayType->dataType->type];
                        if (d == DataType::OBJECTARRAY) {
                            std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(arrayType->dataType);
                            assert(idType != nullptr);
                            returnObject = idType->id->name;
                        }
                    } else if (d == DataType::OBJECT) {
                        std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(methodPtr->type);
                        assert(idType != nullptr);
                        returnObject = idType->id->name;
                    }
                    //create the return pair
                    returnTypePair = std::make_pair(d, returnObject);
                    //next, create the vector of parameter types
                    std::vector<DataType> paramTypes;
                    std::vector<std::string> objectNames;
                    for (auto fp : methodPtr->formalParameters) {
                        d = fp->type->type;
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
                    std::pair<std::vector<DataType>, std::vector<std::string>> key = std::make_pair(paramTypes, objectNames);
                    if (mp.find(key) != mp.end()) { //mp has the paramTypes and objectNames
                        if (mp[key] != returnTypePair) {
                            std::cerr << "Error: Inherited/Declared method signature repeated with different return type" << std::endl;
                            std::cerr << "Error: Hierarchy Checking failed" << std::endl;
                            error = true;
                            break;
                        }
                    } else mp[key] = returnTypePair;
                    
                }
                if (error == true) break;
            }
            if (error == true) break;
        }
        */

    }

    /*
    if (!error) {
        for (std::shared_ptr<Program> program : asts) {
            TypeCheckingVisitor tcvisitor(program->scope);
            program->accept(&tcvisitor);
            if (tcvisitor.isError()) {
                std::cerr << "Error: Type Checking failed" << std::endl;
                error = true;
                break;
            }
            break; // only for developing
        }
    }
    */

    if (error)
        return 42;
    else
        return 0;
}
