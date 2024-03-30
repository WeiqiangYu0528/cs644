#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "PackageTrie.hpp"
#include "parser.h"
#include "contextVisitor.hpp"
#include "symbolTable.hpp"
#include "typeLinkingVisitor.hpp"
#include "hierarchyVisitor.hpp"
#include "transformVisitor.hpp"
#include "IRAst.hpp"
#include "Simulator.hpp"

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
            asts.push_back(program);
        }
    }

    //partition class and interface declarations into two vectors
    std::vector<std::shared_ptr<ClassDecl>> classDecls;
    std::vector<std::shared_ptr<InterfaceDecl>> interfaceDecls;
    for (auto program : asts) {
        if (!program) continue;
        if (program->classOrInterfaceDecl) {
            std::shared_ptr<ClassDecl> classDecl = std::dynamic_pointer_cast<ClassDecl>(program->classOrInterfaceDecl);
            if (classDecl != nullptr) classDecls.push_back(classDecl);
            else {
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
        std::shared_ptr<TIR::NodeFactory_c> nodeFactory = std::make_shared<TIR::NodeFactory_c>();
        for (std::shared_ptr<Program> program : asts) {
            TransformVisitor tvisitor(nodeFactory);
            program->accept(&tvisitor);
            TIR::Simulator sim(tvisitor.getCompUnit());
            long result = sim.call("a", 2, 1);
            std::cout << "a(-1,1) evaluates to " << result << std::endl;
            break;
        }
    }

    if (error)
        return 42;
    else
        return 0;
}
