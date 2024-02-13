#include "ast.hpp"
#include "visitor.hpp"

void Ast::setAst(std::shared_ptr<Exp> e)
{
    exp = e;
}

std::shared_ptr<Exp> Ast::getAst() const
{
    return exp;
}

void Exp::accept(Visitor *v)
{
    // may need to make this pure virtual later
}

Type::Type(DataType t) : type(t)
{
    std::cout << "Type constructor" << std::endl;
}

void Type::accept(Visitor *v)
{
    // may need to make this pure virtual later
}

BlockStatement::BlockStatement(std::shared_ptr<StatementList> sl) : statementList(sl)
{
    std::cout << "Block Statement constructor" << std::endl;
}

void BlockStatement::accept(Visitor *v)
{
    for (auto stmt : statementList->statements)
    {
        stmt->accept(v);
    }
    v->visit(shared_from_this());
}

IfStatement::IfStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement> s1, std::shared_ptr<Statement> s2) : exp(e), statement1(s1), statement2(s2)
{
    std::cout << "If Statement constructor" << std::endl;
}

void IfStatement::accept(Visitor *v)
{
    exp->accept(v);
    statement1->accept(v);
    statement2->accept(v);
    v->visit(shared_from_this());
}

WhileStatement::WhileStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement>) : exp(e), statement(s)
{
    std::cout << "While Statement constructor" << std::endl;
}

void WhileStatement::accept(Visitor *v)
{
    exp->accept(v);
    statement->accept(v);
    v->visit(shared_from_this());
}

// PrintStatement::PrintStatement(std::shared_ptr<Exp> e) : exp(e)
// {
//     std::cout << "Print Statement constructor" << std::endl;
// }

// void PrintStatement::accept(Visitor *v)
// {
// }

AssignStatement::AssignStatement(std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e) : id(i), exp(e)
{
    std::cout << "Assign Statement constructor" << std::endl;
}

void AssignStatement::accept(Visitor *v)
{
    id->accept(v);
    exp->accept(v);
    v->visit(shared_from_this());
}

ArrayAssignStatement::ArrayAssignStatement(std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : id(i), exp1(e1), exp2(e2)
{
    std::cout << "ArrayAssign Statement constructor" << std::endl;
}

void ArrayAssignStatement::accept(Visitor *v)
{
    id->accept(v);
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

PlusExp::PlusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2)
{
    std::cout << "PlusExp constructor" << std::endl;
}

void PlusExp::accept(Visitor *v)
{
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

MinusExp::MinusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2)
{
    std::cout << "Minus constructor" << std::endl;
}

void MinusExp::accept(Visitor *v)
{
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

TimesExp::TimesExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2)
{
    std::cout << "Times constructor" << std::endl;
}

void TimesExp::accept(Visitor *v)
{
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

DivideExp::DivideExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2)
{
    std::cout << "Divide constructor" << std::endl;
}

void DivideExp::accept(Visitor *v)
{
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

ModuloExp::ModuloExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2)
{
    std::cout << "Modulo constructor" << std::endl;
}

void ModuloExp::accept(Visitor *v)
{
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

NotExp::NotExp(std::shared_ptr<Exp> e) : exp(e)
{
    std::cout << "Not constructor" << std::endl;
}

void NotExp::accept(Visitor *v)
{
    exp->accept(v);
    v->visit(shared_from_this());
}

IdentifierExp::IdentifierExp(std::shared_ptr<Identifier> i) : id(i)
{
    std::cout << "Identifier expression constructor" << std::endl;
}

void IdentifierExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

IntegerLiteralExp::IntegerLiteralExp(long val) : value(val)
{
    std::cout << "IntegerLiteral constructor" << std::endl;
}

void IntegerLiteralExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

BoolLiteralExp::BoolLiteralExp(bool val) : value(val)
{
    std::cout << "BoolLiteral constructor" << std::endl;
}

void BoolLiteralExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

CharLiteralExp::CharLiteralExp(char val) : value(val)
{
    std::cout << "CharLiteral constructor" << std::endl;
}

void CharLiteralExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

StringLiteralExp::StringLiteralExp(std::string &val) : value(val)
{
    std::cout << "StringLiteral constructor" << std::endl;
}

void StringLiteralExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

NulLiteralExp::NulLiteralExp()
{
    std::cout << "NulLiteral constructor" << std::endl;
}

void NulLiteralExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

ArrayAccessExp::ArrayAccessExp(std::shared_ptr<Exp> arr, std::shared_ptr<Exp> idx) : array(arr), index(idx)
{
    std::cout << "ArrayAccess constructor" << std::endl;
}

void ArrayAccessExp::accept(Visitor *v)
{
    array->accept(v);
    index->accept(v);
    v->visit(shared_from_this());
}

ThisExp::ThisExp()
{
    std::cout << "This constructor" << std::endl;
}

void ThisExp::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

CastExp::CastExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t) : exp(e), type(t)
{
    std::cout << "CastExpression constructor" << std::endl;
}

void CastExp::accept(Visitor *v)
{
    exp->accept(v);
    type->accept(v);
    v->visit(shared_from_this());
}

IdentifierType::IdentifierType(std::shared_ptr<Identifier> i) : Type(DataType::OBJECT), id(i)
{
    std::cout << "IdentifierType constructor" << std::endl;
}

void IdentifierType::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

ArrayType::ArrayType(std::shared_ptr<Type> t) : Type(DataType::ARRAY), dataType(t)
{
    std::cout << "ArrayType constructor" << std::endl;
}

void ArrayType::accept(Visitor *v)
{
    dataType->accept(v);
    v->visit(shared_from_this());
}

FieldAccessExp::FieldAccessExp(std::shared_ptr<Exp> e, std::shared_ptr<Identifier> i) : exp(e), id(i)
{
    std::cout << "FieldAccessExp constructor" << std::endl;
}

void FieldAccessExp::accept(Visitor *v)
{
    exp->accept(v);
    v->visit(shared_from_this());
}

NewArrayExp::NewArrayExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t) : exp(e), type(t)
{
    std::cout << "NewArrayExp constructor" << std::endl;
}

void NewArrayExp::accept(Visitor *v)
{
    exp->accept(v);
    type->accept(v);
    v->visit(shared_from_this());
}

// CompoundType::CompoundType(std::shared_ptr<Type> t1, std::shared_ptr<IdentifierType> t2) : Type(DataType::OBJECT), type(t1), subType(t2) {
//     std::cout << "CompoundType constructor" << std::endl;
// }

// void CompoundType::accept(Visitor* v) {
//     v->visit(shared_from_this());
// }

NegExp::NegExp(std::shared_ptr<Exp> e) : exp(e)
{
    std::cout << "NegExp constructor" << std::endl;
}

void NegExp::accept(Visitor *v)
{
    exp->accept(v);
    v->visit(shared_from_this());
}

ParExp::ParExp(std::shared_ptr<Exp> e) : exp(e)
{
    std::cout << "ParExp constructor" << std::endl;
}

void ParExp::accept(Visitor *v)
{
    exp->accept(v);
    v->visit(shared_from_this());
}

Identifier::Identifier(std::string &s) : name(s)
{
    std::cout << "Identifier constructor" << std::endl;
}
