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
%define parse.error detailed

%parse-param {MyLexer &lexer}


%code {
    #define yylex lexer.yylex
}

%start Program

%token IDENTIFIER 
%token DOT
%token ASSIGN
%token INTEGER 	
%token OR_OR AND_AND OR XOR AND EQUAL NOT_EQUAL LESS GREATER LESS_EQUAL GREATER_EQUAL 
%token LEFT_SHIFT RIGHT_SHIFT UNSIGNED_RIGHT_SHIFT PLUS MINUS MULTIPLY DIVIDE MODULO
%token LEFT_PAREN RIGHT_PAREN COLON QUESTION_MARK
%token THIS
%token CLASS IMPLEMENTS
%token INCREMENT DECREMENT NOT BITWISE_NOT
%token BYTE SHORT CHAR INT LONG FLOAT DOUBLE BOOLEAN
%token LEFT_BRACKET RIGHT_BRACKET COMMA AT LEFT_BRACE RIGHT_BRACE
%token EXTENDS SUPER
%token PUBLIC PROTECTED PRIVATE STATIC ABSTRACT FINAL NATIVE SYNCHRONIZED TRANSIENT VOLATILE STRICTFP
%%

Program
    :
    | Program NormalClassDeclaration
    ;

QualifiedIdentifier 
    : IDENTIFIER
    | QualifiedIdentifier DOT IDENTIFIER
    ;

QualifiedIdentifierList
    : QualifiedIdentifier
    | QualifiedIdentifierList COMMA QualifiedIdentifier 
    ;

NormalClassDeclaration
    : CLASS IDENTIFIER NormalClassDeclarationOpt1 NormalClassDeclarationOpt2 NormalClassDeclarationOpt3 ClassBody
    ;
NormalClassDeclarationOpt1
    :
    | TypeParameters
    ;    
NormalClassDeclarationOpt2
    :
    | EXTENDS Type
    ;
NormalClassDeclarationOpt3
    :
    | IMPLEMENTS TypeList
    ;


Type
    : BasicType BracketsOpt
    | ReferenceType BracketsOpt
    ;
BracketsOpt
    :
    | LEFT_BRACKET RIGHT_BRACKET
    ;    

BasicType
    : BYTE 
    | SHORT 
    | CHAR
    | INT
    | LONG 
    | FLOAT 
    | DOUBLE 
    | BOOLEAN
    ;

ReferenceType
    : IDENTIFIER ReferenceTypeOpt1 ReferenceTypeOpt2
    ;
ReferenceTypeOpt1
    :
    | TypeArguments
    ;
ReferenceTypeOpt2
    :
    | DOT IDENTIFIER ReferenceTypeOpt1 ReferenceTypeOpt2
    ;

TypeArguments
    : LESS TypeArgument TypeArgumentsOpt1 GREATER
    ;
TypeArgumentsOpt1
    : 
    | COMMA TypeArgument TypeArgumentsOpt1
    ;

TypeArgument
    : ReferenceType
    | QUESTION_MARK LEFT_BRACKET TypeArgumentOpt1 ReferenceType RIGHT_BRACKET
    ;
TypeArgumentOpt1
    : EXTENDS
    | SUPER
    ;

TypeList
    : ReferenceType
    | TypeList COMMA ReferenceType
    ;

TypeParameters
    : LESS TypeParameter TypeParametersOpt1 GREATER
    ;
TypeParametersOpt1
    : 
    | COMMA TypeParameter TypeParametersOpt1
    ;

TypeParameter
    : IDENTIFIER TypeParameterOpt1
    ;
TypeParameterOpt1
    : 
    | EXTENDS Bound
    ;

Bound  
    : ReferenceType
    | Bound AND ReferenceType
    ;


/* Modifier
    : Annotation */
Modifier
    : PUBLIC
    | PROTECTED
    | PRIVATE
    | STATIC
    | ABSTRACT
    | FINAL
    | NATIVE
    | SYNCHRONIZED
    | TRANSIENT
    | VOLATILE
    | STRICTFP

/* Annotations
    : Annotation
    | Annotations Annotation
    ;

Annotation
    :
    ; */

ClassBody
    : LEFT_BRACE ClassBodyOpt1 RIGHT_BRACE 
    ;
ClassBodyOpt1
    :
    | ClassBodyOpt1 ClassBodyDeclaration
    ;

ClassBodyDeclaration
    : COMMA
    /* | ClassBodyDeclarationOpt1 MemberDecl
    | ClassBodyDeclarationOpt2 Block */
    ;
ClassBodyDeclarationOpt1
    : 
    | ClassBodyDeclarationOpt1 Modifier
    ;
ClassBodyDeclarationOpt2
    : 
    | STATIC
    ;

MemberDecl:
    ;
Block:
    ;

Literal
    : IntegerLiteral
    ;
IntegerLiteral
    : INTEGER
    ;

%%


void yy::parser::error(const location &location, const std::string &message)
{
    std::cerr << "Error at line " << location.end.line << " col " << location.end.column << ": " << message << std::endl;
}