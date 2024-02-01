%require "3.2"
%language "c++"
%define lr.type lalr

%code requires {
    #include <string>
    #include <vector>
    #include <variant>
    #include "lexer.h"
    #include "weeder.hpp"
}

%define api.value.type variant

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
%token SWITCH CASE TRY CATCH FINALLY DEFAULT DO BREAK CONTINUE THROW
%token TRUE FALSE

%nonassoc THEN
%nonassoc ELSE

%type <Modifiers> Modifier
%type <std::vector<int>> ModifierOptions ClassBodyDeclarationOpt1
%type <MemberType> MemberDecl MethodOrFieldDecl MethodOrFieldRest MethodDeclaratorRest

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
    : ModifierOptions NormalClassDeclaration {
        for (int val: $1) {
            if (val > 1) throw syntax_error(@1, std::string("Duplicate modifier is not allowed.")); 
        }
        if ($1[1] > 0 && $1[2] > 0) throw syntax_error(@1, std::string("A class cannot be both abstract and final.")); 
    }
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
    | IF ParExpression Statement %prec THEN
    | IF ParExpression Statement ELSE Statement

    | WHILE ParExpression Statement
    | FOR LEFT_PAREN ForControl RIGHT_PAREN Statement
    ;

ForControl:
    ForInit SEMICOLON ForExpression SEMICOLON ForUpdate
    ;

ForExpression:
    | Expression
    ;

ForUpdate:
    | StatementExpression
    ;

ForInit: 
    | StatementExpression
    | LocalVariableDeclaration
    ;

LocalVariableDeclaration :
    Type VariableDeclarator
    ;

VariableDeclarator :
    IDENTIFIER EQUAL VariableInitializer
    ;

VariableInitializer:
    Expression
    ;

Assignment:
    IDENTIFIER EQUAL Expression
    ;

Expression:
    TRUE
    | FALSE
    | MethodInvocation 
    ;

ParExpression:
    LEFT_PAREN Expression RIGHT_PAREN
    ;

StatementExpression:
    Assignment
    | MethodInvocation 
    | ClassInstanceCreationExpression
    ;

MethodInvocation:
    IDENTIFIER LEFT_PAREN ArgumentList RIGHT_PAREN
    ;

ClassInstanceCreationExpression:
    NEW IDENTIFIER LEFT_PAREN ArgumentList RIGHT_PAREN
    ;

ArgumentList: 
    ;

/* Modifier
    : Annotation */
Modifier: 
    PUBLIC {$$ = Modifiers::PUBLIC;}
    | PROTECTED {$$ = Modifiers::PROTECTED;}
    | STATIC {$$ = Modifiers::STATIC;}
    | ABSTRACT  {$$ = Modifiers::ABSTRACT;}
    | FINAL {$$ = Modifiers::FINAL;}
    | NATIVE {$$ = Modifiers::NATIVE;}
    ;

ModifierOptions
    : {$$ = std::vector<int>(3, 0);}
    | Modifier ModifierOptions {
        $$ = $2;
        switch ($1) {
            case Modifiers::PUBLIC: {$$[0]++; break;};
            case Modifiers::FINAL: {$$[1]++; break;}
            case Modifiers::ABSTRACT: {$$[2]++; break;}
            default: throw syntax_error(@1, std::string("Invalid modifier for class declaration"));
        }
    }
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
    | ClassBodyDeclarationOpt1 MemberDecl {
        for (int val: $1) {
            if (val > 1) throw syntax_error(@1, std::string("Duplicate modifier is not allowed.")); 
        }
        if ($1[0] > 0 && $1[1] > 0) throw syntax_error(@1, std::string("Multiple access modifiers are not allowed")); 
        if ($1[2] > 0 && ($1[3] > 0 || $1[4] > 0)) throw syntax_error(@1, std::string("An abstract method cannot be static or final.")); 
        if ($1[4] > 0 && $1[3] > 0) throw syntax_error(@1, std::string("A static method cannot be final.")); 
        if ($1[5] > 0 && $1[4] == 0) throw syntax_error(@1, std::string("A native method must be static."));
        if ($2 == MemberType::FIELD && ($1[2] + $1[3] + $1[5] > 0)) throw syntax_error(@1, std::string("No field can be final, abstract or native."));
        if ( ($2 == MemberType::METHODWITHBODY && ($1[2] > 0 || $1[5] > 0)) || 
             ($2 == MemberType::METHODWITHOUTBODY && ($1[2] == 0 && $1[5] == 0)) )
        throw syntax_error(@1, std::string("A method has a body if and only if it is neither abstract nor native"));
        if ($2 == MemberType::CONSTRUCTOR && ($1[2] + $1[3] + $1[4] + $1[5] > 0))
        throw syntax_error(@1, std::string("A constructor cannot be final, abstract, static or native"));
    }
    | ClassBodyDeclarationOpt2 Block
    ;

ClassBodyDeclarationOpt1
    : 
    Modifier {
        $$ = std::vector<int>(6, 0);
        if (!validateModifier($$, $1)) throw syntax_error(@1, std::string("Invalid modifier for member declaration"));
    }
    | Modifier ClassBodyDeclarationOpt1 {
        $$ = $2;
        if (!validateModifier($$, $1)) throw syntax_error(@1, std::string("Invalid modifier for member declaration"));
    }
    ;

ClassBodyDeclarationOpt2
    : 
    | STATIC
    ;

MemberDecl:
    MethodOrFieldDecl {$$ = $1;}
    | IDENTIFIER ConstructorDeclaratorRest {$$ = MemberType::CONSTRUCTOR;}
    ;

MethodOrFieldDecl:
    Type IDENTIFIER MethodOrFieldRest {$$ = $3;}
    ;

MethodOrFieldRest:  
    FieldDeclaratorsRest SEMICOLON {$$ = MemberType::FIELD;}
    | MethodDeclaratorRest
    ;

MethodDeclaratorRest:
    FormalParameters Block {$$ = MemberType::METHODWITHBODY;}
    | FormalParameters SEMICOLON  {$$ = MemberType::METHODWITHOUTBODY;}
    ;

FieldDeclaratorsRest:
    VariableDeclaratorRest
    ;

VariableDeclaratorRest:
    | EQUAL VariableInitializer

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

VariableDeclaratorId:
    IDENTIFIER 
    | IDENTIFIER MultiDimensionArray

MultiDimensionArray:
    LEFT_BRACKET RIGHT_BRACKET
    | MultiDimensionArray LEFT_BRACKET RIGHT_BRACKET
    ;

Block: 
    LEFT_BRACE BlockStatementsOpt RIGHT_BRACE
    ;

BlockStatementsOpt: 
    | BlockStatements
    ;

BlockStatements: 
    BlockStatement
    | BlockStatements BlockStatement
    ;

BlockStatement: 
    LocalVariableDeclarationStatement
    | Statement
    ;

LocalVariableDeclarationStatement: 
    LocalVariableDeclaration SEMICOLON
    ;

LocalVariableDeclaration: 
    Type VariableDeclarators
    ;

Statement: 
    StatementWithoutTrailingSubstatement
    | LabeledStatement
    | IfThenStatement
    | IfThenElseStatement
    | WhileStatement
    | ForStatement
    ;

StatementNoShortIf: 
    StatementWithoutTrailingSubstatement
    | LabeledStatementNoShortIf
    | IfThenElseStatementNoShortIf
    | WhileStatementNoShortIf
    | ForStatementNoShortIf
    ;

StatementWithoutTrailingSubstatement: 
    Block
    | EmptyStatement
    | ExpressionStatement
    | SwitchStatement
    | DoStatement
    | BreakStatement
    | ContinueStatement
    | ReturnStatement
    | SynchronizedStatement
    | ThrowsStatement
    | TryStatement
    ;

EmptyStatement: 
    SEMICOLON
    ;

LabeledStatement: 
    IDENTIFIER COLON Statement
    ;

LabeledStatementNoShortIf: 
    IDENTIFIER COLON StatementNoShortIf
    ;

ExpressionStatement: 
    StatementExpression SEMICOLON
    ;

StatementExpression: 
    Assignment
    /* | PreincrementExpression
    | PostincrementExpression
    | PredecrementExpression
    | PostdecrementExpression
    | MethodInvocation
    | ClassInstanceCreationExpression */
    ;

IfThenStatement: 
    IF LEFT_PAREN Expression RIGHT_PAREN Statement
    ;

IfThenElseStatement: 
    IF LEFT_PAREN Expression RIGHT_PAREN StatementNoShortIf ELSE Statement
    ;

IfThenElseStatementNoShortIf: 
    IF LEFT_PAREN Expression RIGHT_PAREN StatementNoShortIf ELSE StatementNoShortIf
    ;

SwitchStatement: 
    SWITCH LEFT_PAREN Expression RIGHT_PAREN SwitchBlock
    ;

SwitchBlock: 
    LEFT_BRACE SwitchBlockStatementGroupsOpt SwitchLabelsOpt RIGHT_BRACE
    ;

SwitchBlockStatementGroupsOpt:
    | SwitchBlockStatementGroups
    ;

SwitchLabelsOpt:
    | SwitchLabels
    ;


SwitchBlockStatementGroups: 
    SwitchBlockStatementGroup
    | SwitchBlockStatementGroups SwitchBlockStatementGroup
    ;

SwitchBlockStatementGroup: 
    SwitchLabels BlockStatements
    ;

SwitchLabels: 
    SwitchLabel
    | SwitchLabels SwitchLabel
    ;

SwitchLabel: 
    CASE ConstantExpression COLON
    | DEFAULT COLON
    ;

WhileStatement: 
    WHILE LEFT_PAREN Expression RIGHT_PAREN Statement
    ;

WhileStatementNoShortIf: 
    WHILE LEFT_PAREN Expression RIGHT_PAREN StatementNoShortIf
    ;

DoStatement: 
    DO Statement WHILE LEFT_PAREN Expression RIGHT_PAREN SEMICOLON
    ;

ForStatement: 
    FOR LEFT_PAREN ForInitOpt SEMICOLON ExpressionOpt SEMICOLON ForUpdateOpt RIGHT_PAREN Statement;

ForInitOpt:
    | ForInit
    ;

ExpressionOpt:
    | Expression
    ;

ForUpdateOpt:
    | ForUpdate
    ;

ForInit: 
    StatementExpressionList
    | LocalVariableDeclaration
    ;

ForUpdate: 
    StatementExpressionList
    ;


ForStatementNoShortIf: 
    FOR LEFT_PAREN ForInitOpt SEMICOLON ExpressionOpt SEMICOLON ForUpdateOpt RIGHT_PAREN StatementNoShortIf
    ;

StatementExpressionList: 
    StatementExpression
    | StatementExpressionList COMMA StatementExpression
    ;

IdentifierOpt:
    | IDENTIFIER
    ;

BreakStatement: 
    BREAK IdentifierOpt SEMICOLON
    ;

ContinueStatement: 
    CONTINUE IdentifierOpt SEMICOLON
    ;

ReturnStatement: 
    RETURN ExpressionOpt SEMICOLON
    ;

ThrowsStatement: 
    THROW Expression SEMICOLON
    ;

SynchronizedStatement: 
    SYNCHRONIZED LEFT_PAREN Expression RIGHT_PAREN Block
    ;

TryStatement: 
    TRY Block Catches
    | TRY Block CatchesOpt Finally
    ;

CatchesOpt:
    | Catches
    ;

Catches: 
    CatchClause
    | Catches CatchClause
    ;

CatchClause: 
    CATCH LEFT_PAREN FormalParameter RIGHT_PAREN Block
    ;

Finally: 
    FINALLY Block
    ;


Expression:  // Dummy Expression!
    IDENTIFIER SEMICOLON    
    ;               

Assignment: // Dummy Assignment!
    IDENTIFIER ASSIGN IDENTIFIER
    | IDENTIFIER ASSIGN IntegerLiteral
    ;

ConstantExpression:
    Expression
    ;

Literal
    : IntegerLiteral
    ;
IntegerLiteral
    : INTEGER
    ;

FormalParameter:
    ;

VariableDeclarators:        
    ;




%%


void yy::parser::error(const location &location, const std::string &message)
{
    std::cerr << "Error at line " << location.end.line << " col " << location.end.column << ": " << message << std::endl;
}
