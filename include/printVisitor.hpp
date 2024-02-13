#pragma once
#include "visitor.hpp"

class PrintVisitor: public Visitor {
    public:
        void visit(std::shared_ptr<PlusExp> n) override;
        void visit(std::shared_ptr<MinusExp> n) override;
        void visit(std::shared_ptr<TimesExp> n) override;
        void visit(std::shared_ptr<DivideExp> n) override;
        void visit(std::shared_ptr<ModuloExp> n) override;
        void visit(std::shared_ptr<NotExp> n) override;
        void visit(std::shared_ptr<IdentifierExp> n) override;
        void visit(std::shared_ptr<IntegerLiteralExp> n) override;
        void visit(std::shared_ptr<BoolLiteralExp> n) override;
        void visit(std::shared_ptr<CharLiteralExp> n) override;
        void visit(std::shared_ptr<StringLiteralExp> n) override;
        void visit(std::shared_ptr<NulLiteralExp> n) override;
        void visit(std::shared_ptr<ArrayAccessExp> n) override;
        void visit(std::shared_ptr<ThisExp> n) override;
        void visit(std::shared_ptr<CastExp> n) override;
        void visit(std::shared_ptr<ArrayType> n) override;
        void visit(std::shared_ptr<IdentifierType> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<NewArrayExp> n) override;
        // void visit(std::shared_ptr<CompoundType> n) override;
        void visit(std::shared_ptr<NegExp> n) override;
        void visit(std::shared_ptr<ParExp> n) override;
};