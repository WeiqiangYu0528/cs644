%require "3.2"
%language "c++"

%code requires {
    #include <string>
    #include "lexer.h"
}

%define api.value.type {std::string}

%parse-param {MyLexer &lexer}


%code {
    #define yylex lexer.yylex
}


%token INT SHORT IDENTIFIER ASSIGN SEMICOLON INTEGER

%%
program:
    | program statement
    ;

statement:
    integer_type_declaration IDENTIFIER ASSIGN INTEGER SEMICOLON
    {       
         std::cout << "Declared " << $1 << " " << $2 << ", set to " << $4 << std::endl;
    };

integer_type_declaration:
    SHORT { $$ = std::string("short variable"); }
    | INT {$$ = std::string("int variable");}
    ;

%%


void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}