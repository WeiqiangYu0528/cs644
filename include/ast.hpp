#pragma once
#include <iostream>
#include <memory>
#include <string>

class Visitor;

class Exp {
    public:
        bool notAName;
        ~Exp() = default;
        virtual void accept(Visitor* v) {};
};

class Ast {
    private:
        std::shared_ptr<Exp> exp;
    public:
        void setAst(std::shared_ptr<Exp> exp);
        std::shared_ptr<Exp> getAst() const;
};

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

