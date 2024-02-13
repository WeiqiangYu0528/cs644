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
%type <std::shared_ptr<Identifier>> Variable ImportStatement PackageImportIdentifier
%type <std::shared_ptr<IdentifierType>> SimpleName QualifiedName Name ClassOrInterfaceType ClassType
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
%type <std::shared_ptr<Exp>> ConditionalExpression StatementExpression Assignment MethodInvocation ClassInstanceCreationExpression
%type <std::vector<std::shared_ptr<Exp>>> ArgumentList 
%type <std::shared_ptr<Package>> PackageDeclaration
%type <std::shared_ptr<ImportStatement>> ImportStatements

%%

Program
    : PackageDeclaration ImportStatements ClassOrInterfaceDeclaration {
        ast.setAst($1);
    }
    ;

ClassOrInterfaceDeclaration
    :
    | ClassDeclaration
    | InterfaceDeclaration
    ;  

PackageDeclaration
    : {$$ = nullptr;}
    | PACKAGE Name SEMICOLON {$$ = std::make_shared<Package>($2->id);}
    ;

ImportStatements
    : {$$ = std::make_shared<ImportStatement>();}
    | ImportStatement ImportStatements {
        $$ = $2;
        $$->addImport($1);
    }
    ;

ImportStatement
    : IMPORT Name SEMICOLON {$$ = std::make_shared<Identifier>($2->id->name);}
    | IMPORT PackageImportIdentifier SEMICOLON {$$ = $2;}
    ;

PackageImportIdentifier    
    : Name DOT STAR {
        std::string temp = $1->id->name + ".*";
        $$ = std::make_shared<Identifier>(temp);
        }
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
    | EXTENDS InterfaceTypeList
    ;

InterfaceTypeList
    :
    Variable
    | InterfaceTypeList COMMA Variable
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
    : BYTE {$$ = std::make_shared<ByteType>();}
    | SHORT {$$ = std::make_shared<ShortType>();}
    | CHAR {$$ = std::make_shared<CharType>();}
    | INT {$$ = std::make_shared<IntType>();}
    | BOOLEAN {$$ = std::make_shared<BooleanType>();}
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
    Variable {$$ = std::make_shared<IdentifierType>($1);}
    ;

QualifiedName:
    Name DOT Variable {
        //$$ = std::make_shared<CompoundType>($1, $3);
        std::string newStr = $1->id->name + '.' + $3->name;
        std::shared_ptr<Identifier> temp = std::make_shared<Identifier>(newStr);
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
    | VOID {$$ = std::make_shared<VoidType>();}
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
    AssignmentExpression {$$ = $1;}
    ;

AssignmentExpression:
    ConditionalExpression   { $$ = $1; }
    | Assignment            { $$ = $1;}
    ;

Assignment:
    LeftHandSide ASSIGN AssignmentExpression { $$ = std::make_shared<Assignment>($1, $3);}
    ;

LeftHandSide:
    Name {$$ = std::make_shared<IdentifierExp>($1->id);}
    | FieldAccess {$$ = $1;}
    | ArrayAccess {$$ = $1;}
    ;

ConditionalExpression:
    ConditionalOrExpression  { $$ = $1; }
    | ConditionalOrExpression QUESTION_MARK Expression COLON ConditionalExpression { throw syntax_error(@1, std::string("conditional operator not supported")); }
    ;

ConditionalOrExpression:
    ConditionalAndExpression { $$ = $1; }
    | ConditionalOrExpression OR_OR ConditionalAndExpression {$$ = std::make_shared<ConditionalOrExp>($1, $3);}
    ;

ConditionalAndExpression:
    InclusiveOrExpression   { $$ = $1; }
    | ConditionalAndExpression AND_AND InclusiveOrExpression {$$ = std::make_shared<ConditionalAndExp>($1, $3);}
    ;

InclusiveOrExpression:
    ExclusiveOrExpression   { $$ = $1; }
    | InclusiveOrExpression OR ExclusiveOrExpression {$$ = std::make_shared<OrExp>($1, $3);}
    ;

ExclusiveOrExpression:
    AndExpression           { $$ = $1; }
    | ExclusiveOrExpression XOR AndExpression {$$ = std::make_shared<XorExp>($1, $3);}
    ;

AndExpression:
    EqualityExpression      { $$ = $1; }
    | AndExpression AND EqualityExpression {$$ = std::make_shared<AndExp>($1, $3);}
    ;

EqualityExpression:
    RelationalExpression   { $$ = $1; }
    | EqualityExpression EQUAL RelationalExpression     { $$ = std::make_shared<EqualExp>($1, $3);}
    | EqualityExpression NOT_EQUAL RelationalExpression { $$ = std::make_shared<NotEqualExp>($1, $3);}
    ;

RelationalExpression:
    ShiftExpression { $$ = $1; }
    | RelationalExpression LESS ShiftExpression { $$ = std::make_shared<LessExp>($1, $3); }
    | RelationalExpression GREATER ShiftExpression  { $$ = std::make_shared<GreaterExp>($1, $3);}
    | RelationalExpression LESS_EQUAL ShiftExpression   { $$ = std::make_shared<LessEqualExp>($1, $3);}
    | RelationalExpression GREATER_EQUAL ShiftExpression    { $$ = std::make_shared<GreaterEqualExp>($1, $3);}
    | RelationalExpression INSTANCEOF ReferenceType { $$ = std::make_shared<InstanceOfExp>($1, $3);}
    ;

ShiftExpression:
    AdditiveExpression  { $$ = $1; }
    | ShiftExpression LEFT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("left shift not supported")); }
    | ShiftExpression RIGHT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("right shift not supported")); }
    | ShiftExpression UNSIGNED_RIGHT_SHIFT AdditiveExpression { throw syntax_error(@1, std::string("unsigned right shift not supported")); }
    ;

AdditiveExpression:
    MultiplicativeExpression    {$$ = $1;}
    | AdditiveExpression PLUS MultiplicativeExpression  {$$ = std::make_shared<PlusExp>($1, $3);}
    | AdditiveExpression MINUS MultiplicativeExpression {$$ = std::make_shared<MinusExp>($1, $3);}
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
    }
    | MultiplicativeExpression DIVIDE UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($1)) {
            if (integral->value > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<DivideExp>($1, $3);
    }
    | MultiplicativeExpression MODULO UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($1)) {
            if (integral->value > INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<ModuloExp>($1, $3);
    }
    ;

CastExpression:
    LEFT_PAREN BasicType Dims RIGHT_PAREN UnaryExpression {
        std::shared_ptr<ArrayType> arr = std::make_shared<ArrayType>($2);
        $$ = std::make_shared<CastExp>($5, arr);  
        }
    | LEFT_PAREN BasicType RIGHT_PAREN UnaryExpression {
        $$ = std::make_shared<CastExp>($4, $2);
        }
    | LEFT_PAREN Expression RIGHT_PAREN UnaryExpressionNotPlusMinus  {
        auto temp = std::dynamic_pointer_cast<IdentifierExp>($2);
        if (!temp) {
            throw syntax_error(@1, std::string("invalid casting."));
        }
        std::shared_ptr<Type> type = std::make_shared<IdentifierType>(temp->id);
        $$ = std::make_shared<CastExp>($4, type);
        }
    | LEFT_PAREN Name Dims RIGHT_PAREN UnaryExpressionNotPlusMinus {
        std::shared_ptr<ArrayType> arr = std::make_shared<ArrayType>($2);
        $$ = std::make_shared<CastExp>($5, arr);
        }
    ;

UnaryExpression:
    MINUS UnaryExpression {
        if (auto integral = std::dynamic_pointer_cast<IntegerLiteralExp>($2)) {
            if (integral->value > 1L + INT_MAX) throw syntax_error(@1, std::string("integer literal should be within the legal range for 32-bit signed integers."));
        }
        $$ = std::make_shared<NegExp>($2);
    }
    | UnaryExpressionNotPlusMinus {
        $$ = $1;
        }
    ;

UnaryExpressionNotPlusMinus:
    PostfixExpression   { $$ = $1; }
    | NOT UnaryExpression   { $$ = std::make_shared<NotExp>($2); }
    | CastExpression   { $$ = $1; }

PostfixExpression:
    Primary { $$ = $1; }
    | Name {
        $$ = std::make_shared<IdentifierExp>($1->id);
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
    Primary DOT Variable {$$ = std::make_shared<FieldAccessExp>($1, $3);}
    | SUPER DOT Variable { throw syntax_error(@1, std::string("super not supported")); }
    ;

Primary:
    PrimaryNoNewArray   { $$ = $1;}
    | ArrayCreationExpression   { $$ = $1;}
    ;

PrimaryNoNewArray:
    Literal {$$ = $1;}    
    | THIS {$$ = std::make_shared<ThisExp>();}
    | LEFT_PAREN Expression RIGHT_PAREN {$$ = std::make_shared<ParExp>($2);}
    | ClassInstanceCreationExpression {$$ = $1; }
    | FieldAccess {$$ = $1;}
    | MethodInvocation {$$ = $1;}
    | ArrayAccess {$$ = $1;}
    ;

ArrayAccess:
    Name LEFT_BRACKET Expression RIGHT_BRACKET {
        std::shared_ptr<IdentifierExp> identifier = std::make_shared<IdentifierExp>($1->id);
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
    Assignment {$$ = $1;}
    | MethodInvocation {$$ = $1;}
    | ClassInstanceCreationExpression  {$$ = $1;}
    ;

ReturnStatements:
    RETURN Expression SEMICOLON
    | RETURN SEMICOLON
    ;

MethodInvocation:
    Name LEFT_PAREN ArgumentList RIGHT_PAREN {
        $$ = std::make_shared<MethodInvocation>(nullptr, nullptr, $1, $3);
    }
    | Name LEFT_PAREN RIGHT_PAREN {
        $$ = std::make_shared<MethodInvocation>(nullptr, nullptr, $1, std::vector<std::shared_ptr<Exp>>());
    }
    | Primary DOT Variable LEFT_PAREN ArgumentList RIGHT_PAREN {
        $$ = std::make_shared<MethodInvocation>($1, $3, nullptr, $5); 
    }
    | Primary DOT Variable LEFT_PAREN RIGHT_PAREN {
        $$ = std::make_shared<MethodInvocation>($1, $3, nullptr, std::vector<std::shared_ptr<Exp>>());
    }
    | SUPER DOT Variable LEFT_PAREN ArgumentList RIGHT_PAREN { throw syntax_error(@1, std::string("super method calls not allowed")); }
    | SUPER DOT Variable LEFT_PAREN RIGHT_PAREN { throw syntax_error(@1, std::string("super method calls not allowed")); }
    ;

ClassInstanceCreationExpression:
    NEW ClassType LEFT_PAREN ArgumentList RIGHT_PAREN {$$ = std::make_shared<ClassInstanceCreationExp>($2, $4); }
    | NEW ClassType LEFT_PAREN RIGHT_PAREN {$$ = std::make_shared<ClassInstanceCreationExp>($2, std::vector<std::shared_ptr<Exp>>()); }
    ;

ArgumentList: 
    Expression { 
        $$ = std::vector<std::shared_ptr<Exp>>();
        $$.push_back($1); 
    }
    | ArgumentList COMMA Expression { $1.push_back($3); $$ = $1;}
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
        $$ = std::make_shared<Identifier>($1);
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
    : STRING {$$ = std::make_shared<StringLiteralExp>($1);}
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
