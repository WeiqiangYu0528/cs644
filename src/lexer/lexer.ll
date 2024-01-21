%option c++
%option noyywrap
%option yyclass="MyLexer"

%{
    #include "lexer.h"
    #include "parser.h"
    #undef  YY_DECL 
    #define YY_DECL int yy::MyLexer::yylex(std::string *const yylval, location *const yylloc)
    using Token = yy::parser::token;  
    int prev_token_length = 0;
    #define update_yylloc do { \
        yylloc->columns(prev_token_length); \
        prev_token_length = strlen(yytext); \
    } while(0)


%}

DIGIT   [0-9]
LETTER  [a-zA-Z]
IDENTIFIER  ({LETTER}|_)({LETTER}|{DIGIT}|_)*
NEWLINE \n
WHITESPACE [ \t\r]+ 

%%
{NEWLINE}               { yylloc->lines(1); yylloc->step(); yylloc->columns(0); prev_token_length = 0;  }
"."                     { update_yylloc; return Token::DOT; }

"{"                     { update_yylloc; return Token::LEFT_BRACE; }
"}"                     { update_yylloc; return Token::RIGHT_BRACE; }
"class"                 { update_yylloc; return Token::CLASS; }
"public"                { update_yylloc; return Token::PUBLIC; }
[-]?{DIGIT}+            { update_yylloc; return Token::INTEGER; }
{IDENTIFIER}            { update_yylloc; *yylval = std::string(yytext); return Token::IDENTIFIER; }
{WHITESPACE}            { update_yylloc; }
.                       { update_yylloc; }
%%

