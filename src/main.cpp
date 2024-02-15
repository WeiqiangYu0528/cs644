#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "parser.h"
#include "contextVisitor.hpp"
#include "symbolTable.hpp"

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
    std::unordered_map<std::string, std::shared_ptr<SymbolTable>> tables;
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
            std::string key{program->getQualifiedName()};
            if (cvisitor.isError() || tables.contains(key)) {
                error = true;
                break;
            }
            tables[key] = cvisitor.getSymbolTable();
        }
    }
    if (error)
        return 42;
    else
        return 0;
}
