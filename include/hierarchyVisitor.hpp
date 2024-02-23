#pragma once
#include <memory>
#include "scope.hpp"
#include "visitor.hpp"

class HierarchyVisitor: public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        bool error;

    public:
        HierarchyVisitor(std::shared_ptr<Scope> s);
        void visit(std::shared_ptr<Program> n) override;
        void visit(std::shared_ptr<ClassDecl> n) override;
        void visit(std::shared_ptr<InterfaceDecl> n) override;
        bool isError() const;
};
