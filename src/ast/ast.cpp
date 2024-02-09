#include "ast.hpp"
#include "visitor.hpp"

void Ast::setAst(std::shared_ptr<Exp> e) {
    exp = e;
}

std::shared_ptr<Exp> Ast::getAst() const {
    return exp;
}

PlusExp::PlusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "PlusExp constructor" << std::endl;
}

void PlusExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

MinusExp::MinusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Minus constructor" << std::endl;
}

void MinusExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

TimesExp::TimesExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Times constructor" << std::endl;
}

void TimesExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

DivideExp::DivideExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Divide constructor" << std::endl;
}

void DivideExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ModuloExp::ModuloExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Modulo constructor" << std::endl;
}

void ModuloExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NotExp::NotExp(std::shared_ptr<Exp> e) : exp(e) {
    std::cout << "Not constructor" << std::endl;  
}

void NotExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IdentifierExp::IdentifierExp(std::string& s) : name(s) {
    std::cout << "Identifier expression constructor" << std::endl;
}

void IdentifierExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IntegerLiteral::IntegerLiteral(long val) : value(val) {
    std::cout << "IntegerLiteral constructor" << std::endl;
}

void IntegerLiteral::accept(Visitor* v) {
    v->visit(shared_from_this());
}

BoolLiteral::BoolLiteral(bool val) : value(val) {
    std::cout << "BoolLiteral constructor" << std::endl;
}

void BoolLiteral::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CharLiteral::CharLiteral(char val) : value(val) {
    std::cout << "CharLiteral constructor" << std::endl;
}

void CharLiteral::accept(Visitor* v) {
    v->visit(shared_from_this());
}

StringLiteral::StringLiteral(std::string& val) : value(val) {
    std::cout << "StringLiteral constructor" << std::endl;
}

void StringLiteral::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NulLiteral::NulLiteral() {
    std::cout << "NulLiteral constructor" << std::endl;
}

void NulLiteral::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ArrayAccessExp::ArrayAccessExp(std::shared_ptr<Exp> arr, std::shared_ptr<Exp> idx) : array(arr), index(idx) {
    std::cout << "ArrayAccess constructor" << std::endl;
}

void ArrayAccessExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ThisExp::ThisExp() {
    std::cout << "This constructor" << std::endl;
}

void ThisExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CastExp::CastExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t) : exp(e), type(t) {
    std::cout << "CastExpression constructor" << std::endl;
}

void CastExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IdentifierType::IdentifierType(std::string& s) : Type(DataType::OBJECT), name(s) {
    std::cout << "IdentifierType constructor" << std::endl;
}

void IdentifierType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ArrayType::ArrayType(std::shared_ptr<Type> t) : Type(DataType::ARRAY), dataType(t) {
    std::cout << "ArrayType constructor" << std::endl;
}

void ArrayType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

FieldAccessExp::FieldAccessExp(std::shared_ptr<Exp> e, std::string& s) : exp(e), field(s) {
    std::cout << "FieldAccessExp constructor" << std::endl;
}

void FieldAccessExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NewArrayExp::NewArrayExp(std::shared_ptr<Type> t, std::shared_ptr<Exp> e) : type(t), exp(e) {
    std::cout << "NewArrayExp constructor" << std::endl;
}

void NewArrayExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

// CompoundType::CompoundType(std::shared_ptr<Type> t1, std::shared_ptr<IdentifierType> t2) : Type(DataType::OBJECT), type(t1), subType(t2) {
//     std::cout << "CompoundType constructor" << std::endl;
// }

// void CompoundType::accept(Visitor* v) {
//     v->visit(shared_from_this());
// }

NegExp::NegExp(std::shared_ptr<Exp> e) : exp(e) {
    std::cout << "NegExp constructor" << std::endl;
}

void NegExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}
