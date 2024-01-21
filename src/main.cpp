#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
    std::ifstream inputFile;
    std::istream* input = &std::cin;

    if (argc == 3 && std::string(argv[1]) == "-i") {
        inputFile.open(argv[2]);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Unable to open file " << argv[2] << std::endl;
            return 1;
        }
        input = &inputFile;
    }

    yy::MyLexer lexer(*input);
    yy::parser parser(lexer);
    return parser.parse();
}