%option c++
%option noyywrap
%option yyclass="MyLexer"

%{
    #include "lexer.h"
    #include "parser.h"
    #undef  YY_DECL 
    #define YY_DECL int yy::MyLexer::yylex(std::string *const yylval, location *const yylloc)
    using Token = yy::parser::token;  
%}

DIGIT   [0-9]
INTEGER  [-]?{DIGIT}+
LETTER  [a-zA-Z]
ID      {LETTER}|_({LETTER}|{DIGIT})*
WHITESPACE [ \t\n\r]+ 

%%

"int"                   { return Token::INT; }
"short"                 { return Token::SHORT; }
"="                     { return Token::ASSIGN; }
";"                     { return Token::SEMICOLON; }
{ID}                    { *yylval = std::string(yytext); return Token::IDENTIFIER; }
{INTEGER}               { *yylval = std::string(yytext); return Token::INTEGER; }
{WHITESPACE}              ;
.                       { }

%%