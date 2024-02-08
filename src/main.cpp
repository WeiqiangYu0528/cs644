#include "parser.h"
#include "visitor.hpp"
#include <fstream>
#include <iostream>


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
    std::ifstream inputFile;

    inputFile.open(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << argv[1] << std::endl;
        return 1;
    }

    std::string filename{extractFilename(argv[1])};
    yy::MyLexer lexer(*static_cast<std::istream*>(&inputFile), filename);
    Ast ast;
    yy::parser parser(lexer, ast);
    int ret = parser.parse();
    Visitor visitor;
    std::shared_ptr<Exp> exp = ast.getAst();
    if (exp != nullptr) {
        std::cout << "Visiting AST" << std::endl;
    }
    exp->accept(&visitor);
    if (ret == 0)
        return 0;
    else if (ret == 1)
        return 42;
    else
        return 1;
}
