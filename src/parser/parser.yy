%require "3.2"
%language "c++"
%define lr.type lalr

%code requires {
    #include <string>
    #include <vector>
    #include <limits.h>
    #include "weeder.hpp"
    #ifndef yyFlexLexerOnce // https://stackoverflow.com/questions/40663527/how-to-inherit-from-yyflexlexer
    #include <FlexLexer.h>
    #endif

    namespace yy
    {
        class MyLexer;
    }
}

%define api.value.type variant

%locations
%define api.location.file "../../include/location.h"
%define api.location.include {"location.h"}
%header "include/parser.h"
%define parse.error detailed
%define parse.trace
%parse-param {MyLexer &lexer}


%code {
    #define yylex lexer.yylex
}


%code provides {
    namespace yy
    {
        class MyLexer : public yyFlexLexer 
        {
        private:
            std::string filename;
        public:
            MyLexer() { }
            MyLexer(std::istream &in, std::string& fn) : yyFlexLexer(&in), filename(fn) { }
            int yylex(parser::value_type *const yylval, location *const yylloc);
            inline std::string getFilename() const {return filename;}
        };
    }
}


%start Program

%token <std::string> IDENTIFIER 
%token DOT
%token STAR
%token ASSIGN
%token <std::string> INTEGER CHARACTER NUL
%token ESCAPE
%token OR_OR AND_AND OR XOR AND EQUAL NOT_EQUAL LESS GREATER LESS_EQUAL GREATER_EQUAL 
%token LEFT_SHIFT RIGHT_SHIFT UNSIGNED_RIGHT_SHIFT PLUS MINUS MULTIPLY DIVIDE MODULO
%token LEFT_PAREN RIGHT_PAREN COLON QUESTION_MARK
%token THIS INSTANCEOF
%token CLASS IMPLEMENTS
%token INCREMENT DECREMENT NOT BITWISE_NOT
%token BYTE SHORT CHAR INT BOOLEAN
%token LEFT_BRACKET RIGHT_BRACKET COMMA AT LEFT_BRACE RIGHT_BRACE SEMICOLON APOSTROPHE QUOTE
%token EXTENDS SUPER
%token PUBLIC PROTECTED PRIVATE STATIC ABSTRACT FINAL NATIVE SYNCHRONIZED TRANSIENT VOLATILE STRICTFP
%token IMPORT PACKAGE INTERFACE RETURN VOID NEW
%token IF ELSE WHILE FOR
%token TRUE FALSE STRING

%nonassoc THEN
%nonassoc ELSE

%type <std::string> Variable
%type <bool> ClassBodyDeclaration ClassBodyOpt1
%type <DataType> Type BasicType ReferenceType ResultType
%type <Modifiers> Modifier
%type <std::vector<int>> ClassBodyDeclarationOpt1
%type <MemberType> MemberDecl MethodOrFieldDecl MethodOrFieldRest MethodDeclaratorRest

%%

Program
    : PackageDeclaration ImportStatements ClassOrInterfaceDeclaration 
    ;

ClassOrInterfaceDeclaration
    :
    | ClassDeclaration
    | InterfaceDeclaration
    ;  

PackageDeclaration
    :
    | PACKAGE Name SEMICOLON
    ;

ImportStatements
    :
    | ImportStatement ImportStatements
    ;

ImportStatement
    : IMPORT Name SEMICOLON
    | IMPORT PackageImportIdentifier SEMICOLON
    ;

PackageImportIdentifier    
    : Name DOT STAR
    ;


InterfaceDeclaration 
    : PUBLIC InterfaceOpt1 INTERFACE Variable InterfaceOpt2 InterfaceBody {
        if ($4 != lexer.getFilename()) throw syntax_error(@1, std::string("An interface must be declared with the same filename."));
     }
    ;

InterfaceOpt1
    :
    | ABSTRACT
    ;

InterfaceOpt2
    :
    | EXTENDS Variable
    ;

InterfaceBody
    : LEFT_BRACE MethodDeclarations RIGHT_BRACE
    ;

MethodDeclarations 
    : 
    | MethodDeclaration MethodDeclarations
    ;

MethodDeclaration
    : PUBLIC MethodDeclarationOpt Type Variable FormalParameters SEMICOLON
    ;    

MethodDeclarationOpt
    : 
    | ABSTRACT
    ;

ClassDeclaration
    : PUBLIC ClassDeclarationOpt NormalClassDeclaration
    ;

ClassDeclarationOpt
    : 
    | ABSTRACT
    | FINAL
    ;

NormalClassDeclaration
    : CLASS Variable NormalClassDeclarationOpt1 NormalClassDeclarationOpt2 NormalClassDeclarationOpt3 ClassBody {
        if ($2 != lexer.getFilename()) throw syntax_error(@1, std::string("A class must be declared with the same filename."));
    }
    ;

NormalClassDeclarationOpt1
    :
    | TypeParameters
    ;

NormalClassDeclarationOpt2
    :
    | EXTENDS TypeList
    ;

NormalClassDeclarationOpt3
    :
    | IMPLEMENTS TypeList
    ;

Type
    : BasicType {$$ = $1;}
    | ReferenceType {$$ = $1;}
    ;   

BasicType
    : BYTE {$$ = DataType::BYTE;}
    | SHORT {$$ = DataType::SHORT;}
    | CHAR {$$ = DataType::CHAR;}
    | INT {$$ = DataType::INT;}
    | BOOLEAN {$$ = DataType::BOOLEAN;}
    ;

ReferenceType
    : ClassOrInterfaceType {$$ = DataType::OBJECT;}
    | ArrayType {$$ = DataType::ARRAY;}
    ;

Name:
    SimpleName 
    | QualifiedName;

SimpleName:
    Variable;

QualifiedName:
    Name DOT Variable;

ClassOrInterfaceType:
    Name;

ClassType:
    ClassOrInterfaceType;

ArrayType
    : BasicType LEFT_BRACKET RIGHT_BRACKET
    | Name LEFT_BRACKET RIGHT_BRACKET
    | ArrayType LEFT_BRACKET RIGHT_BRACKET
    ;

ResultType
    : Type {$$ = $1;}
    | VOID {$$ = DataType::VOID;}

// Do not use ReferenceType as arraytype is not supported here
TypeList
    : ClassOrInterfaceType
    | TypeList COMMA ClassOrInterfaceType
    ;

TypeParameters
    : LESS TypeParameter TypeParametersOpt1 GREATER
    ;

TypeParametersOpt1
    : 
    | COMMA TypeParameter TypeParametersOpt1
    ;

TypeParameter
    : Variable TypeParameterOpt1
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
    | ReturnStatements
    | ExpressionStatement
    ;

ExpressionStatement:
    StatementExpression SEMICOLON
    ;

VariableInitializers:
    VariableInitializer
    | VariableInitializers COMMA VariableInitializer
    ;

/* ArrayInitializer:
    LEFT_BRACE VariableInitializers RIGHT_BRACE
    | LEFT_BRACE COMMA RIGHT_BRACE
    | LEFT_BRACE RIGHT_BRACE
    ; */


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

LocalVariableDeclaration:
    Type VariableDeclarators
    ;

VariableDeclarators:
    VariableDeclarator
    ;

VariableDeclarator :
    VariableDeclaratorId
    | VariableDeclaratorId ASSIGN VariableInitializer
    ;

VariableDeclaratorId
    : Variable
    ;

VariableInitializer:
    Expression
//    | ArrayInitializer
    ;

Expression:
    AssignmentExpression
    ;

AssignmentExpression:
    ConditionalExpression
    | Assignment
    ;

Assignment:
    LeftHandSide ASSIGN AssignmentExpression
    ;

LeftHandSide:
    Name
    | FieldAccess
    | ArrayAccess
    ;


ConditionalExpression:
    ConditionalOrExpression
    | ConditionalOrExpression QUESTION_MARK Expression COLON ConditionalExpression { throw syntax_error(@1, std::string("conditional operator not supported")); }
    ;

ConditionalOrExpression:
    ConditionalAndExpression
    | ConditionalOrExpression OR_OR ConditionalAndExpression
    ;

ConditionalAndExpression:
    InclusiveOrExpression
    | ConditionalAndExpression AND_AND InclusiveOrExpression
    ;

InclusiveOrExpression:
    ExclusiveOrExpression
    | InclusiveOrExpression OR ExclusiveOrExpression
    ;

ExclusiveOrExpression:
    AndExpression
    | ExclusiveOrExpression XOR AndExpression
    ;

AndExpression:
    EqualityExpression
    | AndExpression AND EqualityExpression
    ;

EqualityExpression:
    RelationalExpression
    | EqualityExpression EQUAL RelationalExpression
    | EqualityExpression NOT_EQUAL RelationalExpression
    ;

RelationalExpression:
    ShiftExpression
    | RelationalExpression LESS ShiftExpression
    | RelationalExpression GREATER ShiftExpression
    | RelationalExpression LESS_EQUAL ShiftExpression
    | RelationalExpression GREATER_EQUAL ShiftExpression
    | RelationalExpression INSTANCEOF ReferenceType
    ;

ShiftExpression:
    AdditiveExpression
    | ShiftExpression LEFT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("left shift not supported")); }
    | ShiftExpression RIGHT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("right shift not supported")); }
    | ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("unsigned right shift not supported")); }
    ;

AdditiveExpression:
    MultiplicativeExpression
    | AdditiveExpression PLUS MultiplicativeExpression
    | AdditiveExpression MINUS MultiplicativeExpression
    ;

MultiplicativeExpression:
    UnaryExpression {
        if (integerVal > INT_MAX ) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        integerVal = 0;
    }
    | MultiplicativeExpression STAR UnaryExpression {
        if (integerVal > INT_MAX ) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        integerVal = 0;
    }
    | MultiplicativeExpression DIVIDE UnaryExpression {
        if (integerVal > INT_MAX ) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        integerVal = 0;
    }
    | MultiplicativeExpression MODULO UnaryExpression {
        if (integerVal > INT_MAX ) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        integerVal = 0;
    }
    ;

CastExpression:
    LEFT_PAREN BasicType Dims RIGHT_PAREN UnaryExpression 
    | LEFT_PAREN BasicType RIGHT_PAREN UnaryExpression 
    | LEFT_PAREN Expression RIGHT_PAREN UnaryExpressionNotPlusMinus
    | LEFT_PAREN Name Dims RIGHT_PAREN UnaryExpressionNotPlusMinus
    ;

UnaryExpression:
    MINUS UnaryExpression {
        if (integerVal > 1LL + INT_MAX ) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        integerVal = 0;
    }
    | UnaryExpressionNotPlusMinus
    ;

UnaryExpressionNotPlusMinus:
    PostfixExpression
    | NOT UnaryExpression
    | CastExpression;

PostfixExpression:
    Primary
    | Name
    ;

Dims:
    LEFT_BRACKET RIGHT_BRACKET
    | Dims LEFT_BRACKET RIGHT_BRACKET
    ;

DimExpr:
    LEFT_BRACKET Expression RIGHT_BRACKET;

DimExprs:
    DimExpr
    | DimExprs DimExpr { throw syntax_error(@1, std::string("multidimensional array not allowed")); }
    ;


FieldAccess:
    Primary DOT Variable
    | SUPER DOT Variable { throw syntax_error(@1, std::string("super not supported")); }
    ;

Primary:
    PrimaryNoNewArray
    | ArrayCreationExpression
    ;  

PrimaryNoNewArray:
    Literal
    | THIS
    | LEFT_PAREN Expression RIGHT_PAREN
    | ClassInstanceCreationExpression
    | FieldAccess
    | MethodInvocation
    | ArrayAccess
    ;

ArrayAccess:
    Name LEFT_BRACKET Expression RIGHT_BRACKET
    | PrimaryNoNewArray LEFT_BRACKET Expression RIGHT_BRACKET
    ;

ArrayCreationExpression:
    NEW BasicType DimExprs Dims
    | NEW BasicType DimExprs 
    | NEW ClassOrInterfaceType DimExprs Dims
    | NEW ClassOrInterfaceType DimExprs
    ;  

ParExpression:
    LEFT_PAREN Expression RIGHT_PAREN
    ;

StatementExpression:
    Assignment
    | MethodInvocation 
    | ClassInstanceCreationExpression
    ;

ReturnStatements:
    RETURN Expression SEMICOLON
    | RETURN SEMICOLON
    ;
    
MethodInvocation:
    Name LEFT_PAREN ArgumentList RIGHT_PAREN
    | Name LEFT_PAREN RIGHT_PAREN
    | Primary DOT Variable LEFT_PAREN ArgumentList RIGHT_PAREN
    | Primary DOT Variable LEFT_PAREN RIGHT_PAREN
    | SUPER DOT Variable LEFT_PAREN ArgumentList RIGHT_PAREN { throw syntax_error(@1, std::string("super method calls not allowed")); }
    | SUPER DOT Variable LEFT_PAREN RIGHT_PAREN { throw syntax_error(@1, std::string("super method calls not allowed")); }
    ;

ClassInstanceCreationExpression:
    NEW ClassType LEFT_PAREN ArgumentList RIGHT_PAREN
    | NEW ClassType LEFT_PAREN RIGHT_PAREN;
    ;

ArgumentList: 
    Expression 
    | ArgumentList COMMA Expression
    ;

/* Modifier
    : Annotation */
Modifier
    : PUBLIC {$$ = Modifiers::PUBLIC;}
    | PROTECTED {$$ = Modifiers::PROTECTED;}
    | STATIC {$$ = Modifiers::STATIC;}
    | ABSTRACT  {$$ = Modifiers::ABSTRACT;}
    | FINAL {$$ = Modifiers::FINAL;}
    | NATIVE {$$ = Modifiers::NATIVE;}
    ;

/* Annotations
    : Annotation
    | Annotations Annotation
    ;

Annotation
    :
    ; */

ClassBody
    : LEFT_BRACE ClassBodyOpt1 RIGHT_BRACE {
        if (!$2) throw syntax_error(@1, std::string("Every class must contain at least one explicit constructor.")); 
    }
    ;

ClassBodyOpt1
    : {$$ = false;}
    | ClassBodyDeclaration ClassBodyOpt1 {
        $$ = $1 | $2;
    }
    ;

ClassBodyDeclaration
    : SEMICOLON {$$ = false;}
    | ClassBodyDeclarationOpt1 MemberDecl {
        $$ = $2 == MemberType::CONSTRUCTOR;
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
        if ($1[0] == 0 && $1[1] == 0) throw syntax_error(@1, std::string("Every method/constructor/field is required to have an access modifier.")); 
    }
    | STATIC Block { throw syntax_error(@1, std::string("static initializers not allowed.")); }    
    | Block { throw syntax_error(@1, std::string("instance initializers not allowed.")); }
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

MemberDecl
    : MethodOrFieldDecl {$$ = $1;}
    | Variable ConstructorDeclaratorRest {
        $$ = MemberType::CONSTRUCTOR;
        if ($1 != lexer.getFilename()) throw syntax_error(@1, std::string("A constructor must be declared with the same filename."));
    }
    ;

MethodOrFieldDecl:
    ResultType Variable MethodOrFieldRest {
        $$ = $3;
        if ($1 == DataType::VOID && $3 == MemberType::FIELD) throw syntax_error(@1, std::string("The type void may only be used as the return type of a method."));
    } 
    ;

MethodOrFieldRest
    : FieldDeclaratorsRest SEMICOLON {$$ = MemberType::FIELD;}
    | MethodDeclaratorRest
    ;

MethodDeclaratorRest
    : FormalParameters Block {$$ = MemberType::METHODWITHBODY;}
    | FormalParameters SEMICOLON  {$$ = MemberType::METHODWITHOUTBODY;}
    ;

FieldDeclaratorsRest
    : VariableDeclaratorRest
    ;

VariableDeclaratorRest
    :
    | ASSIGN VariableInitializer
    ;

ConstructorDeclaratorRest
    : FormalParameters Block
    ;

FormalParameters: 
    LEFT_PAREN FormalParameterDecls RIGHT_PAREN
    ;


FormalParameterDecls:
    | Type FormalParameterDeclsRest
    ;

FormalParameterDeclsRest
    : VariableDeclaratorId
    | VariableDeclaratorId COMMA FormalParameterDecls
    ;

Block
    : LEFT_BRACE BlockStatements RIGHT_BRACE
    | LEFT_BRACE RIGHT_BRACE 
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

Variable
    : IDENTIFIER {
        if (std::find(keywords.begin(), keywords.end(), $1) != keywords.end()) throw syntax_error(@1, std::string("Invalid identifier: " + $1 + " is a Java reserved keyword"));
        $$ = $1;
    }
    ;

Literal
    : IntegerLiteral
    | BooleanLiteral
    | StringLiteral
    | CharLiteral
    | NullLiteral
    ;

IntegerLiteral
    : INTEGER {
        integerVal = stol($1);
    }
    ;

BooleanLiteral
    : TRUE
    | FALSE
    ;

StringLiteral
    : STRING
    ;

CharLiteral
    : CHARACTER
    | EscapeSequence
    ;     

EscapeSequence
    : ESCAPE EscapeSequence
    ;
    
NullLiteral
    : NUL
    ;

    
%%


void yy::parser::error(const location &location, const std::string &message)
{
    std::cerr << "Error at line " << location.end.line << " col " << location.end.column << ": " << message << std::endl;
}
