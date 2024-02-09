#pragma once
#include <memory>
#include "ast.hpp"

class Visitor {
    public:
        void visit(std::shared_ptr<PlusExp> n);
        void visit(std::shared_ptr<MinusExp> n);
        void visit(std::shared_ptr<TimesExp> n);
        void visit(std::shared_ptr<DivideExp> n);
        void visit(std::shared_ptr<ModuloExp> n);
        void visit(std::shared_ptr<NotExp> n);
        void visit(std::shared_ptr<IdentifierExp> n);
        void visit(std::shared_ptr<IntegerLiteral> n);
        void visit(std::shared_ptr<BoolLiteral> n);
        void visit(std::shared_ptr<CharLiteral> n);
        void visit(std::shared_ptr<StringLiteral> n);
        void visit(std::shared_ptr<NulLiteral> n);
        void visit(std::shared_ptr<ArrayAccessExp> n);
        void visit(std::shared_ptr<ThisExp> n);
        void visit(std::shared_ptr<CastExp> n);
        void visit(std::shared_ptr<ArrayType> n);
        void visit(std::shared_ptr<IdentifierType> n);
        void visit(std::shared_ptr<FieldAccessExp> n);
        void visit(std::shared_ptr<NewArrayExp> n);
        // void visit(std::shared_ptr<CompoundType> n);
        void visit(std::shared_ptr<NegExp> n);
};
