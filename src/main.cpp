#include "lexer.h"
#include "parser.tab.hpp"

int main()
{
    MyLexer lexer;
    yy::parser parser(lexer);
    return parser.parse();
}
