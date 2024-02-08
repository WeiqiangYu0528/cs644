#pragma once
#include <memory>
#include "ast.hpp"

class Visitor {
    public:
        void visit(std::shared_ptr<PlusExp> exp);
        void visit(std::shared_ptr<MinusExp> exp);
        void visit(std::shared_ptr<TimesExp> exp);
        void visit(std::shared_ptr<DivideExp> exp);
        void visit(std::shared_ptr<ModuloExp> exp);
};
