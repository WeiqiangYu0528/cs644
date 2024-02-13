#pragma once
#include <memory>
#include "ast.hpp"

class Visitor {
    public:
        virtual void visit(std::shared_ptr<PlusExp> n) = 0;
        virtual void visit(std::shared_ptr<MinusExp> n) = 0;
        virtual void visit(std::shared_ptr<TimesExp> n) = 0;
        virtual void visit(std::shared_ptr<DivideExp> n) = 0;
        virtual void visit(std::shared_ptr<ModuloExp> n) = 0;
        virtual void visit(std::shared_ptr<NotExp> n) = 0;
        virtual void visit(std::shared_ptr<IdentifierExp> n) = 0;
        virtual void visit(std::shared_ptr<IntegerLiteralExp> n) = 0;
        virtual void visit(std::shared_ptr<BoolLiteralExp> n) = 0;
        virtual void visit(std::shared_ptr<CharLiteralExp> n) = 0;
        virtual void visit(std::shared_ptr<StringLiteralExp> n) = 0;
        virtual void visit(std::shared_ptr<NulLiteralExp> n) = 0;
        virtual void visit(std::shared_ptr<ArrayAccessExp> n) = 0;
        virtual void visit(std::shared_ptr<ThisExp> n) = 0;
        virtual void visit(std::shared_ptr<CastExp> n) = 0;
        virtual void visit(std::shared_ptr<ArrayType> n) = 0;
        virtual void visit(std::shared_ptr<IdentifierType> n) = 0;
        virtual void visit(std::shared_ptr<FieldAccessExp> n) = 0;
        virtual void visit(std::shared_ptr<NewArrayExp> n) = 0;
        // virtual void visit(std::shared_ptr<CompoundType> n) = 0;
        virtual void visit(std::shared_ptr<NegExp> n) = 0;
        virtual void visit(std::shared_ptr<ParExp> n) = 0;
};
