%require "3.2"
%language "c++"
%define lr.type lalr

%code requires {
    #include <string>
    #include "lexer.h"
}

%define api.value.type {std::string}

%locations
%define api.location.file "../../include/location.h"
%define api.location.include {"location.h"}
%header "include/parser.h"

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


void yy::parser::error(const location &location, const std::string &message)
{
    std::cerr << "Error at lines " << location << ": " << message << std::endl;
}