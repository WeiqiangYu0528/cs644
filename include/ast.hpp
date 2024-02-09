#pragma once
#include <iostream>
#include <memory>
#include <string>
#include "weeder.hpp"

class Visitor;

class Exp {
    public:
        bool notAName;
        ~Exp() = default;
        virtual void accept(Visitor* v) {};
};

class Type {
    public:
        DataType type;
        Type() = default;
        Type(DataType t) : type(t) {};
        ~Type() = default;
        virtual void accept(Visitor* v) {};
};

class Ast {
    private:
        std::shared_ptr<Exp> exp;
    public:
        void setAst(std::shared_ptr<Exp> exp);
        std::shared_ptr<Exp> getAst() const;
};

class IdentifierType : public Type, public std::enable_shared_from_this<IdentifierType> {
    public:
        std::string name;
        IdentifierType(std::string& s);
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
        std::string name;
        IdentifierExp(std::string& s);
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
        std::string field;
        FieldAccessExp(std::shared_ptr<Exp> e, std::string& s);
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
