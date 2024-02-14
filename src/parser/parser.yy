%require "3.2"
%language "c++"
%define lr.type lalr

%code requires {
    #include <memory>
    #include <string>
    #include <tuple>
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

%type <std::shared_ptr<Identifier>> Variable VariableDeclaratorId
%type <std::shared_ptr<IdentifierType>> SimpleName QualifiedName Name ClassOrInterfaceType ClassType
%type <std::shared_ptr<Type>> Type BasicType ReferenceType ResultType ArrayType
%type <Modifiers> Modifier


%type <std::shared_ptr<Exp>> AdditiveExpression MultiplicativeExpression UnaryExpression UnaryExpressionNotPlusMinus
%type <std::shared_ptr<Exp>> ShiftExpression CastExpression PostfixExpression Primary PrimaryNoNewArray
%type <std::shared_ptr<Exp>> IntegerLiteral BooleanLiteral CharLiteral StringLiteral NullLiteral Literal
%type <std::shared_ptr<Exp>> FieldAccess DimExpr DimExprs ArrayAccess ArrayCreationExpression ParExpression 
%type <std::shared_ptr<Exp>> Expression AssignmentExpression LeftHandSide ConditionalAndExpression ConditionalOrExpression
%type <std::shared_ptr<Exp>> InclusiveOrExpression ExclusiveOrExpression AndExpression EqualityExpression RelationalExpression
%type <std::shared_ptr<Exp>> ConditionalExpression StatementExpression Assignment

%type <std::shared_ptr<Exp>> VariableInitializer

%type <std::string> ClassDeclarationOpt
%type <std::shared_ptr<ClassDecl>> ClassDeclaration NormalClassDeclaration


%type <std::vector<std::shared_ptr<IdentifierType>>> NormalClassDeclarationOpt2 NormalClassDeclarationOpt3 TypeList

%type <std::vector<std::vector<std::shared_ptr<MemberDecl>>>> ClassBody

%type <std::vector<std::shared_ptr<MemberDecl>>> ClassBodyOpt1
%type <std::vector<Modifiers>> ClassBodyDeclarationOpt1
%type <std::shared_ptr<MemberDecl>> MemberDecl MethodOrFieldDecl ClassBodyDeclaration

%type <std::tuple<MemberType, std::shared_ptr<Exp>, std::vector<std::shared_ptr<FormalParameter>>, std::shared_ptr<Block>>> MethodOrFieldRest FieldDeclaratorsRest VariableDeclaratorRest MethodDeclaratorRest
%type <std::tuple<std::vector<std::shared_ptr<FormalParameter>>, std::shared_ptr<Block>>> ConstructorDeclaratorRest
%type <std::shared_ptr<Block>> Block

%type <std::vector<std::shared_ptr<FormalParameter>>> FormalParameters
%type <std::vector<std::pair<std::shared_ptr<Type>, std::shared_ptr<Identifier>>>> FormalParameterDecls
%type <std::pair<std::shared_ptr<Identifier>, std::vector<std::pair<std::shared_ptr<Type>, std::shared_ptr<Identifier>>>>> FormalParameterDeclsRest


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
    : PUBLIC ClassDeclarationOpt NormalClassDeclaration {
        $$ = $3;
        $$->modifier = $2;
        ast.setAst($$);
    }
    ;

ClassDeclarationOpt
    : {$$ = "";}
    | ABSTRACT {$$ = "abstract";}
    | FINAL {$$ = "final";}
    ;

NormalClassDeclaration
    : CLASS Variable NormalClassDeclarationOpt1 NormalClassDeclarationOpt2 NormalClassDeclarationOpt3 ClassBody {
        if ($2->name != lexer.getFilename()) throw syntax_error(@1, std::string("A class must be declared with the same filename."));
        $$ = std::make_shared<ClassDecl>("placeholder", $2, $4, $5, $6);
    }
    ;

NormalClassDeclarationOpt1
    :
    | TypeParameters
    ;

NormalClassDeclarationOpt2
    : {$$ = {};}
    | EXTENDS TypeList {$$ = $2;}
    ;

NormalClassDeclarationOpt3
    : {$$ = {};}
    | IMPLEMENTS TypeList {$$ = $2;}
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
    : Type {
        $$ = $1;
    }
    | VOID {
        $$ = std::make_shared<Type>(); $$->type = DataType::VOID;
    }
    ;

// Do not use ReferenceType as arraytype is not supported here
TypeList
    : ClassOrInterfaceType {$$ = {$1};}
    | TypeList COMMA ClassOrInterfaceType {$$ = $1; $$.push_back($3);}
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
    : Variable {$$ = $1;}
    ;

VariableInitializer:
    Expression {$$ = $1;}
    ;

Expression:
    AssignmentExpression {$$ = $1;}
    ;

AssignmentExpression:
    ConditionalExpression   { $$ = $1; }
    | Assignment            { $$ = $1;}
    ;

Assignment:
    LeftHandSide ASSIGN AssignmentExpression {$$ = $3;}
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
    | EqualityExpression EQUAL RelationalExpression     { $$ = std::make_shared<Exp>();}
    | EqualityExpression NOT_EQUAL RelationalExpression { $$ = std::make_shared<Exp>();}
    ;

RelationalExpression:
    ShiftExpression { $$ = $1; }
    | RelationalExpression LESS ShiftExpression { $$ = std::make_shared<Exp>();}
    | RelationalExpression GREATER ShiftExpression  { $$ = std::make_shared<Exp>();}
    | RelationalExpression LESS_EQUAL ShiftExpression   { $$ = std::make_shared<Exp>();}
    | RelationalExpression GREATER_EQUAL ShiftExpression    { $$ = std::make_shared<Exp>();}
    | RelationalExpression INSTANCEOF ReferenceType { $$ = std::make_shared<Exp>();}
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
    | ClassInstanceCreationExpression {$$ = std::make_shared<Exp>();}
    | FieldAccess {$$ = $1;}
    | MethodInvocation {$$ = std::make_shared<Exp>();}
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
        bool hasConstructor = false;

        $$ = std::vector<std::vector<std::shared_ptr<MemberDecl>>>{};
        $$.resize(3);

        for (std::shared_ptr<MemberDecl> m : $2) {
            if (m->memberType == MemberType::FIELD) $$[0].push_back(m);
            else if (m->memberType == MemberType::METHODWITHBODY || m->memberType == MemberType::METHODWITHOUTBODY) {
                $$[1].push_back(m);
            } else {
                assert(m->memberType == MemberType::CONSTRUCTOR);
                $$[2].push_back(m);
                hasConstructor = true;
            }
        }
        if (hasConstructor == false) throw syntax_error(@1, std::string("Every class must contain at least one explicit constructor.")); 

    }
    ;

ClassBodyOpt1
    : {$$ = {};}
    | ClassBodyDeclaration ClassBodyOpt1 {
        $$ = {$1}; $$.insert($$.end(), $2.begin(), $2.end());
    }
    ;

ClassBodyDeclaration
    : SEMICOLON {
        $$ = nullptr;
    }
    | ClassBodyDeclarationOpt1 MemberDecl {
        $$ = $2; //with placeholder modifiers
        $$->modifiers = $1;

        std::vector<int> modifiers = std::vector<int>(6, 0);
        for (Modifiers m : $1) {
            if (!validateModifier(modifiers, m)) throw syntax_error(@1, std::string("Invalid modifier for member declaration"));
        }

        std::string error_message = "";

        for (int val: modifiers) {
            if (val > 1) throw syntax_error(@1, std::string("Duplicate modifier is not allowed.")); 
        }

        if (modifiers[0] && modifiers[1]) error_message = "Multiple access modifiers are not allowed"; 
        else if (modifiers[2] && (modifiers[3] || modifiers[4])) error_message = "An abstract method cannot be static or final."; 
        else if (modifiers[4] && modifiers[3]) error_message = "A static method cannot be final."; 
        else if (modifiers[5] && !modifiers[4]) error_message = "A native method must be static.";
        else if ($2->memberType == MemberType::FIELD && (modifiers[2] || modifiers[3] || modifiers[5])) error_message = "No field can be final, abstract or native.";
        else if ( ($2->memberType == MemberType::METHODWITHBODY && (modifiers[2] || modifiers[5])) || 
             ($2->memberType == MemberType::METHODWITHOUTBODY && (!modifiers[2] && !modifiers[5])) )
            error_message = "A method has a body if and only if it is neither abstract nor native";
        else if ($2->memberType == MemberType::CONSTRUCTOR && (modifiers[2] || modifiers[3] || modifiers[4] || modifiers[5]))
            error_message = "A constructor cannot be final, abstract, static or native";
        else if (!modifiers[0] && !modifiers[1]) error_message = "Every method/constructor/field is required to have an access modifier."; 

        if (error_message != "") throw syntax_error(@1, error_message);
    }
    | STATIC Block { throw syntax_error(@1, std::string("static initializers not allowed.")); }    
    | Block { throw syntax_error(@1, std::string("instance initializers not allowed.")); }
    ;

ClassBodyDeclarationOpt1
    : 
    Modifier {
        $$ = {$1};
    }
    | Modifier ClassBodyDeclarationOpt1 {
        $$ = {$1};
        $$.insert($$.end(), $2.begin(), $2.end());
    }
    ;

MemberDecl
    : MethodOrFieldDecl {$$ = $1;}
    | Variable ConstructorDeclaratorRest {
        std::vector<Modifiers> placeholder = {};
        $$ = std::make_shared<Constructor>(MemberType::CONSTRUCTOR, placeholder, $1, std::get<0>($2), std::get<1>($2));
        if ($1->name != lexer.getFilename()) throw syntax_error(@1, std::string("A constructor must be declared with the same filename."));
    }
    ;

MethodOrFieldDecl:
    ResultType Variable MethodOrFieldRest {
        std::vector<Modifiers> placeholder = {};
        //$3 is a 4-tuple of MemberType, Exp, FPs, and Block
        MemberType memberType = std::get<0>($3);
        if (memberType == MemberType::FIELD) {
            $$ = std::make_shared<Field>(memberType, placeholder, $1, $2, std::get<1>($3));
        } else {
            assert(memberType == MemberType::METHODWITHOUTBODY || memberType == MemberType::METHODWITHBODY);
            $$ = std::make_shared<Method>(memberType, placeholder, $1, $2, std::get<2>($3), std::get<3>($3));
            if (memberType == MemberType::METHODWITHOUTBODY) assert(std::get<3>($3) == nullptr);
        }
        if ($1->type == DataType::VOID && memberType == MemberType::FIELD) throw syntax_error(@1, std::string("The type void may only be used as the return type of a method."));
    } 
    ;

MethodOrFieldRest
    : FieldDeclaratorsRest SEMICOLON {
        $$ = $1;
    }
    | MethodDeclaratorRest {
        $$ = $1;
    }
    ;

MethodDeclaratorRest
    : FormalParameters Block {
        $$ = std::make_tuple(MemberType::METHODWITHBODY, nullptr, $1, $2);
    }
    | FormalParameters SEMICOLON  {
        $$ = std::make_tuple(MemberType::METHODWITHOUTBODY, nullptr, $1, nullptr);
    }
    ;

FieldDeclaratorsRest
    : VariableDeclaratorRest {
        $$ = $1;
    }
    ;

VariableDeclaratorRest
    : {
        $$ = std::make_tuple(MemberType::FIELD, nullptr, std::vector<std::shared_ptr<FormalParameter>>{}, nullptr);
    }
    | ASSIGN VariableInitializer {
        $$ = std::make_tuple(MemberType::FIELD, $2, std::vector<std::shared_ptr<FormalParameter>>{}, nullptr);
    }
    ;

ConstructorDeclaratorRest
    : FormalParameters Block {
        $$ = std::make_tuple($1, $2);
        }
    ;

FormalParameters: 
    LEFT_PAREN FormalParameterDecls RIGHT_PAREN {
        $$ = {};
        for (auto fp : $2) {
            $$.push_back(std::make_shared<FormalParameter>(fp.first, fp.second));
        }
    }
    ;


FormalParameterDecls: {$$ = {};}
    | Type FormalParameterDeclsRest {
        $$ = {std::make_pair($1, $2.first)};
        $$.insert($$.end(), $2.second.begin(), $2.second.end());
    }
    ;

FormalParameterDeclsRest
    : VariableDeclaratorId 
    {
        std::vector<std::pair<std::shared_ptr<Type>, std::shared_ptr<Identifier>>> tempvector = {};
        $$ = std::make_pair($1, tempvector);
    }
    | VariableDeclaratorId COMMA FormalParameterDecls
    {
        $$ = std::make_pair($1, $3);
    }
    ;

Block
    : LEFT_BRACE BlockStatements RIGHT_BRACE {$$ = std::make_shared<Block>(); //To be expanded
    }
    | LEFT_BRACE RIGHT_BRACE {$$ = std::make_shared<Block>(); //To be expanded
    }
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
