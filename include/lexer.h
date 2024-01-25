#pragma once
#include <string>
#ifndef yyFlexLexerOnce // https://stackoverflow.com/questions/40663527/how-to-inherit-from-yyflexlexer
#include <FlexLexer.h>
#endif
#include "location.h"

namespace yy
{
    class MyLexer : public yyFlexLexer
    {
    public:
        MyLexer() { }
        MyLexer(std::istream &in) : yyFlexLexer(&in) { }
        int yylex(std::string *const yylval, location *const yylloc);
    };
}
