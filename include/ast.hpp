#pragma once
#include <iostream>
#include <memory>
#include <string>
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

class ResultType : public Type, public std::enable_shared_from_this<ResultType> {
    public:
        std::shared_ptr<Type> dataType;
        ResultType(std::shared_ptr<Type> t) : Type(t->type), dataType(t) {
            std::cout << "ResultType constructor" << std::endl;
        }
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




//Imperative: Anything included here gets fully implemented in parser.yy, then test
// then we add the necessary visitor stuff
// then we test 
//then we move things to ast.cpp as needed

class MethodOrFieldRest {
    public:
        MemberType memberType;
        MethodOrFieldRest(MemberType m) : memberType(m) {
            std::cout << "MethodOrFieldRest constructor" << std::endl;
        }
        virtual void accept(Visitor* v) = 0;
};

class FieldDeclaratorsRest : public MethodOrFieldRest, public std::enable_shared_from_this<FieldDeclaratorsRest> {
    public:
        FieldDeclaratorsRest(MemberType m) : MethodOrFieldRest(m) {
            std::cout << "FieldDeclaratorsRest constructor" << std::endl;
        }
        void accept(Visitor* v) override;
};

class MethodDeclaratorRest : public MethodOrFieldRest, public std::enable_shared_from_this<MethodDeclaratorRest> {
    public:
        MethodDeclaratorRest(MemberType m) : MethodOrFieldRest(m) {
            std::cout << "MethodDeclaratorRest constructor" << std::endl;
        }
        void accept(Visitor* v) override;
};

class MemberDecl {
    public:
        MemberType memberType;
        MemberDecl(MemberType m) : memberType(m) {}

        ~MemberDecl() = default;
        virtual void accept(Visitor* v) = 0;
};

class MethodOrFieldDecl : public MemberDecl, public std::enable_shared_from_this<MethodOrFieldDecl> {
    public:
        std::shared_ptr<Type> resultType;
        std::shared_ptr<Identifier> variable;
        std::shared_ptr<MethodOrFieldRest> methodOrFieldRest;

        MethodOrFieldDecl(std::shared_ptr<Type> rt, std::shared_ptr<Identifier> v, 
        std::shared_ptr<MethodOrFieldRest> mofr) : MemberDecl(mofr->memberType), resultType(rt), variable(v), methodOrFieldRest(mofr)
         {
            std::cout << "MethodOrFieldDecl constructor" << std::endl;
        }

        void accept(Visitor* v) override;
};

class ConstructorDecl : public MemberDecl, public std::enable_shared_from_this<ConstructorDecl> {
    public:
        std::shared_ptr<Identifier> variable;
        //std::shared_ptr<CDR> cdr; NOT YET IMPLEMENTED

        ConstructorDecl(std::shared_ptr<Identifier> v/*, std::shared_ptr<CDR> c*/, MemberType m) : MemberDecl(m), 
        variable(v)/*, cdr(c)*/ 
        { assert(m == MemberType::CONSTRUCTOR); 
            std::cout << "ConstructorDecl constructor" << std::endl;
        }

        void accept(Visitor* v) override;

};


























class ClassBody : public std::enable_shared_from_this<ClassBody> {
    public:

        //classbody is made of a list of ClassBodyDeclaration
        //ClassBodyDeclaration

        ClassBody(); //Not yet implemented
        ~ClassBody() = default;
        void accept(Visitor* v);
};

class TypeParameter : public std::enable_shared_from_this<TypeParameter> {
    public:
        std::shared_ptr<Identifier> typeParameterName; //Variable
        std::vector<std::shared_ptr<Type>> extended; //TypeParameterOpt1 -> List of ReferenceType

        TypeParameter(std::shared_ptr<Identifier> tpn, std::vector<std::shared_ptr<Type>> e);
        ~TypeParameter() = default;
        void accept(Visitor* v);
};

class NormalClassDecl : public std::enable_shared_from_this<NormalClassDecl> {
    public:
        std::shared_ptr<Identifier> className; //Variable
        std::vector<std::shared_ptr<TypeParameter>> typeParams; //NormalClassDeclarationOpt1 -> List of TypeParameter
        std::vector<std::shared_ptr<IdentifierType>> extended; //NormalClassDeclarationOpt2 -> List of ClassOrInterfaceType
        std::vector<std::shared_ptr<IdentifierType>> implemented; //NormalClassDeclarationOpt3 -> List of ClassOrInterfaceType
        std::shared_ptr<ClassBody> classBody;

        NormalClassDecl(std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<TypeParameter>> tp,
        std::vector<std::shared_ptr<IdentifierType>> e, std::vector<std::shared_ptr<IdentifierType>> i,
        std::shared_ptr<ClassBody> cb);
        ~NormalClassDecl() = default;
        void accept(Visitor* v);
};

class ClassDecl : public std::enable_shared_from_this<ClassDecl> {
    public:
        std::string modifier;
        std::shared_ptr<NormalClassDecl> ncdecl;
        ClassDecl(std::string m, std::shared_ptr<NormalClassDecl> n);
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





