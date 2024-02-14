#include "ast.hpp"
#include "visitor.hpp"

/*void Ast::setAst(std::shared_ptr<Exp> e) {
    exp = e;
}

std::shared_ptr<Exp> Ast::getAst() const {
    return exp;
}*/

void Ast::setAst(std::shared_ptr<ClassDecl> cd) {
    cdecl = cd;
}

std::shared_ptr<ClassDecl> Ast::getAst() const {
    return cdecl;
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
    std::cout << "Identifier expression constructor" << std::endl;
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
    std::cout << "IdentifierType constructor" << std::endl;
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
    std::cout << "Identifier constructor" << std::endl;
}

FormalParameter::FormalParameter(std::shared_ptr<Type> t, std::shared_ptr<Identifier> vn) : type(t), variableName(vn) {
    std::cout << "FormalParameter constructor" << std::endl;
}

void FormalParameter::accept(Visitor* v) {
    type->accept(v);
    v->visit(shared_from_this());
}

MemberDecl::MemberDecl(MemberType mt, std::vector<Modifiers> m) : memberType(mt), modifiers(m) {
    std::cout << "MemberDecl constructor" << std::endl;
}

Field::Field(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> fn, std::shared_ptr<Exp> i)
: MemberDecl(mt, m), returnType(rt), fieldName(fn), initializer(i) {
    std::cout << "Field constructor" << std::endl;
}

void Field::accept(Visitor* v) {
    returnType->accept(v);
    if (initializer) initializer->accept(v);
    v->visit(shared_from_this());
}

Method::Method(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Type> rt, std::shared_ptr<Identifier> mn, 
std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<Block> b)
: MemberDecl(mt, m), returnType(rt), methodName(mn), formalParameters(fp), block(b) {
    std::cout << "Method constructor" << std::endl;
}

void Method::accept(Visitor* v) {
    returnType->accept(v);
    for (auto fp : formalParameters) fp->accept(v);
    if (block); //block->accept(v); //Uncomment this once Block is implemented
    v->visit(shared_from_this());
}

Constructor::Constructor(MemberType mt, std::vector<Modifiers> m, std::shared_ptr<Identifier> cn, 
std::vector<std::shared_ptr<FormalParameter>> fp, std::shared_ptr<Block> b)
: MemberDecl(mt, m), constructorName(cn), formalParameters(fp), block(b) {
    std::cout << "Constructor constructor" << std::endl;
}

void Constructor::accept(Visitor* v) {
    for (auto fp : formalParameters) fp->accept(v);
    //block->accept(v); //Uncomment this once Block is implemented
    v->visit(shared_from_this());
}

ClassDecl::ClassDecl(std::string m, std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<IdentifierType>> e, 
std::vector<std::shared_ptr<IdentifierType>> i, std::vector<std::vector<std::shared_ptr<MemberDecl>>> d) :
modifier(m), className(cn), extended(e), implemented(i), declarations(d) {
    std::cout << "ClassDecl constructor" << std::endl;
}

void ClassDecl::accept(Visitor* v) {
    for (auto e : extended) e->accept(v);
    for (auto i : implemented) i->accept(v);
    for (int i = 0; i < 3; i++) {
        for (auto d : declarations[i]) d->accept(v);
    }
    v->visit(shared_from_this());
}