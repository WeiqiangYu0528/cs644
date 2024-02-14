#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "weeder.hpp"

class Visitor;

class Exp {
    public:
        ~Exp() = default;
        virtual void accept(Visitor* v);
};

class Type {
    public:
        DataType type;
        // should delete this default constructor later, currently used as a placeholder for basic case types
        Type() = default;
        Type(DataType t);
        ~Type() = default;
        virtual void accept(Visitor* v);
};

class Identifier : public std::enable_shared_from_this<Identifier> {
    public:
        std::string name;
        Identifier(std::string& s);
        void accept(Visitor* v);
};

class IdentifierType : public Type, public std::enable_shared_from_this<IdentifierType> {
    public:
        std::shared_ptr<Identifier> id;
        IdentifierType(std::shared_ptr<Identifier> i);
        void accept(Visitor* v) override;
};

class ArrayType : public Type, public std::enable_shared_from_this<ArrayType> {
    public:
        std::shared_ptr<Type> dataType;
        ArrayType(std::shared_ptr<Type> t);
        void accept(Visitor* v) override;
};

class Package : public std::enable_shared_from_this<Package>  {
    std::shared_ptr<Identifier> id;
    public:
        Package(std::shared_ptr<Identifier> i);
        void accept(Visitor* v);
};

class ImportStatement : public std::enable_shared_from_this<ImportStatement> {
    std::vector<std::shared_ptr<Identifier>> stmts;
    public:
        ImportStatement();
        void addImport(std::shared_ptr<Identifier> i);
        void accept(Visitor* v);
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
        IdentifierExp(std::shared_ptr<Identifier> i);
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
        std::shared_ptr<Identifier> id;
        FieldAccessExp(std::shared_ptr<Exp> e, std::shared_ptr<Identifier> i);
        void accept(Visitor* v) override;
};

class NewArrayExp: public Exp, public std::enable_shared_from_this<NewArrayExp> {
    public:
        std::shared_ptr<Exp> exp;
        std::shared_ptr<Type> type;
        NewArrayExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t);
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

class MethodInvocation : public Exp, public std::enable_shared_from_this<MethodInvocation> {
public:
    std::shared_ptr<Exp> primary;
    std::shared_ptr<Identifier> primaryMethodName;
    std::shared_ptr<IdentifierType> methodName;
    std::vector<std::shared_ptr<Exp>> arguments;


    MethodInvocation(std::shared_ptr<Exp> primary, 
        std::shared_ptr<Identifier> primaryMethodName,
        std::shared_ptr<IdentifierType> methodName, 
        std::vector<std::shared_ptr<Exp>> arguments);
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
    void accept(Visitor* v) override;
};

class ShortType : public Type, public std::enable_shared_from_this<ShortType> {
public:
    ShortType();
    void accept(Visitor* v) override;
};

class CharType : public Type, public std::enable_shared_from_this<CharType> {
public:
    CharType();
    void accept(Visitor* v) override;
};

class IntType : public Type, public std::enable_shared_from_this<IntType> {
public:
    IntType();
    void accept(Visitor* v) override;
};

class BooleanType : public Type, public std::enable_shared_from_this<BooleanType> {
public:
    BooleanType();
    void accept(Visitor* v) override;
};


class VoidType : public Type, public std::enable_shared_from_this<VoidType> {
public:
    VoidType();
    void accept(Visitor* v) override;
};

class Block {
    //Needs to be implemented
};

class FormalParameter : public std::enable_shared_from_this<FormalParameter> {
    public:
        std::shared_ptr<Type> type;
        std::shared_ptr<Identifier> variableName;

        FormalParameter(std::shared_ptr<Type> t, std::shared_ptr<Identifier> vn);
        void accept(Visitor* v);
};

class MemberDecl {
    public:
        MemberType memberType;
        std::vector<Modifiers> modifiers;

        MemberDecl(MemberType mt, std::vector<Modifiers> m);
        virtual void accept(Visitor* v) = 0;
};

class Field : public MemberDecl, public std::enable_shared_from_this<Field> {
    public:
        std::shared_ptr<Type> returnType;
        std::shared_ptr<Identifier> fieldName;
        std::shared_ptr<Exp> initializer;

        Field(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> fn, 
        std::shared_ptr<Exp> i);
        void accept(Visitor* v) override;
};

class Method : public MemberDecl, public std::enable_shared_from_this<Method> {
    public:
        std::shared_ptr<Type> returnType;
        std::shared_ptr<Identifier> methodName;
        std::vector<std::shared_ptr<FormalParameter>> formalParameters;
        std::shared_ptr<Block> block;

        Method(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> mn, 
        std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<Block> b);
        void accept(Visitor* v) override;
};

class Constructor : public MemberDecl, public std::enable_shared_from_this<Constructor> {
    public:
        std::shared_ptr<Identifier> constructorName;
        std::vector<std::shared_ptr<FormalParameter>> formalParameters;
        std::shared_ptr<Block> block;

        Constructor(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Identifier> cn, 
        std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<Block> b);
        void accept(Visitor* v) override;
};

class ClassDecl : public std::enable_shared_from_this<ClassDecl> {
    public:
        std::string modifier;
        std::shared_ptr<Identifier> className;
        std::vector<std::shared_ptr<IdentifierType>> extended;
        std::vector<std::shared_ptr<IdentifierType>> implemented;
        //0 = Field, 1 = Method, 2 = Constructor
        std::vector<std::vector<std::shared_ptr<MemberDecl>>> declarations; 

        ClassDecl(std::string m, std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<IdentifierType>> e, 
        std::vector<std::shared_ptr<IdentifierType>> i, std::vector<std::vector<std::shared_ptr<MemberDecl>>> d);
        ~ClassDecl() = default;
        void accept(Visitor* v);
};

class Ast {
    private:
        //std::shared_ptr<Exp> exp;
        std::shared_ptr<ClassDecl> cdecl;
    public:
        //void setAst(std::shared_ptr<Exp> exp);
        void setAst(std::shared_ptr<ClassDecl> cdecl);
        //std::shared_ptr<Exp> getAst() const;
        std::shared_ptr<ClassDecl> getAst() const;
};
