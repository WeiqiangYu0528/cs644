#include "visitor.hpp"

void PrintVisitor::visit(std::shared_ptr<PlusExp> n) {
    std::cout << "Visiting PlusExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<MinusExp> n) { 
    std::cout << "Visiting MinusExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<TimesExp> n) { 
    std::cout << "Visiting TimesExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<DivideExp> n) { 
    std::cout << "Visiting DivideExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ModuloExp> n) { 
    std::cout << "Visiting ModuloExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<NotExp> n) { 
    std::cout << "Visiting NotExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<IdentifierExp> n) {
    std::cout << "Visiting Identifier Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<IntegerLiteralExp> n) { 
    std::cout << "Visiting IntegerLiteral" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<BoolLiteralExp> n) { 
    std::cout << "Visiting BoolLiteral" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<CharLiteralExp> n) { 
    std::cout << "Visiting CharLiteral" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<StringLiteralExp> n) { 
    std::cout << "Visiting StringLiteral" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<NulLiteralExp> n) { 
    std::cout << "Visiting NulLiteral" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ArrayAccessExp> n) { 
    std::cout << "Visiting ArrayAccess" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ThisExp> n) { 
    std::cout << "Visiting This" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<CastExp> n) { 
    std::cout << "Visiting CastExpression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ArrayType> n) { 
    std::cout << "Visiting ArrayType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<IdentifierType> n) { 
    std::cout << "Visiting IdentifierType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<FieldAccessExp> n) { 
    std::cout << "Visiting FieldAccess" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<NewArrayExp> n) { 
    std::cout << "Visiting NewArray" << std::endl;
}
// void PrintVisitor::visit(std::shared_ptr<CompoundType> n) { 
//     std::cout << "Visiting CompoundType" << std::endl;
// }
void PrintVisitor::visit(std::shared_ptr<NegExp> n) {
    std::cout << "Visiting NegExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ParExp> n) {
    std::cout << "Visiting ParExp" << std::endl;
}
