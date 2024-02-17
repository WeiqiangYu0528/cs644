#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "parser.h"
#include "contextVisitor.hpp"
#include "symbolTable.hpp"
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

bool detectCycle(std::string current, std::unordered_set<std::string>& visited, std::unordered_set<std::string>& pathVisited, 
std::unordered_map<std::string, std::vector<std::string>>& edges) {
    visited.insert(current);
    pathVisited.insert(current);
    for (std::string neighbour : edges[current]) {
        if (pathVisited.contains(neighbour)) return true;
        else if (visited.contains(neighbour)) continue;
        else if (detectCycle(neighbour, visited, pathVisited, edges)) return true;
    }
    pathVisited.erase(current);
    return false;
}
bool detectCycleWrapper(std::vector<std::string>& vertices,
std::unordered_map<std::string, std::vector<std::string>>& edges) {
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> pathVisited;
    for (std::string vertex : vertices) {
        if (!edges.contains(vertex)) continue;
        if (visited.contains(vertex)) continue;
        else {
            bool result = detectCycle(vertex, visited, pathVisited, edges);
            if (result) return result;
        }
    }
    return false;
}

int main(int argc, char* argv[])
{
    std::unordered_map<std::string, 
                       std::unordered_map<std::string, std::shared_ptr<SymbolTable>>> tables;
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
            if (cvisitor.isError() || (tables.contains(pkg) && tables.at(pkg).contains(cdecl))) {
                error = true;
                break;
            }
            tables[pkg][cdecl] = cvisitor.getSymbolTable();
            asts.push_back(program);
        }
    }

    for (std::shared_ptr<Program> program : asts) {
        auto [pkg, cdecl] = program->getQualifiedName();
        auto x = tables[pkg][cdecl];
        if (x) {
            HierarchyVisitor hvisitor(tables[pkg][cdecl], tables);
            program->accept(&hvisitor);
            error |= hvisitor.isError();
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
    //classes first
    std::vector<std::string> classVertices;
    std::unordered_map<std::string, std::vector<std::string>> classEdges;
    for (auto cdecl : classDecls) {
        classVertices.push_back(cdecl->id->name); //add all classes to vector of vertices
        classEdges[cdecl->id->name] = std::vector<std::string>{}; //initialize edges vector for each class
        for (auto ext : cdecl->extended) { //go through neighbours
            classEdges[cdecl->id->name].push_back(ext->id->name); //add them to the list of edges
        }
    }

    //interfaces second
    std::vector<std::string> interfaceVertices;
    std::unordered_map<std::string, std::vector<std::string>> interfaceEdges;
    for (auto idecl : interfaceDecls) {
        interfaceVertices.push_back(idecl->id->name); //add all interfaces to vector of vertices
        interfaceEdges[idecl->id->name] = std::vector<std::string>{}; //initialize edges vector for each interface
        for (auto ext : idecl->extended) { //go through neighbours
            interfaceEdges[idecl->id->name].push_back(ext->id->name); //add them to the list of edges
        }
    }

    if (detectCycleWrapper(interfaceVertices, interfaceEdges)) {
        std::cerr << "Detected a cycle among the interfaces" << std::endl;
        error = true;
    }

    if (detectCycleWrapper(interfaceVertices, interfaceEdges)) {
        std::cerr << "Detected a cycle among the interfaces" << std::endl;
        error = true;
    }

    if (detectCycleWrapper(classVertices, classEdges)) {
        std::cerr << "Detected a cycle among the classes" << std::endl;
        error = true;
    }

    if (!error) {
        for (auto ast : asts) {
            TypeLinkingVisitor tvisitor{tables};
            ast->accept(&tvisitor);
            if (tvisitor.isError()) {
                std::cerr << "Error: Type linking failed" << std::endl;
                error = true;
                break;
            }
        }
    }
    if (error)
        return 42;
    else
        return 0;
}
