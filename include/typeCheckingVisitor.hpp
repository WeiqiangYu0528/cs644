#pragma once
#include <memory>
#include "scope.hpp"
#include "visitor.hpp"

class TypeCheckingVisitor : public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        bool error;
        // used to determine a scope for field initialization
        bool initialized;
    public:
        TypeCheckingVisitor(std::shared_ptr<Scope> s);
        void visit(std::shared_ptr<FormalParameter> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        void visit(std::shared_ptr<Constructor> n) override;
        void visit(std::shared_ptr<Method> n) override;
        void visit(std::shared_ptr<Field> n) override;
        void visit(std::shared_ptr<BlockStatement> n) override;
        void visit(std::shared_ptr<IfStatement> n) override;
        void visit(std::shared_ptr<WhileStatement> n) override;
        void visit(std::shared_ptr<ForStatement> n) override;
        void visit(std::shared_ptr<IdentifierExp> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<MethodInvocation> n) override;
        void visit(std::shared_ptr<Assignment> n) override;
        bool isError() const;

        std::string currentPackageName;
        std::string currentClassName;
};
