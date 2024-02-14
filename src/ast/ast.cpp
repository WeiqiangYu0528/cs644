#include "ast.hpp"
#include "visitor.hpp"

void Ast::setAst(std::shared_ptr<Statement> e) {
    exp = e;
}

std::shared_ptr<Statement> Ast::getAst() const {
    std::cout << "test" << std::endl;
    return exp;
}

void Exp::accept(Visitor* v) {
    // may need to make this pure virtual later
}

Type::Type(DataType t) : type(t) {
    std::cout << "Type constructor" << std::endl;
}

void Type::accept(Visitor* v) {
    // may need to make this pure virtual later
}

PlusExp::PlusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "PlusExp constructor" << std::endl;
}

void PlusExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

MinusExp::MinusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Minus constructor" << std::endl;
}

void MinusExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

TimesExp::TimesExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Times constructor" << std::endl;
}

void TimesExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

DivideExp::DivideExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Divide constructor" << std::endl;
}

void DivideExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

ModuloExp::ModuloExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "Modulo constructor" << std::endl;
}

void ModuloExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

NotExp::NotExp(std::shared_ptr<Exp> e) : exp(e) {
    std::cout << "Not constructor" << std::endl;  
}

void NotExp::accept(Visitor* v) {
    exp->accept(v);
    v->visit(shared_from_this());
}

IdentifierExp::IdentifierExp(std::shared_ptr<Identifier> i) : id(i) {
    std::cout << id->name << std::endl;
}

void IdentifierExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IntegerLiteralExp::IntegerLiteralExp(long val) : value(val) {
    std::cout << "IntegerLiteral constructor" << std::endl;
}

void IntegerLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

BoolLiteralExp::BoolLiteralExp(bool val) : value(val) {
    std::cout << "BoolLiteral constructor" << std::endl;
}

void BoolLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CharLiteralExp::CharLiteralExp(char val) : value(val) {
    std::cout << "CharLiteral constructor" << std::endl;
}

void CharLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

StringLiteralExp::StringLiteralExp(std::string& val) : value(val) {
    std::cout << "StringLiteral constructor" << std::endl;
}

void StringLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NulLiteralExp::NulLiteralExp() {
    std::cout << "NulLiteral constructor" << std::endl;
}

void NulLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ArrayAccessExp::ArrayAccessExp(std::shared_ptr<Exp> arr, std::shared_ptr<Exp> idx) : array(arr), index(idx) {
    std::cout << "ArrayAccess constructor" << std::endl;
}

void ArrayAccessExp::accept(Visitor* v) {
    array->accept(v);
    index->accept(v);
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
    exp->accept(v);
    type->accept(v);
    v->visit(shared_from_this());
}

IdentifierType::IdentifierType(std::shared_ptr<Identifier> i) : Type(DataType::OBJECT), id(i) {
    std::cout << id->name << std::endl;
}

void IdentifierType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ArrayType::ArrayType(std::shared_ptr<Type> t) : Type(DataType::ARRAY), dataType(t) {
    std::cout << "ArrayType constructor" << std::endl;
}

void ArrayType::accept(Visitor* v) {
    dataType->accept(v);
    v->visit(shared_from_this());
}

FieldAccessExp::FieldAccessExp(std::shared_ptr<Exp> e, std::shared_ptr<Identifier> i) : exp(e), id(i) {
    std::cout << "FieldAccessExp constructor" << std::endl;
}

void FieldAccessExp::accept(Visitor* v) {
    exp->accept(v);
    v->visit(shared_from_this());
}

NewArrayExp::NewArrayExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t) : exp(e), type(t) {
    std::cout << "NewArrayExp constructor" << std::endl;
}

void NewArrayExp::accept(Visitor* v) {
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

NegExp::NegExp(std::shared_ptr<Exp> e) : exp(e) {
    std::cout << "NegExp constructor" << std::endl;
}

void NegExp::accept(Visitor* v) {
    exp->accept(v);
    v->visit(shared_from_this());
}

ParExp::ParExp(std::shared_ptr<Exp> e) : exp(e) {
    std::cout << "ParExp constructor" << std::endl;
}

void ParExp::accept(Visitor* v) {
    exp->accept(v);
    v->visit(shared_from_this());
}

Identifier::Identifier(std::string& s) : name(s) {
    std::cout << name << std::endl;
}

void Identifier::accept(Visitor* v) {
    v->visit(shared_from_this());
}

void LessExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

LessExp::LessExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "LessExp constructor" << std::endl;
}


void GreaterExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

GreaterExp::GreaterExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "GreaterExp constructor" << std::endl;
}


void LessEqualExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

LessEqualExp::LessEqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "LessEqualExp constructor" << std::endl;
}


void GreaterEqualExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}

GreaterEqualExp::GreaterEqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "GreaterEqual constructor" << std::endl;
}

void InstanceOfExp::accept(Visitor* v) {
    exp->accept(v);
    type->accept(v);
    v->visit(shared_from_this());
}

InstanceOfExp::InstanceOfExp(std::shared_ptr<Exp> exp, std::shared_ptr<Type> type) : exp(exp), type(type) {
    std::cout << "InstanceOfExp constructor" << std::endl;
}

void EqualExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
EqualExp::EqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "EqualExp constructor" << std::endl;
}

void NotEqualExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
NotEqualExp::NotEqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "NotEqualExp constructor" << std::endl;
}

void AndExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
AndExp::AndExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "AndExp constructor" << std::endl;
}

void XorExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
XorExp::XorExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "XorExp constructor" << std::endl;
}

void OrExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
OrExp::OrExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "OrExp constructor" << std::endl;
}


void ConditionalAndExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
ConditionalAndExp::ConditionalAndExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "ConditionalAndExp constructor" << std::endl;
}

void ConditionalOrExp::accept(Visitor* v) {
    exp1->accept(v);
    exp2->accept(v);
    v->visit(shared_from_this());
}
ConditionalOrExp::ConditionalOrExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "ConditionalOrExp constructor" << std::endl;
}


void Assignment::accept(Visitor* v) {
    left->accept(v);
    right->accept(v);
    v->visit(shared_from_this());
}
Assignment::Assignment(std::shared_ptr<Exp> l, std::shared_ptr<Exp> r) : left(l), right(r) {
    std::cout << "Assignment constructor" << std::endl;
}


void MethodInvocation::accept(Visitor* v) {
    if (primary != nullptr)
        primary->accept(v);

    if(methodName != nullptr)
        methodName->accept(v);

    for (auto& arg : arguments) {
        arg->accept(v);
    }
    v->visit(shared_from_this());
}
MethodInvocation::MethodInvocation(std::shared_ptr<Exp> primary, 
        std::shared_ptr<Identifier> primaryMethodName,
        std::shared_ptr<IdentifierType> methodName, 
        std::vector<std::shared_ptr<Exp>> arguments)
        : primary(primary), primaryMethodName(primaryMethodName), methodName(methodName), arguments(arguments) {
        std::cout << "MethodInvocation constructor" << std::endl;
}

void ClassInstanceCreationExp::accept(Visitor* v) {
    classType->accept(v);

    for (auto& arg : arguments) {
        arg->accept(v);
    }

    v->visit(shared_from_this());
}

ClassInstanceCreationExp::ClassInstanceCreationExp(std::shared_ptr<IdentifierType> classType, std::vector<std::shared_ptr<Exp>> arguments)
    : classType(classType), arguments(arguments)
{
    std::cout << "ClassInstanceCreationExpression constructor" << std::endl;
}

void IntType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IntType::IntType() : Type(DataType::INT) {
    std::cout << "IntType constructor" << std::endl;
}

void CharType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CharType::CharType() : Type(DataType::CHAR) {
    std::cout << "CharType constructor" << std::endl;
}

void ShortType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ShortType::ShortType() : Type(DataType::SHORT) {
    std::cout << "ShortType constructor" << std::endl;
}

void VoidType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

VoidType::VoidType() : Type(DataType::VOID) {
    std::cout << "VoidType constructor" << std::endl;
}

void BooleanType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

BooleanType::BooleanType() : Type(DataType::BOOLEAN) {
    std::cout << "BooleanType constructor" << std::endl;
}

void ByteType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ByteType::ByteType() : Type(DataType::BYTE) {
    std::cout << "ByteType constructor" << std::endl;
}

Package::Package(std::shared_ptr<Identifier> i) : id(i) {
    std::cout << "Package constructor" << std::endl;
}

void Package::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ImportStatement::ImportStatement() {
    std::cout << "ImportStatement constructor" << std::endl;
}

void ImportStatement::addImport(std::shared_ptr<Identifier> i) {
    stmts.push_back(i);
}

void ImportStatement::accept(Visitor* v) {
    for (auto stmt: stmts) {
        stmt->accept(v);
    }
    v->visit(shared_from_this());
}

BlockStatement::BlockStatement(std::shared_ptr<BlockStatements> sl) : blockStatements(sl)
{
    std::cout << "Block Statement constructor" << std::endl;
}

void BlockStatement::accept(Visitor *v)
{
    blockStatements->accept(v);
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
    if (statement2) 
    {
        statement2->accept(v);
    }
    v->visit(shared_from_this());
}

WhileStatement::WhileStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement> s) : exp(e), statement(s)
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
ForStatement::ForStatement(std::shared_ptr<Statement> s1, std::shared_ptr<Exp> e, std::shared_ptr<Exp> es2)
        : stmt1(s1), exp(e), expStmt2(es2) 
        {
            std::cout << "For Statement constructor" << std::endl;
        }

void ForStatement::accept(Visitor *v) 
{
    if(stmt1) 
    {
        stmt1->accept(v);
    }

    if (exp) 
    {
        exp->accept(v);
    }

    if (expStmt2) 
    {
        expStmt2->accept(v);
    }

    if (stmt2) 
    {
        stmt2->accept(v);
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

LocalVariableDeclarationStatement::LocalVariableDeclarationStatement(std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e)
            :id(i), exp(e)
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

void SemicolonStatement::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

void BlockStatements::addStatement(std::shared_ptr<Statement> s)
{
    statements.push_back(s);
}

void BlockStatements::accept(Visitor *v)
{
    for (auto stmt : statements)
    {
        stmt->accept(v);
    }
    v->visit(shared_from_this());
}
