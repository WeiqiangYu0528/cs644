%option c++
%option noyywrap
%option yyclass="MyLexer"

%{
    #include "lexer.h"
    #include "parser.tab.hpp"
    #undef  YY_DECL 
    #define YY_DECL int MyLexer::yylex(std::string *const yylval)
    #define TOKEN(tok) yy::parser::token::tok
%}

DIGIT   [0-9]
INTEGER  [-]?{DIGIT}+
LETTER  [a-zA-Z]
ID      {LETTER}|_({LETTER}|{DIGIT})*
WHITESPACE [ \t\n\r]+ 

%%

"int"                   { return TOKEN(INT); }
"short"                 { return TOKEN(SHORT); }
"="                     { return TOKEN(ASSIGN); }
";"                     { return TOKEN(SEMICOLON); }
{ID}                    { *yylval = std::string(yytext); return TOKEN(IDENTIFIER); }
{INTEGER}               { *yylval = std::string(yytext); return TOKEN(INTEGER); }
{WHITESPACE}              ;
.                       { }

%%