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
%token LEFT_BRACKET RIGHT_BRACKET COMMA AT LEFT_BRACE RIGHT_BRACE SEMICOLON
%token EXTENDS SUPER
%token PUBLIC PROTECTED PRIVATE STATIC ABSTRACT FINAL NATIVE SYNCHRONIZED TRANSIENT VOLATILE STRICTFP
%token IMPORT PACKAGE INTERFACE RETURN VOID NEW
%token IF ELSE WHILE FOR
%%

Program
    :
    | Program ClassDeclaration
    ;

QualifiedIdentifier 
    : IDENTIFIER
    | QualifiedIdentifier DOT IDENTIFIER
    ;

QualifiedIdentifierList
    : QualifiedIdentifier
    | QualifiedIdentifierList COMMA QualifiedIdentifier 
    ;

ClassDeclaration
    : ClassModifiers NormalClassDeclaration
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

Statement:
    Block
    | SEMICOLON
    | IF ParExpression Statement ElseStatement
    | WHILE ParExpression Statement
    | FOR LEFT_PAREN ForControl RIGHT_PAREN Statement
    ;

ElseStatement:
    | ELSE Statement
    ;

ForControl:
    ForInit SEMICOLON ForExpression SEMICOLON ForUpdate
    ;

ForInit: 
    ;

ForExpression:
    | Expression
    ;

ForUpdate:
    ;

Expression:
    ;

ParExpression:
    LEFT_PAREN Expression RIGHT_PAREN
    ;

/* Modifier
    : Annotation */

ClassModifiers:
    ClassAccessModifiers ClassNonAccessModifiers
    ;

ClassAccessModifiers:
    | PUBLIC
    ;

ClassNonAccessModifiers:
    | FINAL
    | ABSTRACT
    ;

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
    : SEMICOLON
    | EmptyMethodDeclaration
    | MethodDeclaration
    | FieldDeclaration
    ;

EmptyMethodDeclaration
    :
    ClassBodyDeclarationOpt1 ClassBodyDeclarationOpt2 EmptyMethodDecl
    ;

MethodDeclaration:
    ClassBodyDeclarationOpt1 MethodDecl
    | ClassBodyDeclarationOpt1 ClassBodyDeclarationOpt3 MethodDecl
    | ClassBodyDeclarationOpt1 ClassBodyDeclarationOpt4 MethodDecl
    ;

FieldDeclaration:
    ClassBodyDeclarationOpt1 FieldDecl
    | ClassBodyDeclarationOpt1 ClassBodyDeclarationOpt3 FieldDecl
    ;

ClassBodyDeclarationOpt1
    : 
    PUBLIC
    | PROTECTED
    ;

ClassBodyDeclarationOpt2
    : ABSTRACT
    | STATIC NATIVE
    ;

ClassBodyDeclarationOpt3
    : 
    STATIC
    ;

ClassBodyDeclarationOpt4
    : 
    FINAL
    ;

MethodOrFieldDecl:
    Type IDENTIFIER 
    ;

EmptyMethodDecl
    : MethodOrFieldDecl FormalParameters SEMICOLON
    ;
 
MethodDecl:
    MethodOrFieldDecl MethodDeclaratorRest
    | IDENTIFIER ConstructorDeclaratorRest
    ;

FieldDecl:
    MethodOrFieldDecl FieldDeclaratorsRest SEMICOLON
    ;

MethodDeclaratorRest:
    FormalParameters Block
    ;

FieldDeclaratorsRest:
    VariableDeclaratorRest
    ;

VariableDeclaratorRest:
    | EQUAL VariableInitializer
    ;

VariableInitializer:
    ;

ConstructorDeclaratorRest:
    FormalParameters Block
    ;

FormalParameters: 
    LEFT_PAREN FormalParameterDecls RIGHT_PAREN
    ;

FormalParameterDecls:
    | Type FormalParameterDeclsRest
    ;

FormalParameterDeclsRest: 
    VariableDeclaratorId
    | VariableDeclaratorId COMMA FormalParameterDecls
    ;

VariableDeclaratorId:
    IDENTIFIER 
    | IDENTIFIER MultiDimensionArray
    ;

MultiDimensionArray:
    LEFT_BRACKET RIGHT_BRACKET
    | MultiDimensionArray LEFT_BRACKET RIGHT_BRACKET
    ;

Block:
    LEFT_BRACE BlockStatements RIGHT_BRACE
    ;

BlockStatements:
    | BlockStatement
    ;

BlockStatement:
    Statement
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
