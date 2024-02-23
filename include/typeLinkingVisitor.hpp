#pragma once
#include <memory>
#include "scope.hpp"
#include "visitor.hpp"

class TypeLinkingVisitor : public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        bool error;
    public:
        TypeLinkingVisitor(std::shared_ptr<Scope> s);
        void visit(std::shared_ptr<ImportStatements> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        // void visit(std::shared_ptr<IdentifierType> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        void visit(std::shared_ptr<ClassInstanceCreationExp> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        void visit(std::shared_ptr<Program> n) override;
        bool isError() const;

        std::string currentPackageName;
        std::string currentClassName;
};
