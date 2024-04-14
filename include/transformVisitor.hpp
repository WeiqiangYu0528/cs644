#pragma once
#include <memory>
#include "IRAst.hpp"
#include "ast.hpp"
#include "visitor.hpp"
#include "scope.hpp"

class TransformVisitor : public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        std::shared_ptr<TIR::NodeFactory> nodeFactory;
        std::shared_ptr<TIR::Node> node;
        static int labelCounter;
        static int arrayCounter;
    public:
        std::string className;
        TransformVisitor(std::shared_ptr<Scope> s, std::shared_ptr<TIR::NodeFactory> nf);
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
        // void visit(std::shared_ptr<StringLiteralExp> n) override;
        void visit(std::shared_ptr<NulLiteralExp> n) override;
        void visit(std::shared_ptr<ArrayAccessExp> n) override;
        // void visit(std::shared_ptr<ThisExp> n) override;
        void visit(std::shared_ptr<CastExp> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<NewArrayExp> n) override;
        void visit(std::shared_ptr<NegExp> n) override;
        void visit(std::shared_ptr<ParExp> n) override;
        void visit(std::shared_ptr<LessExp> n) override;
        void visit(std::shared_ptr<GreaterExp> n) override;
        void visit(std::shared_ptr<LessEqualExp> n) override;
        void visit(std::shared_ptr<GreaterEqualExp> n) override;       
        // void visit(std::shared_ptr<InstanceOfExp> n) override;  
        void visit(std::shared_ptr<EqualExp> n) override;
        void visit(std::shared_ptr<NotEqualExp> n) override;
        void visit(std::shared_ptr<ConditionalAndExp> n) override;
        void visit(std::shared_ptr<ConditionalOrExp> n) override;     
        void visit(std::shared_ptr<Assignment> n) override;
        void visit(std::shared_ptr<MethodInvocation> n) override;
        void visit(std::shared_ptr<ClassInstanceCreationExp> n) override;
        void visit(std::shared_ptr<Constructor> n) override;
        void visit(std::shared_ptr<Method> n) override;
        void visit(std::shared_ptr<Field> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        void visit(std::shared_ptr<Program> n) override;
        void visit(std::shared_ptr<BlockStatement> n) override;
        void visit(std::shared_ptr<BlockStatements> n) override;
        void visit(std::shared_ptr<SemicolonStatement> n) override;
        void visit(std::shared_ptr<IfStatement> n) override;
        void visit(std::shared_ptr<WhileStatement> n) override;
        void visit(std::shared_ptr<ForStatement> n) override;
        void visit(std::shared_ptr<ReturnStatement> n) override;
        void visit(std::shared_ptr<ExpressionStatement> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        void visit(std::shared_ptr<AndExp> n) override;
        void visit(std::shared_ptr<XorExp> n) override;
        void visit(std::shared_ptr<OrExp> n) override;
        std::shared_ptr<TIR::CompUnit> getCompUnit() const;
        std::shared_ptr<TIR::Expr> getExpr() const;
        std::shared_ptr<TIR::Stmt> getStmt() const;
        std::shared_ptr<TIR::ESeq> getCastExpr(int mask) const;
};
