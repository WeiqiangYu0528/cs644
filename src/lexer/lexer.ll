%option c++
%option noyywrap
%option yyclass="MyLexer"
%x java_comment

%{
    #include "lexer.h"
    #include "parser.h"
    #include <variant>
    #undef  YY_DECL 
    #define YY_DECL int yy::MyLexer::yylex(void * yylval, location *const yylloc)
    using Token = yy::parser::token;  
    int prev_token_length = 0;
    #define update_yylloc do { \
        yylloc->columns(prev_token_length); \
        prev_token_length = strlen(yytext); \
    } while(0)
    #define update_linenumber do { \
        for(int i = 0; i < yyleng; ++i) { \
            if(yytext[i] == '\n') { \
                yylloc->lines(1); \
            } \
        } \
    } while(0)
%}

DIGIT   [0-9]
LETTER  [a-zA-Z]
IDENTIFIER  ({LETTER}|_)({LETTER}|{DIGIT}|_)*
NEWLINE \n
WHITESPACE [ \t\r]+ 

%%
[/][/].* { }
"/*"   { BEGIN(java_comment); }
<java_comment>[^*]*        { update_linenumber; }
<java_comment>"*"+[^*/]*   { update_linenumber; }
<java_comment>"*/"         { BEGIN(INITIAL); }

{NEWLINE}               { yylloc->lines(1); yylloc->step(); yylloc->columns(0); prev_token_length = 0;  }
","                     { update_yylloc; return Token::COMMA; }
";"                     { update_yylloc; return Token::SEMICOLON; }
"."                     { update_yylloc; return Token::DOT; }
"="                     { update_yylloc; return Token::ASSIGN; }
"{"                     { update_yylloc; return Token::LEFT_BRACE; }
"}"                     { update_yylloc; return Token::RIGHT_BRACE; }
"("                     { update_yylloc; return Token::LEFT_PAREN; }
")"                     { update_yylloc; return Token::RIGHT_PAREN; }
"["                     { update_yylloc; return Token::LEFT_BRACKET; }
"]"                     { update_yylloc; return Token::RIGHT_BRACKET; }
"="                     { update_yylloc; return Token::EQUAL; }
"abstract"              { update_yylloc; return Token::ABSTRACT; }
"int"                   { update_yylloc; return Token::INT; }
"boolean"               { update_yylloc; return Token::BOOLEAN; }
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
"if"                    { update_yylloc; return Token::IF; }
"else"                  { update_yylloc; return Token::ELSE; }
"while"                 { update_yylloc; return Token::WHILE; }
"for"                   { update_yylloc; return Token::FOR; }
"true"                  { update_yylloc; return Token::TRUE; }
"false"                 { update_yylloc; return Token::FALSE; }
[-]?{DIGIT}+            { update_yylloc; return Token::INTEGER; }
{IDENTIFIER}            { update_yylloc; yylval = new std::string(yytext); return Token::IDENTIFIER; }
{WHITESPACE}            { update_yylloc; }
.                       { update_yylloc; }
%%

