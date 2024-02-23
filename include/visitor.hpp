#pragma once
#include <memory>
#include "ast.hpp"

class Visitor {
    public:
        virtual void visit(std::shared_ptr<PlusExp> n);
        virtual void visit(std::shared_ptr<MinusExp> n);
        virtual void visit(std::shared_ptr<TimesExp> n);
        virtual void visit(std::shared_ptr<DivideExp> n);
        virtual void visit(std::shared_ptr<ModuloExp> n);
        virtual void visit(std::shared_ptr<NotExp> n);
        virtual void visit(std::shared_ptr<IdentifierExp> n);
        virtual void visit(std::shared_ptr<IntegerLiteralExp> n);
        virtual void visit(std::shared_ptr<BoolLiteralExp> n);
        virtual void visit(std::shared_ptr<CharLiteralExp> n);
        virtual void visit(std::shared_ptr<StringLiteralExp> n);
        virtual void visit(std::shared_ptr<NulLiteralExp> n);
        virtual void visit(std::shared_ptr<ArrayAccessExp> n);
        virtual void visit(std::shared_ptr<ThisExp> n);
        virtual void visit(std::shared_ptr<CastExp> n);
        virtual void visit(std::shared_ptr<ArrayType> n);
        virtual void visit(std::shared_ptr<IdentifierType> n);
        virtual void visit(std::shared_ptr<FieldAccessExp> n);
        virtual void visit(std::shared_ptr<NewArrayExp> n);
        // virtual void visit(std::shared_ptr<CompoundType> n);
        virtual void visit(std::shared_ptr<NegExp> n);
        virtual void visit(std::shared_ptr<ParExp> n);
        virtual void visit(std::shared_ptr<LessExp> n);
        virtual void visit(std::shared_ptr<GreaterExp> n);
        virtual void visit(std::shared_ptr<LessEqualExp> n);
        virtual void visit(std::shared_ptr<GreaterEqualExp> n);       
        virtual void visit(std::shared_ptr<InstanceOfExp> n);  
        virtual void visit(std::shared_ptr<EqualExp> n);
        virtual void visit(std::shared_ptr<NotEqualExp> n);
        virtual void visit(std::shared_ptr<AndExp> n);
        virtual void visit(std::shared_ptr<XorExp> n);
        virtual void visit(std::shared_ptr<OrExp> n);
        virtual void visit(std::shared_ptr<ConditionalAndExp> n);
        virtual void visit(std::shared_ptr<ConditionalOrExp> n);        
        virtual void visit(std::shared_ptr<Assignment> n);
        virtual void visit(std::shared_ptr<MethodInvocation> n);
        virtual void visit(std::shared_ptr<ClassInstanceCreationExp> n);
        virtual void visit(std::shared_ptr<ByteType> t);
        virtual void visit(std::shared_ptr<ShortType> t);
        virtual void visit(std::shared_ptr<CharType> t);
        virtual void visit(std::shared_ptr<IntType> t);
        virtual void visit(std::shared_ptr<BooleanType> t);
        virtual void visit(std::shared_ptr<VoidType> t);
        virtual void visit(std::shared_ptr<Package> n);
        virtual void visit(std::shared_ptr<ImportStatement> n);
        virtual void visit(std::shared_ptr<ImportStatements> n);
        virtual void visit(std::shared_ptr<Identifier> n);
        virtual void visit(std::shared_ptr<FormalParameter> n);
        virtual void visit(std::shared_ptr<Constructor> n);
        virtual void visit(std::shared_ptr<Method> n);
        virtual void visit(std::shared_ptr<Field> n);
        virtual void visit(std::shared_ptr<ClassDecl> n);
        virtual void visit(std::shared_ptr<InterfaceDecl> n);
        virtual void visit(std::shared_ptr<Program> n);
        // Statement part begin
        virtual void visit(std::shared_ptr<BlockStatement> n);
        virtual void visit(std::shared_ptr<BlockStatements> n);
        virtual void visit(std::shared_ptr<SemicolonStatement> n);
        virtual void visit(std::shared_ptr<IfStatement> n);
        virtual void visit(std::shared_ptr<WhileStatement> n);
        virtual void visit(std::shared_ptr<ForStatement> n);
        virtual void visit(std::shared_ptr<ReturnStatement> n);
        virtual void visit(std::shared_ptr<ExpressionStatement> n);
        virtual void visit(std::shared_ptr<LocalVariableDeclarationStatement> n);
        // Statement part end
};
