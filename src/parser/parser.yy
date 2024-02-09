%require "3.2"
%language "c++"
%define lr.type lalr

%code requires {
    #include <memory>
    #include <string>
    #include <vector>
    #include <limits.h>
    #include "ast.hpp"
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
%parse-param {MyLexer &lexer} {Ast& ast}


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
%token <std::string> INTEGER STRING NUL
%token <char> CHARACTER
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
%token INVALID

%nonassoc THEN
%nonassoc ELSE

%type <bool> ClassBodyDeclaration ClassBodyOpt1
%type <std::shared_ptr<IdentifierType>> Variable SimpleName QualifiedName Name ClassOrInterfaceType ClassType
%type <std::shared_ptr<Type>> Type BasicType ReferenceType ResultType ArrayType
%type <Modifiers> Modifier
%type <std::vector<int>> ClassBodyDeclarationOpt1
%type <MemberType> MemberDecl MethodOrFieldDecl MethodOrFieldRest MethodDeclaratorRest

%type <std::shared_ptr<Exp>> AdditiveExpression MultiplicativeExpression UnaryExpression UnaryExpressionNotPlusMinus
%type <std::shared_ptr<Exp>> ShiftExpression CastExpression PostfixExpression Primary PrimaryNoNewArray
%type <std::shared_ptr<Exp>> IntegerLiteral BooleanLiteral CharLiteral StringLiteral NullLiteral Literal
%type <std::shared_ptr<Exp>> FieldAccess DimExpr DimExprs ArrayAccess ArrayCreationExpression ParExpression 
%type <std::shared_ptr<Exp>> Expression AssignmentExpression LeftHandSide ConditionalAndExpression ConditionalOrExpression
%type <std::shared_ptr<Exp>> InclusiveOrExpression ExclusiveOrExpression AndExpression EqualityExpression RelationalExpression
%type <std::shared_ptr<Exp>> ConditionalExpression StatementExpression Assignment

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
        if ($4->name != lexer.getFilename()) throw syntax_error(@1, std::string("An interface must be declared with the same filename."));
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
        if ($2->name != lexer.getFilename()) throw syntax_error(@1, std::string("A class must be declared with the same filename."));
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
    : BYTE {$$ = std::make_shared<Type>(); $$->type = DataType::BYTE;}
    | SHORT {$$ = std::make_shared<Type>(); $$->type = DataType::SHORT;}
    | CHAR {$$ = std::make_shared<Type>(); $$->type = DataType::CHAR;}
    | INT {$$ = std::make_shared<Type>(); $$->type = DataType::INT;}
    | BOOLEAN {$$ = std::make_shared<Type>(); $$->type = DataType::BOOLEAN;}
    ;

ReferenceType
    : ClassOrInterfaceType {$$ = $1;}
    | ArrayType {$$ = $1;}
    ;

Name:
    SimpleName {$$ = $1;}
    | QualifiedName {$$ = $1;}
    ;

SimpleName:
    Variable {$$ = $1;}
    ;

QualifiedName:
    Name DOT Variable {
        //$$ = std::make_shared<CompoundType>($1, $3);
        std::string temp = $1->name + '.' + $3->name;
        $$ = std::make_shared<IdentifierType>(temp);
    }
    ;

ClassOrInterfaceType:
    Name {$$ = $1;}
    ;

ClassType:
    ClassOrInterfaceType {$$ = $1;}
    ;

ArrayType
    : BasicType LEFT_BRACKET RIGHT_BRACKET {$$ = std::make_shared<ArrayType>($1);}
    | Name LEFT_BRACKET RIGHT_BRACKET {$$ = std::make_shared<ArrayType>($1);}
    ;

ResultType
    : Type {$$ = $1;}
    | VOID {$$ = std::make_shared<Type>(); $$->type = DataType::VOID;}
    ;

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
    ;

Expression:
    AssignmentExpression {$$ = $1; ast.setAst($$);}
    ;

AssignmentExpression:
    ConditionalExpression   { $$ = $1; }
    | Assignment            { $$ = $1; $$->notAName = true; }
    ;

Assignment:
    LeftHandSide ASSIGN AssignmentExpression {$$ = $3;}
    ;

LeftHandSide:
    Name {$$ = std::make_shared<IdentifierExp>($1->name);}
    | FieldAccess {$$ = $1;}
    | ArrayAccess {$$ = $1;}
    ;

ConditionalExpression:
    ConditionalOrExpression  { $$ = $1; }
    | ConditionalOrExpression QUESTION_MARK Expression COLON ConditionalExpression { throw syntax_error(@1, std::string("conditional operator not supported")); }
    ;

ConditionalOrExpression:
    ConditionalAndExpression { $$ = $1; }
    | ConditionalOrExpression OR_OR ConditionalAndExpression {$$ = std::make_shared<Exp>();}
    ;

ConditionalAndExpression:
    InclusiveOrExpression   { $$ = $1; }
    | ConditionalAndExpression AND_AND InclusiveOrExpression {$$ = std::make_shared<Exp>();}
    ;

InclusiveOrExpression:
    ExclusiveOrExpression   { $$ = $1; }
    | InclusiveOrExpression OR ExclusiveOrExpression {$$ = std::make_shared<Exp>();}
    ;

ExclusiveOrExpression:
    AndExpression           { $$ = $1; }
    | ExclusiveOrExpression XOR AndExpression {$$ = std::make_shared<Exp>();}
    ;

AndExpression:
    EqualityExpression      { $$ = $1; }
    | AndExpression AND EqualityExpression {$$ = std::make_shared<Exp>();}
    ;

EqualityExpression:
    RelationalExpression   { $$ = $1; }
    | EqualityExpression EQUAL RelationalExpression     { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    | EqualityExpression NOT_EQUAL RelationalExpression { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    ;

RelationalExpression:
    ShiftExpression { $$ = $1; }
    | RelationalExpression LESS ShiftExpression { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    | RelationalExpression GREATER ShiftExpression  { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    | RelationalExpression LESS_EQUAL ShiftExpression   { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    | RelationalExpression GREATER_EQUAL ShiftExpression    { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    | RelationalExpression INSTANCEOF ReferenceType { $$ = std::make_shared<Exp>(); $$->notAName = true; }
    ;

ShiftExpression:
    AdditiveExpression  { $$ = $1; }
    | ShiftExpression LEFT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("left shift not supported")); }
    | ShiftExpression RIGHT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("right shift not supported")); }
    | ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("unsigned right shift not supported")); }
    ;

AdditiveExpression:
    MultiplicativeExpression    {$$ = $1;}
    | AdditiveExpression PLUS MultiplicativeExpression  {$$ = std::make_shared<PlusExp>($1, $3); $$->notAName = true; }
    | AdditiveExpression MINUS MultiplicativeExpression {$$ = std::make_shared<MinusExp>($1, $3); $$->notAName = true; }
    ;   

MultiplicativeExpression:
    UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($1)) {
            if (integral->value > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = $1;
    }
    | MultiplicativeExpression STAR UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($1)) {
            if (integral->value > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<TimesExp>($1, $3);
        $$->notAName = true;
    }
    | MultiplicativeExpression DIVIDE UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($1)) {
            if (integral->value > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<DivideExp>($1, $3);
        $$->notAName = true;
    }
    | MultiplicativeExpression MODULO UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($1)) {
            if (integral->value > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<ModuloExp>($1, $3);
        $$->notAName = true;
    }
    ;

CastExpression:
    LEFT_PAREN BasicType Dims RIGHT_PAREN UnaryExpression {
        std::shared_ptr<ArrayType> arr = std::make_shared<ArrayType>($2);
        $$ = std::make_shared<CastExp>($5, arr);  
        $$->notAName = true;
        }
    | LEFT_PAREN BasicType RIGHT_PAREN UnaryExpression {
        $$ = std::make_shared<CastExp>($4, $2);
        $$->notAName = true; 
        }
    | LEFT_PAREN Expression RIGHT_PAREN UnaryExpressionNotPlusMinus  {
        if($2->notAName)  throw syntax_error(@1, std::string("invalid casting."));
        // std::shared_ptr<IdentifierExp> temp = std::static_pointer_cast<IdentifierExp>($2);
        // std::shared_ptr<Type> type = std::make_shared<IdentifierType>(temp->name);
        std::string temp{"placeholder"};
        std::shared_ptr<Type> type = std::make_shared<IdentifierType>(temp);
        $$ = std::make_shared<CastExp>($4, type);
        $$->notAName = true;
        }
    | LEFT_PAREN Name Dims RIGHT_PAREN UnaryExpressionNotPlusMinus {
        std::shared_ptr<ArrayType> arr = std::make_shared<ArrayType>($2);
        $$ = std::make_shared<CastExp>($5, arr);
        $$->notAName = true; 
        }
    ;

UnaryExpression:
    MINUS UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($2)) {
            if (integral->value > 1L + INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<NegExp>($2);
        $$->notAName = true;
    }
    | UnaryExpressionNotPlusMinus {
        $$ = $1;
        }
    ;

UnaryExpressionNotPlusMinus:
    PostfixExpression   { $$ = $1; }
    | NOT UnaryExpression   { $$ = std::make_shared<NotExp>($2); $$->notAName = true; }
    | CastExpression   { $$ = $1; $$->notAName = true; }

PostfixExpression:
    Primary { $$ = $1; }
    | Name {
        $$ = std::make_shared<IdentifierExp>($1->name);
        $$->notAName = false; 
    }
    ;

Dims:
    LEFT_BRACKET RIGHT_BRACKET
    | Dims LEFT_BRACKET RIGHT_BRACKET
    ;

DimExpr:
    LEFT_BRACKET Expression RIGHT_BRACKET {$$ = $2;}
    ;

DimExprs:
    DimExpr {$$ = $1;}
    | DimExprs DimExpr { throw syntax_error(@1, std::string("multidimensional array not allowed")); }
    ;

FieldAccess:
    Primary DOT Variable {$$ = std::make_shared<FieldAccessExp>($1, $3->name); $$->notAName = $1->notAName; }
    | SUPER DOT Variable { throw syntax_error(@1, std::string("super not supported")); }
    ;

Primary:
    PrimaryNoNewArray   { $$ = $1; $$->notAName = true; }
    | ArrayCreationExpression   { $$ = $1; $$->notAName = true; }
    ;  

PrimaryNoNewArray:
    Literal {$$ = $1;}    
    | THIS {$$ = std::make_shared<ThisExp>();}
    | LEFT_PAREN Expression RIGHT_PAREN {$$ = $2;}
    | ClassInstanceCreationExpression {$$ = std::make_shared<Exp>();}
    | FieldAccess {$$ = $1;}
    | MethodInvocation {$$ = std::make_shared<Exp>();}
    | ArrayAccess {$$ = $1;}
    ;

ArrayAccess:
    Name LEFT_BRACKET Expression RIGHT_BRACKET {
        std::shared_ptr<IdentifierExp> identifier = std::make_shared<IdentifierExp>($1->name);
        $$ = std::make_shared<ArrayAccessExp>(identifier, $3);
        }
    | PrimaryNoNewArray LEFT_BRACKET Expression RIGHT_BRACKET {
        $$ = std::make_shared<ArrayAccessExp>($1, $3);
    }
    ;

ArrayCreationExpression:
    NEW BasicType DimExprs Dims {$$ = std::make_shared<NewArrayExp>($3, $2);}
    | NEW BasicType DimExprs {$$ = std::make_shared<NewArrayExp>($3, $2);}
    | NEW ClassOrInterfaceType DimExprs Dims {$$ = std::make_shared<NewArrayExp>($3, $2);}
    | NEW ClassOrInterfaceType DimExprs {$$ = std::make_shared<NewArrayExp>($3, $2);}
    ;  

ParExpression:
    LEFT_PAREN Expression RIGHT_PAREN {$$ = $2;}
    ;

StatementExpression:
    Assignment {$$ = std::make_shared<Exp>();}
    | MethodInvocation {$$ = std::make_shared<Exp>();}
    | ClassInstanceCreationExpression  {$$ = std::make_shared<Exp>();}
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
        if ($1->name != lexer.getFilename()) throw syntax_error(@1, std::string("A constructor must be declared with the same filename."));
    }
    ;

MethodOrFieldDecl:
    ResultType Variable MethodOrFieldRest {
        $$ = $3;
        if ($1->type == DataType::VOID && $3 == MemberType::FIELD) throw syntax_error(@1, std::string("The type void may only be used as the return type of a method."));
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
        $$ = std::make_shared<IdentifierType>($1);
    }
    ;

Literal
    : IntegerLiteral {$$ = $1;}
    | BooleanLiteral {$$ = $1;}
    | StringLiteral {$$ = $1;}
    | CharLiteral {$$ = $1;}
    | NullLiteral {$$ = $1;}
    ;

IntegerLiteral
    : INTEGER {
        $$ = std::make_shared<IntegerLiteralExp>(stol($1));
    }
    ;

BooleanLiteral
    : TRUE {$$ = std::make_shared<BoolLiteralExp>(true);}
    | FALSE {$$ = std::make_shared<BoolLiteralExp>(false);}
    ;

StringLiteral
    : STRING {
        if ($1.length() >= 2 && (std::find(invalidStr.begin(), invalidStr.end(), $1.substr(1, 2)) != invalidStr.end())) throw syntax_error(@1, std::string("Invalid string input"));
        $$ = std::make_shared<StringLiteralExp>($1);
    }
    ;

CharLiteral
    : CHARACTER {$$ = std::make_shared<CharLiteralExp>($1);}
    ;
    
NullLiteral
    : NUL {$$ = std::make_shared<NulLiteralExp>();}
    ;

%%


void yy::parser::error(const location &location, const std::string &message)
{
    std::cerr << "Error at line " << location.end.line << " col " << location.end.column << ": " << message << std::endl;
}
