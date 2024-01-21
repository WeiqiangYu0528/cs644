#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <iostream>


int main(int argc, char* argv[])
{
    std::ifstream inputFile;

    inputFile.open(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << argv[2] << std::endl;
        return 1;
    }
 
    yy::MyLexer lexer(*static_cast<std::istream*>(&inputFile));
    yy::parser parser(lexer);
    int ret = parser.parse();
    if (ret == 0)
        return 0;
    else if (ret == 1)
        return 42;
    else
        return 1;
}
