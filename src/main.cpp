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

void populateIMTable(std::shared_ptr<Program> n) {
    assert(n->scope->current->imtablePopulated == false);
    n->scope->current->imtablePopulated = true;
    auto& imtable = n->scope->current->imtable;
    //for each super's table:
    for (std::shared_ptr<SymbolTable> superTable : n->scope->supers) {
        //recursively populate super imtable
        if (superTable->imtablePopulated == false) populateIMTable(superTable->getAst());
        //add each entry in the super's imtable to the corresponding entry in the current imtable
        for (auto& pair : superTable->imtable) 
            imtable[pair.first].insert(imtable[pair.first].end(), pair.second.begin(), pair.second.end());
        //do the same with the super's mtable
        for (auto& pair : superTable->getMTable()) 
            imtable[pair.first].insert(imtable[pair.first].end(), pair.second.begin(), pair.second.end());
    }
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
            if (ast->scope->current->imtablePopulated == false)
                populateIMTable(ast);
        
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

        std::unordered_map<DataType, DataType> arrayDataTypes = {
        {DataType::VOID, DataType::VOIDARRAY}, {DataType::INT, DataType::INTARRAY}, {DataType::BOOLEAN, DataType::BOOLEANARRAY},
        {DataType::CHAR, DataType::CHARARRAY}, {DataType::BYTE, DataType::BYTEARRAY}, {DataType::SHORT, DataType::SHORTARRAY},
        {DataType::LONG, DataType::LONGARRAY}, {DataType::FLOAT, DataType::FLOATARRAY}, {DataType::DOUBLE, DataType::DOUBLEARRAY},
        {DataType::OBJECT, DataType::OBJECTARRAY}
        };

        //Rule 9: Class/interface can't contain (declare or inherit) two methods with same signature but diff return types
        for (auto ast : asts) {
        //get vector of strings
            std::unordered_set<std::string> methodNames; //to avoid double checks for the same method name if included in mtable and imtable
            for (const auto& mtableEntry : ast->scope->current->getMTable())
                methodNames.insert(mtableEntry.first);
            for (const auto& imtableEntry : ast->scope->current->getIMTable())
                methodNames.insert(imtableEntry.first);
            for (std::string s : methodNames) {
                std::unordered_map<std::pair<std::vector<DataType>, std::vector<std::string>>, 
                std::pair<DataType, std::string>, PairVectorHash> mp;

                std::vector<std::shared_ptr<Method>> methodPtrs;
                for (auto methodPtr : ast->scope->current->getMTable()[s]) methodPtrs.push_back(methodPtr);
                for (auto methodPtr : ast->scope->current->getIMTable()[s]) methodPtrs.push_back(methodPtr);
                for (auto methodPtr : methodPtrs) {
                    std::pair<DataType, std::string> returnTypePair;
                    std::string returnObject = "";
                    DataType d = methodPtr->returnType->type;
                    if (d == DataType::ARRAY) {
                        std::shared_ptr<ArrayType> arrayType = std::dynamic_pointer_cast<ArrayType>(methodPtr->returnType);
                        assert(arrayType != nullptr);
                        d = arrayDataTypes[arrayType->dataType->type];
                        if (d == DataType::OBJECTARRAY) {
                            std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(arrayType->dataType);
                            assert(idType != nullptr);
                            returnObject = idType->id->name;
                        }
                    } else if (d == DataType::OBJECT) {
                        std::shared_ptr<IdentifierType> idType = std::dynamic_pointer_cast<IdentifierType>(methodPtr->returnType);
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
