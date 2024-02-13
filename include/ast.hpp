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

class Identifier {
    public:
        std::string name;
        Identifier(std::string& s);
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

class MemberDecl : public std::enable_shared_from_this<MemberDecl> {
    public:
        MemberType memberType; //method or field or constructor or something

        std::shared_ptr<Type> returnType; //field / method
        std::shared_ptr<Identifier> name; //all
        bool initializer; //field
        std::shared_ptr<Exp> exp; //field
        std::vector<std::shared_ptr<FormalParameter>> formalParameters; //method / constructor
        std::shared_ptr<Block> block; //method with body / constructor

        //uninitialized field
        MemberDecl(MemberType mt, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> n); 
        //initialized field
        MemberDecl(MemberType mt, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> n, std::shared_ptr<Exp> e); 
        //method without body
        MemberDecl(MemberType mt, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> n, std::vector<std::shared_ptr<FormalParameter>> fp);
        //method with body
        MemberDecl(MemberType mt, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> n, std::vector<std::shared_ptr<FormalParameter>> fp,
        std::shared_ptr<Block> b);
        //constructor
        MemberDecl(MemberType mt, std::shared_ptr<Identifier> n, std::vector<std::shared_ptr<FormalParameter>> fp,
        std::shared_ptr<Block> b);

        ~MemberDecl() = default;
        void accept(Visitor* v);
};

class ClassBodyDeclaration : public std::enable_shared_from_this<ClassBodyDeclaration> {
    public:
        bool emptyClassBodyDeclaration;
        std::vector<Modifiers> modifiers;
        std::shared_ptr<MemberDecl> memberDecl;

        ClassBodyDeclaration();
        ClassBodyDeclaration(std::vector<Modifiers> m, std::shared_ptr<MemberDecl> md);
        void accept(Visitor* v);

};


class ClassBody : public std::enable_shared_from_this<ClassBody> {
    public:
        std::vector<std::shared_ptr<ClassBodyDeclaration>> classBodyDeclarations;

        ClassBody(std::vector<std::shared_ptr<ClassBodyDeclaration>> cbd);
        ~ClassBody() = default;
        void accept(Visitor* v);
};

class ClassDecl : public std::enable_shared_from_this<ClassDecl> {
    public:
        std::string modifier;
        std::shared_ptr<Identifier> className;
        std::vector<std::shared_ptr<IdentifierType>> extended;
        std::vector<std::shared_ptr<IdentifierType>> implemented;
        std::shared_ptr<ClassBody> classBody;

        ClassDecl(std::string m, std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<IdentifierType>> e,
        std::vector<std::shared_ptr<IdentifierType>> i, std::shared_ptr<ClassBody> cb);
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





