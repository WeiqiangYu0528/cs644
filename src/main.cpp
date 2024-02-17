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

    if (!error) {
        for (std::shared_ptr<Program> program : asts) {
            auto [pkg, cdecl] = program->getQualifiedName();
            auto x = tables[pkg][cdecl];
            if (x) {
                HierarchyVisitor hvisitor(tables[pkg][cdecl], tables);
                program->accept(&hvisitor);
                if (hvisitor.isError()) {
                    std::cerr << "Error: Hierarchy Checking failed" << std::endl;
                    error = true;
                    break;
                }
            }
        }
    }

    if (error)
        return 42;
    else
        return 0;
}
