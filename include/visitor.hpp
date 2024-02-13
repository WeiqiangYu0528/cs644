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

        virtual void visit(std::shared_ptr<LessExp> n) = 0;
        virtual void visit(std::shared_ptr<GreaterExp> n) = 0;
        virtual void visit(std::shared_ptr<LessEqualExp> n) = 0;
        virtual void visit(std::shared_ptr<GreaterEqualExp> n) = 0;       
        virtual void visit(std::shared_ptr<InstanceOfExp> n) = 0;  
        
        virtual void visit(std::shared_ptr<EqualExp> n) = 0;
        virtual void visit(std::shared_ptr<NotEqualExp> n) = 0;
        virtual void visit(std::shared_ptr<AndExp> n) = 0;
        virtual void visit(std::shared_ptr<XorExp> n) = 0;
        virtual void visit(std::shared_ptr<OrExp> n) = 0;
        virtual void visit(std::shared_ptr<ConditionalAndExp> n) = 0;
        virtual void visit(std::shared_ptr<ConditionalOrExp> n) = 0;        

        virtual void visit(std::shared_ptr<Assignment> n) = 0;
        virtual void visit(std::shared_ptr<MethodInvocation> n) = 0;
        virtual void visit(std::shared_ptr<ClassInstanceCreationExp> n) = 0;

        virtual void visit(std::shared_ptr<ByteType> t) = 0;
        virtual void visit(std::shared_ptr<ShortType> t) = 0;
        virtual void visit(std::shared_ptr<CharType> t) = 0;
        virtual void visit(std::shared_ptr<IntType> t) = 0;
        virtual void visit(std::shared_ptr<BooleanType> t) = 0;
        virtual void visit(std::shared_ptr<VoidType> t) = 0;        
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
        void visit(std::shared_ptr<IdentifierType> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<NewArrayExp> n) override;
        // void visit(std::shared_ptr<CompoundType> n) override;
        void visit(std::shared_ptr<NegExp> n) override;
        void visit(std::shared_ptr<ParExp> n) override;

        void visit(std::shared_ptr<LessExp> n) override;
        void visit(std::shared_ptr<GreaterExp> n) override;
        void visit(std::shared_ptr<LessEqualExp> n) override;
        void visit(std::shared_ptr<GreaterEqualExp> n) override;   
        void visit(std::shared_ptr<InstanceOfExp> n) override; 
        void visit(std::shared_ptr<EqualExp> n) override;
        void visit(std::shared_ptr<NotEqualExp> n) override;
        void visit(std::shared_ptr<AndExp> n) override;
        void visit(std::shared_ptr<XorExp> n) override;
        void visit(std::shared_ptr<OrExp> n) override;
        void visit(std::shared_ptr<ConditionalAndExp> n) override;
        void visit(std::shared_ptr<ConditionalOrExp> n) override;  

        void visit(std::shared_ptr<Assignment> n) override;     
        void visit(std::shared_ptr<MethodInvocation> n) override;            
        void visit(std::shared_ptr<ClassInstanceCreationExp> n) override; 

        void visit(std::shared_ptr<ByteType> t) override;
        void visit(std::shared_ptr<ShortType> t) override;
        void visit(std::shared_ptr<CharType> t) override;
        void visit(std::shared_ptr<IntType> t) override;
        void visit(std::shared_ptr<BooleanType> t) override;
        void visit(std::shared_ptr<VoidType> t) override;      
};
