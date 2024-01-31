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
ASCII_CHAR [\x00-\x7F]

%%
{NEWLINE}               { yylloc->lines(1); yylloc->step(); yylloc->columns(0); prev_token_length = 0;  }
";"                     { update_yylloc; return Token::SEMICOLON; }
"."                     { update_yylloc; return Token::DOT; }
"{"                     { update_yylloc; return Token::LEFT_BRACE; }
"}"                     { update_yylloc; return Token::RIGHT_BRACE; }
"("                     { update_yylloc; return Token::LEFT_PAREN; }
")"                     { update_yylloc; return Token::RIGHT_PAREN; }
"["                     { update_yylloc; return Token::LEFT_BRACKET; }
"]"                     { update_yylloc; return Token::RIGHT_BRACKET; }

"abstract"              { update_yylloc; return Token::ABSTRACT; }
"int"                   { update_yylloc; return Token::INT; }
"boolean"               { update_yylloc; return Token::BOOLEAN; }
"char"                  { update_yylloc; return Token::CHAR; }
"byte"                  { update_yylloc; return Token::BYTE; }
"short"                 { update_yylloc; return Token::SHORT; }
"true"                  { update_yylloc; return Token::TRUE; }
"false"                 { update_yylloc; return Token::FALSE; }
"'"                     { update_yylloc; return Token::APOSTROPHE; } 
"""                     { update_yylloc; return Token::QUOTE; }
"||"                    { update_yylloc; return Token::OR_OR; }
"&&"                    { update_yylloc; return Token::AND_AND; }
"|"                     { update_yylloc; return Token::OR; }
"&"                     { update_yylloc; return Token::AND; }
"^"                     { update_yylloc; return Token::XOR; }
"=="                    { update_yylloc; return Token::EQUAL; }
"!="                    { update_yylloc; return Token::NOT_EQUAL; }
"<"                     { update_yylloc; return Token::LESS; }
">"                     { update_yylloc; return Token::GREATER; }
"<="                    { update_yylloc; return Token::LESS_EQUAL; }
">="                    { update_yylloc; return Token::GREATER_EQUAL; }
"instanceof"            { update_yylloc; return Token::INSTANCEOF; }
"+"                     { update_yylloc; return Token::PLUS; }
"-"                     { update_yylloc; return Token::MINUS; }
"*"                     { update_yylloc; return Token::STAR; }
"/"                     { update_yylloc; return Token::DIVIDE; }
"%"                     { update_yylloc; return Token::MODULO; }
"!"                     { update_yylloc; return Token::NOT; }
"super"                 { update_yylloc; return Token::SUPER; }
"="                     { update_yylloc; return Token::ASSIGN; }
"native"                { update_yylloc; return Token::NATIVE; }
"class"                 { update_yylloc; return Token::CLASS; }
"interface"             { update_yylloc; return Token::INTERFACE; }
"public"                { update_yylloc; return Token::PUBLIC; }
"protected"             { update_yylloc; return Token::PROTECTED; }
"static"                { update_yylloc; return Token::STATIC; }
"final"                 { update_yylloc; return Token::FINAL; }
"extends"               { update_yylloc; return Token::EXTENDS; }
"implements"            { update_yylloc; return Token::IMPLEMENTS; }
"import"                { update_yylloc; return Token::IMPORT; }
"package"               { update_yylloc; return Token::PACKAGE; }
"return"                { update_yylloc; return Token::RETURN; }
"void"                  { update_yylloc; return Token::VOID; }
"new"                   { update_yylloc; return Token::NEW; }
"this"                  { update_yylloc; return Token::THIS; }
"null"                  { update_yylloc; return Token::NULL; }
(0|[1-9]{DIGIT}*)       { update_yylloc; return Token::INTEGER; }
\\([btnfr]|([0-3]?[0-7])?[0-7]) { update_yylloc; return Token::ESCAPE; }
ASCII_CHAR              { update_yylloc; return Token::ASCII; }
{IDENTIFIER}            { update_yylloc; *yylval = std::string(yytext); return Token::IDENTIFIER; }
{WHITESPACE}            { update_yylloc; }
.                       { update_yylloc; }
%%

