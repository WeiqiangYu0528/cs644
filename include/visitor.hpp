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

        virtual void visit(std::shared_ptr<ResultType> n) = 0;

        virtual void visit(std::shared_ptr<IdentifierType> n) = 0;
        virtual void visit(std::shared_ptr<FieldAccessExp> n) = 0;
        virtual void visit(std::shared_ptr<NewArrayExp> n) = 0;
        // virtual void visit(std::shared_ptr<CompoundType> n) = 0;
        virtual void visit(std::shared_ptr<NegExp> n) = 0;
        virtual void visit(std::shared_ptr<ParExp> n) = 0;

        virtual void visit(std::shared_ptr<ClassBodyDeclaration> n) = 0;
        virtual void visit(std::shared_ptr<ClassBody> n) = 0;
        virtual void visit(std::shared_ptr<TypeParameter> n) = 0;
        virtual void visit(std::shared_ptr<NormalClassDecl> n) = 0;
        virtual void visit(std::shared_ptr<ClassDecl> n) = 0;

        virtual void visit(std::shared_ptr<FieldDeclaratorsRest> n) = 0;
        virtual void visit(std::shared_ptr<MethodDeclaratorRest> n) = 0; //to be removed (MDR will be abstract)
        virtual void visit(std::shared_ptr<MethodOrFieldDecl> n) = 0;
        virtual void visit(std::shared_ptr<ConstructorDecl> n) = 0;
        virtual void visit(std::shared_ptr<ConstructorDeclaratorRest> n) = 0;
        
};

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

        void visit(std::shared_ptr<ResultType> n) override;

        void visit(std::shared_ptr<IdentifierType> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<NewArrayExp> n) override;
        // void visit(std::shared_ptr<CompoundType> n) override;
        void visit(std::shared_ptr<NegExp> n) override;
        void visit(std::shared_ptr<ParExp> n) override;

        void visit(std::shared_ptr<ClassBodyDeclaration> n) override;
        void visit(std::shared_ptr<ClassBody> n) override;
        void visit(std::shared_ptr<TypeParameter> n) override;
        void visit(std::shared_ptr<NormalClassDecl> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;

        void visit(std::shared_ptr<FieldDeclaratorsRest> n) override;
        void visit(std::shared_ptr<MethodDeclaratorRest> n) override; //to be removed (MDR will be abstract)
        void visit(std::shared_ptr<MethodOrFieldDecl> n) override;
        void visit(std::shared_ptr<ConstructorDecl> n) override;
        void visit(std::shared_ptr<ConstructorDeclaratorRest> n) override;
        
};
