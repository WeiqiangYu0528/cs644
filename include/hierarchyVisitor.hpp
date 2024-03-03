#pragma once
#include <memory>
#include "scope.hpp"
#include "visitor.hpp"

class HierarchyVisitor: public Visitor {
    public:
        std::shared_ptr<Scope> scope;
        bool error;
        HierarchyVisitor(std::shared_ptr<Scope> s);
        void visit(std::shared_ptr<Program> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        void visit(std::shared_ptr<Method> n) override;
        bool isError() const;
};
