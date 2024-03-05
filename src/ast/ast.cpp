#include "ast.hpp"
#include "scope.hpp"
#include "visitor.hpp"

void Ast::setAst(std::shared_ptr<Program> p) {
    program = p;
}

std::shared_ptr<Program> Ast::getAst() const {
    return program;
}

Type::Type(DataType t) : type(t) {
}

PlusExp::PlusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void PlusExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

MinusExp::MinusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void MinusExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

TimesExp::TimesExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void TimesExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

DivideExp::DivideExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void DivideExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ModuloExp::ModuloExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void ModuloExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NotExp::NotExp(std::shared_ptr<Exp> e) : exp(e) {
}

void NotExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IdentifierExp::IdentifierExp(std::shared_ptr<Identifier> i, bool s) : id(i), simple(s) {
}

void IdentifierExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IntegerLiteralExp::IntegerLiteralExp(long val) : value(val) {
}

void IntegerLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

BoolLiteralExp::BoolLiteralExp(bool val) : value(val) {
}

void BoolLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CharLiteralExp::CharLiteralExp(char val) : value(val) {
}

void CharLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

StringLiteralExp::StringLiteralExp(std::string& val) : value(val) {
}

void StringLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NulLiteralExp::NulLiteralExp() {
}

void NulLiteralExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ArrayAccessExp::ArrayAccessExp(std::shared_ptr<Exp> arr, std::shared_ptr<Exp> idx) : array(arr), index(idx) {
}

void ArrayAccessExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ThisExp::ThisExp() {
}

void ThisExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CastExp::CastExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t) : exp(e), type(t) {
}

void CastExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IdentifierType::IdentifierType(std::shared_ptr<Identifier> i, bool s) : Type(DataType::OBJECT), id(i), simple(s) {
}

void IdentifierType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ArrayType::ArrayType(std::shared_ptr<Type> t) : Type(DataType::ARRAY), dataType(t) {
}

void ArrayType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

FieldAccessExp::FieldAccessExp(std::shared_ptr<Exp> e, std::shared_ptr<IdentifierExp> f) : exp(e), field(f) {
}

void FieldAccessExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NewArrayExp::NewArrayExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t) : exp(e), type(t) {
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
}

void NegExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ParExp::ParExp(std::shared_ptr<Exp> e) : exp(e) {
}

void ParExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

Identifier::Identifier(std::string& s) : name(s) {
}

void Identifier::accept(Visitor* v) {
    v->visit(shared_from_this());
}

void LessExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

LessExp::LessExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}


void GreaterExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

GreaterExp::GreaterExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}


void LessEqualExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

LessEqualExp::LessEqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void GreaterEqualExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

GreaterEqualExp::GreaterEqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void InstanceOfExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

InstanceOfExp::InstanceOfExp(std::shared_ptr<Exp> exp, std::shared_ptr<Type> type) : exp(exp), type(type) {
}

void EqualExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

EqualExp::EqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void NotEqualExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

NotEqualExp::NotEqualExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void AndExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

AndExp::AndExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void XorExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

XorExp::XorExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void OrExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

OrExp::OrExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
    std::cout << "OrExp constructor" << std::endl;
}

void ConditionalAndExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ConditionalAndExp::ConditionalAndExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void ConditionalOrExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ConditionalOrExp::ConditionalOrExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2) : exp1(e1), exp2(e2) {
}

void Assignment::accept(Visitor* v) {
    v->visit(shared_from_this());
}

Assignment::Assignment(std::shared_ptr<Exp> l, std::shared_ptr<Exp> r) : left(l), right(r) {
}

void MethodInvocation::accept(Visitor* v) {
    v->visit(shared_from_this());
}

MethodInvocation::MethodInvocation(std::shared_ptr<Exp> primary, 
        std::shared_ptr<Identifier> primaryMethodName,
        std::shared_ptr<IdentifierType> methodName, 
        std::vector<std::shared_ptr<Exp>> arguments)
        : primary(primary), primaryMethodName(primaryMethodName), methodName(methodName), arguments(arguments) {
}

void ClassInstanceCreationExp::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ClassInstanceCreationExp::ClassInstanceCreationExp(std::shared_ptr<IdentifierType> classType, std::vector<std::shared_ptr<Exp>> arguments)
    : classType(classType), arguments(arguments)
{
}

void IntType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

IntType::IntType() : Type(DataType::INT) {
}

void CharType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

CharType::CharType() : Type(DataType::CHAR) {
}

void ShortType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ShortType::ShortType() : Type(DataType::SHORT) {
}

void VoidType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

VoidType::VoidType() : Type(DataType::VOID) {
}

void BooleanType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

BooleanType::BooleanType() : Type(DataType::BOOLEAN) {
}

void ByteType::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ByteType::ByteType() : Type(DataType::BYTE) {
}

Package::Package(std::shared_ptr<Identifier> i) : id(i) {
}

void Package::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ImportStatement::ImportStatement(std::shared_ptr<Identifier> p, std::shared_ptr<Identifier> i) : package(p), id(i) {
}

void ImportStatement::accept(Visitor* v) {
    v->visit(shared_from_this());
}

void ImportStatements::addImport(std::shared_ptr<ImportStatement> i) {
    stmts.push_back(i);
}

void ImportStatements::accept(Visitor* v) {
    v->visit(shared_from_this());
}

FormalParameter::FormalParameter(std::shared_ptr<Type> t, std::shared_ptr<Identifier> vn) : type(t), variableName(vn) {
}

void FormalParameter::accept(Visitor* v) {
    v->visit(shared_from_this());
}

MemberDecl::MemberDecl(MemberType mt, std::vector<Modifiers> m) : memberType(mt), modifiers(m) {
}

Field::Field(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> t, std::shared_ptr<Identifier> fn, std::shared_ptr<Exp> i)
: MemberDecl(mt, m), isStatic(false), type(t), fieldName(fn), initializer(i) {
}

void Field::accept(Visitor* v) {
    v->visit(shared_from_this());
}

void Field::setModifiers(std::vector<Modifiers>& m) {
    modifiers = m;
    for (Modifiers modifier : m) {
        if (modifier == Modifiers::STATIC) {
            isStatic = true;
            break;
        }
    }
}

Method::Method(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> t, std::shared_ptr<Identifier> mn, 
std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<BlockStatement> b)
: MemberDecl(mt, m), isStatic(false), type(t), methodName(mn), formalParameters(fp), block(b) {
}

void Method::accept(Visitor* v) {
    v->visit(shared_from_this());
}

void Method::setModifiers(std::vector<Modifiers>& m) {
    modifiers = m;
    for (Modifiers modifier : m) {
        if (modifier == Modifiers::STATIC) {
            isStatic = true;
        }
        if (modifier == Modifiers::ABSTRACT)
        {
            isAbstract = true;
        }
        if (modifier == Modifiers::FINAL)
        {
            isFinal = true;
        }
        if (modifier == Modifiers::PUBLIC)
        {
            isPublic = true;
        }
    }
}

Constructor::Constructor(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Identifier> cn, 
std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<BlockStatement> b)
: MemberDecl(mt, m), constructorName(cn), formalParameters(fp), block(b) {
}

void Constructor::accept(Visitor* v) {
    v->visit(shared_from_this());
}

void Constructor::setModifiers(std::vector<Modifiers>& m) {
    modifiers = m;
}

ClassDecl::ClassDecl(std::string m, std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<IdentifierType>> e, 
std::vector<std::shared_ptr<IdentifierType>> i, std::vector<std::vector<std::shared_ptr<MemberDecl>>> d) :
ClassOrInterfaceDecl(cn, e), modifier(m), implemented(i), declarations(d) {
}

void ClassDecl::accept(Visitor* v) {
    v->visit(shared_from_this());
}

InterfaceDecl::InterfaceDecl(std::shared_ptr<Identifier> in, std::vector<std::shared_ptr<IdentifierType>> e,
        std::vector<std::shared_ptr<Method>>& m) : ClassOrInterfaceDecl(in, e), methods(m) {
}

void InterfaceDecl::accept(Visitor* v) {
    v->visit(shared_from_this());
}

ClassOrInterfaceDecl::ClassOrInterfaceDecl(std::shared_ptr<Identifier> i, std::vector<std::shared_ptr<IdentifierType>> e) : id(i), extended(e) {
}

BlockStatement::BlockStatement(std::shared_ptr<BlockStatements> sl) : blockStatements(sl)
{
}

void BlockStatement::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

IfStatement::IfStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement> s1, std::shared_ptr<Statement> s2) : exp(e), statement1(s1), statement2(s2)
{
}

void IfStatement::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

WhileStatement::WhileStatement(std::shared_ptr<Exp> e, std::shared_ptr<Statement> s) : exp(e), statement(s)
{
}

void WhileStatement::accept(Visitor *v)
{
    v->visit(shared_from_this());
}

// Constructor for ExpressionStatement initialization
ForStatement::ForStatement(std::shared_ptr<Statement> s1, std::shared_ptr<Exp> e, std::shared_ptr<Exp> es2)
        : stmt1(s1), exp(e), expStmt2(es2) 
        {
        }

void ForStatement::accept(Visitor *v) 
{
    v->visit(shared_from_this());
}

ReturnStatement::ReturnStatement(std::shared_ptr<Exp> e) : exp(e)
{
}

void ReturnStatement::accept(Visitor *v) 
{
    v->visit(shared_from_this());
}

ExpressionStatement::ExpressionStatement(std::shared_ptr<Exp> e) : exp(e)
{
}

void ExpressionStatement::accept(Visitor *v) 
{
    v->visit(shared_from_this());
}

LocalVariableDeclarationStatement::LocalVariableDeclarationStatement(std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e)
            :id(i), exp(e)
{
}

void LocalVariableDeclarationStatement::accept(Visitor *v)
{
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
    v->visit(shared_from_this());
}

Program::Program(std::shared_ptr<Package> p, std::shared_ptr<ImportStatements> i, std::shared_ptr<ClassOrInterfaceDecl> c) : package(p), importStatements(i), classOrInterfaceDecl(c) {
}

void Program::accept(Visitor* v) {
    v->visit(shared_from_this());
}

std::pair<std::string, std::string> Program::getQualifiedName() const {
    std::string packageName = package->id ? package->id->name : "";
    std::string className = classOrInterfaceDecl ? classOrInterfaceDecl->id->name : "";
    return {packageName, className};
}
