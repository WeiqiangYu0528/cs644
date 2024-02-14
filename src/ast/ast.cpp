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

IfStatement::IfStatement(std::shared_ptr<ParExp> e, std::shared_ptr<Statement> s1, std::shared_ptr<Statement> s2) : parExp(e), statement1(s1), statement2(s2)
{
    std::cout << "If Statement constructor" << std::endl;
}

void IfStatement::accept(Visitor *v)
{
    parExp->accept(v);
    statement1->accept(v);
    if (statement2) 
    {
        statement2->accept(v);
    }
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

// Constructor for ExpressionStatement initialization
ForStatement::ForStatement(std::shared_ptr<ExpressionStatement> es1, std::shared_ptr<Exp> e, std::shared_ptr<ExpressionStatement> es2, std::shared_ptr<Statement> s)
        : forInit(es1), exp(e), expStmt2(es2), stmt(s) 
        {
            std::cout << "For Statement constructor" << std::endl;
        }

// Constructor for LocalVariableDeclarationStatement initialization
ForStatement::ForStatement(std::shared_ptr<LocalVariableDeclarationStatement> lvds, std::shared_ptr<Exp> e, std::shared_ptr<ExpressionStatement> es2, std::shared_ptr<Statement> s)
        : forInit(lvds), exp(e), expStmt2(es2), stmt(s) 
        {
           std::cout << "For Statement constructor" << std::endl;
        }

void ForStatement::accept(Visitor *v) 
{
    std::visit([&v](auto&& arg) 
    {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<ExpressionStatement>> || std::is_same_v<T, std::shared_ptr<LocalVariableDeclarationStatement>>) 
        {
            arg->accept(v);
        }
    }, forInit);

    if (exp) 
    {
        exp->accept(v);
    }

    if (expStmt2) 
    {
        expStmt2->accept(v);
    }

    if (stmt) 
    {
        stmt->accept(v);
    }
    v->visit(shared_from_this());
}

ReturnStatement::ReturnStatement(std::shared_ptr<Exp> e) : exp(e)
{
    std::cout << "Return Statement constructor" << std::endl;
}

void ReturnStatement::accept(Visitor *v) 
{
    if (exp) 
    {
        exp->accept(v);
    }
    v->visit(shared_from_this());
}

ExpressionStatement::ExpressionStatement(std::shared_ptr<Exp> e) : exp(e)
{
    std::cout << "Expression Statement constructor" << std::endl;
}

void ExpressionStatement::accept(Visitor *v) 
{
    if (exp) 
    {
        exp->accept(v);
    }
    v->visit(shared_from_this());
}

LocalVariableDeclarationStatement::LocalVariableDeclarationStatement(std::shared_ptr<Type> t, std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e)
            : type(t), id(i), exp(e)
{
    std::cout << "LocalVariableDeclaration Statement constructor" << std::endl;
}

void LocalVariableDeclarationStatement::accept(Visitor *v)
{
    type->accept(v);
    id->accept(v);
    exp->accept(v);
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
