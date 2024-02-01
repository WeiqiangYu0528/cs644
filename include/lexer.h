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
    private:
        std::string filename;
    public:
        MyLexer() { }
        MyLexer(std::istream &in, std::string& fn) : yyFlexLexer(&in), filename(fn) { }
        int yylex(void * yylval, location *const yylloc);
        std::string getFilename() { return filename; }
    };
}
