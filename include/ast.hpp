#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "weeder.hpp"

class Visitor;
class SymbolTable;
class Scope;

struct ExpressionObject {
    Expression exp;
    std::string name;
    std::shared_ptr<SymbolTable> st;
};

class AstNode {
    public:
        virtual ~AstNode() = default;
        virtual void accept(Visitor* v) = 0;
};

class Exp : public AstNode {
    public:
        ~Exp() = default;
        virtual void accept(Visitor* v) = 0;
};

class Type : public AstNode {
    public:
        DataType type;
        Type(DataType t);
        ~Type() = default;
        static bool isSameType(const std::shared_ptr<Type>& a, const std::shared_ptr<Type>& b) {
            if (!a || !b) return false;
            return a->type == b->type;
        }
        virtual std::string typeToString() = 0;
        virtual void accept(Visitor* v) = 0;
};

class Statement : public AstNode
{
    public:
        virtual ~Statement() = default;
        virtual void accept(Visitor *v) = 0;
        virtual std::string getStmtName() = 0;
};

class MemberDecl : public AstNode {
    public:
        MemberType memberType;
        std::vector<Modifiers> modifiers;
        MemberDecl(MemberType mt, std::vector<Modifiers> m);
        virtual ~MemberDecl() = default;
        virtual void accept(Visitor* v) = 0;
        virtual void setModifiers(std::vector<Modifiers>& m) = 0;
};

class Identifier : public AstNode, public std::enable_shared_from_this<Identifier> {
    public:
        std::string name;
        Identifier(std::string& s);
        void accept(Visitor* v) override;
};

class IdentifierType : public Type, public std::enable_shared_from_this<IdentifierType> {
    public:
        std::shared_ptr<Identifier> id;
        bool simple;
        IdentifierType(std::shared_ptr<Identifier> i, bool s = false);
        std::string typeToString() override;
        void accept(Visitor* v) override;
};

class ArrayType : public Type, public std::enable_shared_from_this<ArrayType> {
    public:
        std::shared_ptr<Type> dataType;
        ArrayType(std::shared_ptr<Type> t);
        std::string typeToString() override;
        void accept(Visitor* v) override;
};

class Package : public AstNode, public std::enable_shared_from_this<Package>  {
    public:
        std::shared_ptr<Identifier> id;
        Package(std::shared_ptr<Identifier> i = nullptr);
        void accept(Visitor* v) override;
};

class ImportStatement : public AstNode, public std::enable_shared_from_this<ImportStatement> {
    public:
        std::shared_ptr<Identifier> package;
        std::shared_ptr<Identifier> id;
        ImportStatement(std::shared_ptr<Identifier> p, std::shared_ptr<Identifier> i);
        void accept(Visitor* v) override;
};

class ImportStatements : public AstNode, public std::enable_shared_from_this<ImportStatements> {
    public:
        std::vector<std::shared_ptr<ImportStatement>> stmts;
        void addImport(std::shared_ptr<ImportStatement> i);
        void accept(Visitor* v) override;
};

class ClassOrInterfaceDecl : public AstNode {
    public:
        std::shared_ptr<Identifier> id;
        std::vector<std::shared_ptr<IdentifierType>> extended;
        ClassOrInterfaceDecl(std::shared_ptr<Identifier> i, std::vector<std::shared_ptr<IdentifierType>> e);
        virtual ~ClassOrInterfaceDecl() = default;
        virtual void accept(Visitor* v) = 0;
};

// class CompoundType : public Type, public std::enable_shared_from_this<CompoundType> {
//     public:
//         std::shared_ptr<Type> type;
//         std::shared_ptr<IdentifierType> subType;
//         CompoundType(std::shared_ptr<Type> t1, std::shared_ptr<IdentifierType> t2);
//         void accept(Visitor* v) override;
// };

class PlusExp : public Exp, public std::enable_shared_from_this<PlusExp> {
    public:
        std::shared_ptr<Exp> exp1, exp2;
        PlusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
        void accept(Visitor* v) override;
};

class MinusExp : public Exp, public std::enable_shared_from_this<MinusExp> {
    public:
        std::shared_ptr<Exp> exp1, exp2;
        MinusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
        void accept(Visitor* v) override;
};

class TimesExp : public Exp, public std::enable_shared_from_this<TimesExp> {
    public:
        std::shared_ptr<Exp> exp1, exp2;
        TimesExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
        void accept(Visitor* v) override;
};

class DivideExp : public Exp, public std::enable_shared_from_this<DivideExp> {
    public:
        std::shared_ptr<Exp> exp1, exp2;
        DivideExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
        void accept(Visitor* v) override;
};

class ModuloExp : public Exp, public std::enable_shared_from_this<ModuloExp> {
    public:
        std::shared_ptr<Exp> exp1, exp2;
        ModuloExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
        void accept(Visitor* v) override;
};

class NotExp : public Exp, public std::enable_shared_from_this<NotExp> {
    public:
        std::shared_ptr<Exp> exp;
        NotExp(std::shared_ptr<Exp> e);
        void accept(Visitor* v) override;
};

class IdentifierExp : public Exp, public std::enable_shared_from_this<IdentifierExp> {
    public:
        std::shared_ptr<Identifier> id;
        std::vector<ExpressionObject> exprs;
        bool simple;
        IdentifierExp(std::shared_ptr<Identifier> i, bool s = false);
        void accept(Visitor* v) override;
};

class IntegerLiteralExp : public Exp, public std::enable_shared_from_this<IntegerLiteralExp> {
    public:
        long value;
        IntegerLiteralExp(long val);
        void accept(Visitor* v) override;
};

class BoolLiteralExp : public Exp, public std::enable_shared_from_this<BoolLiteralExp> {
    public:
        bool value;
        BoolLiteralExp(bool val);
        void accept(Visitor* v) override;
};

class CharLiteralExp : public Exp, public std::enable_shared_from_this<CharLiteralExp> {
    public:
        char value;
        CharLiteralExp(char val);
        void accept(Visitor* v) override;
};

class StringLiteralExp : public Exp, public std::enable_shared_from_this<StringLiteralExp> {
    public:
        std::string value;
        StringLiteralExp(std::string& val);
        void accept(Visitor* v) override;
};

class NulLiteralExp : public Exp, public std::enable_shared_from_this<NulLiteralExp> {
    public:
        NulLiteralExp();
        void accept(Visitor* v) override;
};

class ArrayAccessExp: public Exp, public std::enable_shared_from_this<ArrayAccessExp> {
    public:
        std::shared_ptr<Exp> array, index;
        ArrayAccessExp(std::shared_ptr<Exp> arr, std::shared_ptr<Exp> idx);
        void accept(Visitor* v) override;
};

class ThisExp : public Exp, public std::enable_shared_from_this<ThisExp> {
    public:
        ThisExp();
        void accept(Visitor* v) override;
};

class CastExp : public Exp, public std::enable_shared_from_this<CastExp> {
    public:
        std::shared_ptr<Exp> exp;
        std::shared_ptr<Type> type;
        CastExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t);
        void accept(Visitor* v) override;
};

class FieldAccessExp : public Exp, public std::enable_shared_from_this<FieldAccessExp> {
    public:
        std::shared_ptr<Exp> exp;
        std::shared_ptr<IdentifierExp> field;
        FieldAccessExp(std::shared_ptr<Exp> e, std::shared_ptr<IdentifierExp> f);
        void accept(Visitor* v) override;
};

class NewArrayExp: public Exp, public std::enable_shared_from_this<NewArrayExp> {
    public:
        std::shared_ptr<Exp> exp;
        std::shared_ptr<ArrayType> type;
        NewArrayExp(std::shared_ptr<Exp> e, std::shared_ptr<ArrayType> t);
        void accept(Visitor* v) override;
};

class NegExp : public Exp, public std::enable_shared_from_this<NegExp> {
    public:
        std::shared_ptr<Exp> exp;
        NegExp(std::shared_ptr<Exp> e);
        void accept(Visitor* v) override;
};

class ParExp : public Exp, public std::enable_shared_from_this<ParExp> {
    public:
        std::shared_ptr<Exp> exp;
        ParExp(std::shared_ptr<Exp> e);
        void accept(Visitor* v) override;
};

class LessExp: public Exp, public std::enable_shared_from_this<LessExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    LessExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class GreaterExp: public Exp, public std::enable_shared_from_this<GreaterExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    GreaterExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class LessEqualExp: public Exp, public std::enable_shared_from_this<LessEqualExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    LessEqualExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};


class GreaterEqualExp: public Exp, public std::enable_shared_from_this<GreaterEqualExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    GreaterEqualExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};


class InstanceOfExp: public Exp, public std::enable_shared_from_this<InstanceOfExp> {
public:
    std::shared_ptr<Exp> exp;
    std::shared_ptr<Type> type;
    InstanceOfExp(std::shared_ptr<Exp> exp, std::shared_ptr<Type> type);
    void accept(Visitor* v) override;
};

class EqualExp : public Exp, public std::enable_shared_from_this<EqualExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    EqualExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class NotEqualExp : public Exp, public std::enable_shared_from_this<NotEqualExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    NotEqualExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class AndExp : public Exp, public std::enable_shared_from_this<AndExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    AndExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class XorExp : public Exp, public std::enable_shared_from_this<XorExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    XorExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class OrExp : public Exp, public std::enable_shared_from_this<OrExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    OrExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class ConditionalAndExp : public Exp, public std::enable_shared_from_this<ConditionalAndExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    ConditionalAndExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};

class ConditionalOrExp : public Exp, public std::enable_shared_from_this<ConditionalOrExp> {
public:
    std::shared_ptr<Exp> exp1, exp2;
    ConditionalOrExp(std::shared_ptr<Exp> exp1, std::shared_ptr<Exp> exp2);
    void accept(Visitor* v) override;
};


class Assignment : public Exp, public std::enable_shared_from_this<Assignment> {
public:
    std::shared_ptr<Exp> left, right;
    Assignment(std::shared_ptr<Exp> left, std::shared_ptr<Exp> right);
    void accept(Visitor* v) override;
};

class ClassInstanceCreationExp : public Exp, public std::enable_shared_from_this<ClassInstanceCreationExp> {
public:
    std::shared_ptr<IdentifierType> classType;
    std::vector<std::shared_ptr<Exp>> arguments;
    ClassInstanceCreationExp(std::shared_ptr<IdentifierType> classType, std::vector<std::shared_ptr<Exp>> arguments);
    void accept(Visitor* v) override;
};

class ByteType : public Type, public std::enable_shared_from_this<ByteType> {
public:
    ByteType();
    std::string typeToString() override;
    void accept(Visitor* v) override;
};

class ShortType : public Type, public std::enable_shared_from_this<ShortType> {
public:
    ShortType();
    std::string typeToString() override;
    void accept(Visitor* v) override;
};

class CharType : public Type, public std::enable_shared_from_this<CharType> {
public:
    CharType();
    std::string typeToString() override;
    void accept(Visitor* v) override;
};

class IntType : public Type, public std::enable_shared_from_this<IntType> {
public:
    IntType();
    std::string typeToString() override;
    void accept(Visitor* v) override;
};

class BooleanType : public Type, public std::enable_shared_from_this<BooleanType> {
public:
    BooleanType();
    std::string typeToString() override;
    void accept(Visitor* v) override;
};

class VoidType : public Type, public std::enable_shared_from_this<VoidType> {
public:
    VoidType();
    std::string typeToString() override;
    void accept(Visitor* v) override;
};

class SemicolonStatement : public Statement, 
                           public std::enable_shared_from_this<SemicolonStatement>
{
public:
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "SemicolonStatement";
    }
};

class BlockStatements : public Statement, 
                        public std::enable_shared_from_this<BlockStatements>
{
public:
    std::vector<std::shared_ptr<Statement>> statements;
    void addStatement(std::shared_ptr<Statement> stmt);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "BlockStatements";
    }
};

class BlockStatement : public Statement,
                       public std::enable_shared_from_this<BlockStatement>
{
public:
    std::shared_ptr<BlockStatements> blockStatements;
    BlockStatement(std::shared_ptr<BlockStatements> sl);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "BlockStatement";
    }
};

// This is for if/then and if/else
class IfStatement : public Statement,
                    public std::enable_shared_from_this<IfStatement>
{
public:
    std::shared_ptr<Exp> exp;
    std::shared_ptr<Statement> statement1, statement2;
    IfStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement> s1, std::shared_ptr<Statement> s2);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "IfStatement";
    }
};

class WhileStatement : public Statement,
                       public std::enable_shared_from_this<WhileStatement>
{
public:
    std::shared_ptr<Exp> exp;
    std::shared_ptr<Statement> statement;
    WhileStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement>);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "WhileStatement";
    }
};

class LocalVariableDeclarationStatement : public Statement, 
                                          public std::enable_shared_from_this<LocalVariableDeclarationStatement>
{
public:
    std::shared_ptr<Type> type;
    std::shared_ptr<Identifier> id;
    std::shared_ptr<Exp> exp;
    LocalVariableDeclarationStatement(std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "LocalVariableDeclarationStatement";
    }

    void setType(std::shared_ptr<Type> t)
    {
        type = t;
    }
};

class ExpressionStatement : public Statement, 
                            public std::enable_shared_from_this<ExpressionStatement>
{
public:
    std::shared_ptr<Exp> exp;
    ExpressionStatement(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "ExpressionStatement";
    }
};

class ForStatement : public Statement, public std::enable_shared_from_this<ForStatement> {
public:
    std::shared_ptr<Statement> stmt1; // ForInit
    std::shared_ptr<Exp> exp; // ForExpression
    std::shared_ptr<Exp> expStmt2; // ForUpdate
    std::shared_ptr<Statement> stmt2;

    // Constructor for ExpressionStatement initialization
    ForStatement(std::shared_ptr<Statement> s1, std::shared_ptr<Exp> e, std::shared_ptr<Exp> es2);

    void accept(Visitor *v) override;

    void setStmt2(std::shared_ptr<Statement> s) {
        stmt2 = s;
    }

    inline std::string getStmtName() override
    {
        return "ForStatement";
    }
};

class ReturnStatement : public Statement, 
                        public std::enable_shared_from_this<ReturnStatement>
{
public:
    std::shared_ptr<Exp> exp;
    ReturnStatement(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
    inline std::string getStmtName() override
    {
        return "ReturnStatement";
    }
};

class FormalParameter : public AstNode, public std::enable_shared_from_this<FormalParameter> {
    public:
        std::shared_ptr<Type> type;
        std::shared_ptr<Identifier> variableName;

        FormalParameter(std::shared_ptr<Type> t, std::shared_ptr<Identifier> vn);
        void accept(Visitor* v) override;
};

class Field : public MemberDecl, public std::enable_shared_from_this<Field> {
    public:
        bool isStatic;
        bool isFinal;
        bool isProtected;
        std::shared_ptr<Type> type;
        std::shared_ptr<Identifier> fieldName;
        std::shared_ptr<Exp> initializer;

        Field(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> t, std::shared_ptr<Identifier> fn, 
        std::shared_ptr<Exp> i);
        void accept(Visitor* v) override;
        void setModifiers(std::vector<Modifiers>& m) override;
};

class Method : public MemberDecl, public std::enable_shared_from_this<Method> {
    public:
        bool isStatic;
        bool isNative;
        bool isPublic;
        bool isFinal;
        bool isAbstract;
        std::string className;
        std::shared_ptr<Type> type;
        std::shared_ptr<Identifier> methodName;
        std::vector<std::shared_ptr<FormalParameter>> formalParameters;
        std::shared_ptr<BlockStatement> block;

        Method(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> t, std::shared_ptr<Identifier> mn, 
        std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<BlockStatement> b);
        void accept(Visitor* v) override;
        void setModifiers(std::vector<Modifiers>& m) override;
        std::string getSignature() const;
};

class MethodInvocation : public Exp, public std::enable_shared_from_this<MethodInvocation> {
public:
    std::shared_ptr<Exp> primary;
    std::shared_ptr<IdentifierExp> primaryMethodName;
    std::shared_ptr<IdentifierType> ambiguousName;
    std::shared_ptr<IdentifierExp> ambiguousMethodName;
    std::shared_ptr<Method> method;
    std::vector<std::shared_ptr<Exp>> arguments;
    MethodInvocation(std::shared_ptr<Exp> primary, 
        std::shared_ptr<IdentifierExp> primaryMethodName,
        std::vector<std::shared_ptr<Exp>> arguments);
    MethodInvocation(std::shared_ptr<IdentifierType> ambiguousName,
    std::shared_ptr<IdentifierExp> ambiguousMethodName,
    std::vector<std::shared_ptr<Exp>> arguments);
    MethodInvocation(std::shared_ptr<IdentifierExp> ambiguousMethodName,
    std::vector<std::shared_ptr<Exp>> arguments);
    void accept(Visitor* v) override;
};

class Constructor : public MemberDecl, public std::enable_shared_from_this<Constructor> {
    public:
        std::shared_ptr<Identifier> constructorName;
        std::vector<std::shared_ptr<FormalParameter>> formalParameters;
        std::shared_ptr<BlockStatement> block;

        Constructor(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Identifier> cn, 
        std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<BlockStatement> b);
        void accept(Visitor* v) override;
        void setModifiers(std::vector<Modifiers>& m) override;
        std::string getSignature() const;
};

class ClassDecl : public ClassOrInterfaceDecl, public std::enable_shared_from_this<ClassDecl> {
    public:
        std::string modifier;
        std::vector<std::shared_ptr<IdentifierType>> implemented;
        //0 = Field, 1 = Method, 2 = Constructor
        std::vector<std::vector<std::shared_ptr<MemberDecl>>> declarations; 

        ClassDecl(std::string m, std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<IdentifierType>> e, 
        std::vector<std::shared_ptr<IdentifierType>> i, std::vector<std::vector<std::shared_ptr<MemberDecl>>> d);
        void accept(Visitor* v) override;
        bool isAbstract ()
        {
            return modifier == "abstract";
        }

        bool isFinal ()
        {
            return modifier == "final";
        }
};

class InterfaceDecl: public ClassOrInterfaceDecl, public std::enable_shared_from_this<InterfaceDecl> {
    public:
        std::vector<std::shared_ptr<Method>> methods;
        InterfaceDecl(std::shared_ptr<Identifier> in, std::vector<std::shared_ptr<IdentifierType>> e,
        std::vector<std::shared_ptr<Method>>& m);
        void accept(Visitor* v) override;
};

class Program : public AstNode, public std::enable_shared_from_this<Program> {
    public:
        std::shared_ptr<Package> package;
        std::shared_ptr<ImportStatements> importStatements;
        std::shared_ptr<ClassOrInterfaceDecl> classOrInterfaceDecl;
        std::shared_ptr<Scope> scope;
        Program(std::shared_ptr<Package> p, std::shared_ptr<ImportStatements> i, std::shared_ptr<ClassOrInterfaceDecl> c);
        void accept(Visitor* v) override;
        std::pair<std::string, std::string> getQualifiedName() const;

};

class Ast {
    private:
        std::shared_ptr<Program> program;
    public:
        void setAst(std::shared_ptr<Program> p);
        std::shared_ptr<Program> getAst() const;
};
