%precedence IDENTIFIER
%precedence LEFT_BRACKET

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
%token TRUE FALSE

%nonassoc THEN
%nonassoc ELSE

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
    | PACKAGE QualifiedIdentifier SEMICOLON
    ;

ImportStatements
    :
    | ImportStatement ImportStatements
    ;

ImportStatement
    : IMPORT QualifiedIdentifier SEMICOLON
    | IMPORT PackageImportIdentifier SEMICOLON
    ;

PackageImportIdentifier    
    : QualifiedIdentifier DOT STAR
    ;

QualifiedIdentifier 
    : IDENTIFIER
    | QualifiedIdentifier DOT IDENTIFIER
    ;

InterfaceDeclaration 
    : PUBLIC InterfaceOpt1 INTERFACE IDENTIFIER InterfaceOpt2 InterfaceBody {
        if ($4 != lexer.getFilename()) throw syntax_error(@1, std::string("An interface must be declared with the same filename."));
     }
    ;

InterfaceOpt1
    :
    | ABSTRACT
    ;

InterfaceOpt2
    :
    | EXTENDS IDENTIFIER
    ;

InterfaceBody
    : LEFT_BRACE MethodDeclarations RIGHT_BRACE
    ;

MethodDeclarations 
    : 
    | MethodDeclaration MethodDeclarations
    ;

MethodDeclaration
    : PUBLIC MethodDeclarationOpt Type IDENTIFIER FormalParameters SEMICOLON
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
    : CLASS IDENTIFIER NormalClassDeclarationOpt1 NormalClassDeclarationOpt2 NormalClassDeclarationOpt3 ClassBody {
        if ($2 != lexer.getFilename()) throw syntax_error(@1, std::string("A class must be declared with the same filename."));
    }
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
    : IDENTIFIER {$$ = DataType::OBJECT;}
    | ArrayType {$$ = DataType::ARRAY;}
    ;

ArrayType
    : Type LEFT_BRACKET RIGHT_BRACKET
    ;

ResultType
    : Type {$$ = $1;}
    | VOID {$$ = DataType::VOID;}

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
    | ReturnStatements
    | StatementExpression
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
    IDENTIFIER ASSIGN VariableInitializer
    ;

VariableInitializer:
    Expression
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
    IDENTIFIER
    | FieldAccess
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
    UnaryExpression
    | MultiplicativeExpression STAR UnaryExpression
    | MultiplicativeExpression DIVIDE UnaryExpression
    | MultiplicativeExpression MODULO UnaryExpression
    ;

UnaryExpression:
    COMMA;

FieldAccess:
    Primary DOT IDENTIFIER
    | SUPER DOT IDENTIFIER { throw syntax_error(@1, std::string("super not supported")); }
    ;

Primary:
    PrimaryNoNewArray
    ;

PrimaryNoNewArray:
    Literal
    | THIS
    | LEFT_PAREN Expression RIGHT_PAREN
    | ClassInstanceCreationExpression
    | FieldAccess
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

ReturnStatements
    : RETURN ReturnStatement SEMICOLON
    ;
    
ReturnStatement
    : Literal
    | QualifiedIdentifier
    | MethodInvocation
    | ClassInstanceCreationExpression
    | THIS
    ;

MethodInvocation:
    IDENTIFIER LEFT_PAREN ArgumentList RIGHT_PAREN
    | THIS LEFT_PAREN RIGHT_PAREN { throw syntax_error(@1, std::string("this call not allowed")); }
    | SUPER LEFT_PAREN RIGHT_PAREN { throw syntax_error(@1, std::string("super call not allowed")); }
    | SUPER DOT IDENTIFIER LEFT_PAREN ArgumentList RIGHT_PAREN { throw syntax_error(@1, std::string("super method calls not allowed")); }
    ;

ClassInstanceCreationExpression:
    NEW IDENTIFIER LEFT_PAREN ArgumentList RIGHT_PAREN
    ;

ArgumentList: 
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
    | IDENTIFIER ConstructorDeclaratorRest {
        $$ = MemberType::CONSTRUCTOR;
        if ($1 != lexer.getFilename()) throw syntax_error(@1, std::string("A constructor must be declared with the same filename."));
    }
    ;

MethodOrFieldDecl:
    ResultType IDENTIFIER MethodOrFieldRest {
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

VariableDeclaratorId
    : IDENTIFIER 
    | IDENTIFIER MultiDimensionArray
    ;

MultiDimensionArray
    : LEFT_BRACKET RIGHT_BRACKET
    | MultiDimensionArray LEFT_BRACKET RIGHT_BRACKET
    ;

Block
    : LEFT_BRACE BlockStatements RIGHT_BRACE
    ;

BlockStatements:
    | BlockStatement BlockStatements
    ;

BlockStatement:
    Statement
    | LocalVariableDeclaration
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
        if (stol($1) > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
    }
    ;

BooleanLiteral
    : TRUE
    | FALSE
    ;

StringLiteral
    : QUOTE StringCharacters QUOTE
    ;

StringCharacters
    : StringCharacter
    | StringCharacter StringCharacters
    ;

StringCharacter
    : IDENTIFIER
    | ESCAPE
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
