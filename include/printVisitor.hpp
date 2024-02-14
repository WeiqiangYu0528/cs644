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
        void visit(std::shared_ptr<Package> n) override;
        void visit(std::shared_ptr<ImportStatement> n) override;
        void visit(std::shared_ptr<Identifier> n) override;

        // Statement part begin
        void visit(std::shared_ptr<BlockStatement> n) override;
        void visit(std::shared_ptr<BlockStatements> n) override;
        void visit(std::shared_ptr<SemicolonStatement> n) override;
        void visit(std::shared_ptr<IfStatement> n) override;
        void visit(std::shared_ptr<WhileStatement> n) override;
        void visit(std::shared_ptr<ForStatement> n) override;
        void visit(std::shared_ptr<ReturnStatement> n) override;
        void visit(std::shared_ptr<ExpressionStatement> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        // Statement part end
};
