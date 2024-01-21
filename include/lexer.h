#pragma once
#include <string>

#ifndef yyFlexLexerOnce // https://stackoverflow.com/questions/40663527/how-to-inherit-from-yyflexlexer
#include <FlexLexer.h>
#endif

class MyLexer : public yyFlexLexer
{
public:
    int yylex(std::string *const yylval);
};