#include "visitor.hpp"

void Visitor::visit(std::shared_ptr<PlusExp> exp) {
    std::cout << "Visiting PlusExp" << std::endl;
    exp->exp1->accept(this);
    exp->exp2->accept(this);
}
void Visitor::visit(std::shared_ptr<MinusExp> exp) { 
    std::cout << "Visiting MinusExp" << std::endl;
    exp->exp1->accept(this);
    exp->exp2->accept(this);
}
void Visitor::visit(std::shared_ptr<TimesExp> exp) { 
    std::cout << "Visiting TimesExp" << std::endl;
    exp->exp1->accept(this);
    exp->exp2->accept(this);
}
void Visitor::visit(std::shared_ptr<DivideExp> exp) { 
    std::cout << "Visiting DivideExp" << std::endl;
    exp->exp1->accept(this);
    exp->exp2->accept(this);
}
void Visitor::visit(std::shared_ptr<ModuloExp> exp) { 
    std::cout << "Visiting ModuloExp" << std::endl;
    exp->exp1->accept(this);
    exp->exp2->accept(this);
}
