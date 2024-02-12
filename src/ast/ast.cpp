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

void ResultType::accept(Visitor* v) {
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


MethodOrFieldRest::MethodOrFieldRest(MemberType m) : memberType(m) {
    std::cout << "MethodOrFieldRest constructor" << std::endl;
}

FieldDeclaratorsRest::FieldDeclaratorsRest(MemberType m) : MethodOrFieldRest(m), initializer(false) {
    std::cout << "FieldDeclaratorsRest declaration constructor" << std::endl;
}

FieldDeclaratorsRest::FieldDeclaratorsRest(MemberType m, std::shared_ptr<Exp> e) : MethodOrFieldRest(m), initializer(true), exp(e) {
    std::cout << "FieldDeclaratorsRest initialization constructor" << std::endl;
}

void FieldDeclaratorsRest::accept(Visitor* v) {
    if (initializer) exp->accept(v);
    v->visit(shared_from_this());
}


MethodDeclaratorRest::MethodDeclaratorRest(MemberType m, std::vector<std::pair<std::shared_ptr<Type>, std::shared_ptr<Identifier>>> fp) 
: MethodOrFieldRest(m), hasBlock(false), formalParameters(fp) {
    std::cout << "MethodDeclaratorRest (no block) constructor" << std::endl;
}

MethodDeclaratorRest::MethodDeclaratorRest(MemberType m, std::vector<std::pair<std::shared_ptr<Type>, std::shared_ptr<Identifier>>> fp,
std::shared_ptr<Block> b) : MethodOrFieldRest(m), hasBlock(true), formalParameters(fp), block(b)  {
    std::cout << "MethodDeclaratorRest (with block) constructor" << std::endl;
}

void MethodDeclaratorRest::accept(Visitor* v) {
    //depending on what we do with visitor, we might want these to be different
    for (auto fp : formalParameters) (fp.first)->accept(v); 
    if (hasBlock) {
        //block->accept(v); //To be uncommented once Block is implemented
    }
    v->visit(shared_from_this());
}

MemberDecl::MemberDecl(MemberType m) : memberType(m) {
    std::cout << "MemberDecl constructor" << std::endl;
}

MethodOrFieldDecl::MethodOrFieldDecl(std::shared_ptr<Type> rt, std::shared_ptr<Identifier> v, 
std::shared_ptr<MethodOrFieldRest> mofr) : MemberDecl(mofr->memberType), resultType(rt), variable(v), methodOrFieldRest(mofr) {
    std::cout << "MethodOrFieldDecl constructor" << std::endl;
}

void MethodOrFieldDecl::accept(Visitor* v) {
    resultType->accept(v);
    methodOrFieldRest->accept(v);
    v->visit(shared_from_this());
}

ConstructorDeclaratorRest::ConstructorDeclaratorRest(std::vector<std::pair<std::shared_ptr<Type>, std::shared_ptr<Identifier>>> fp, std::shared_ptr<Block> b) 
: formalParameters(fp), block(b) {
    std::cout << "ConstructorDeclaratorRest constructor" << std::endl;
}

void ConstructorDeclaratorRest::accept(Visitor* v) {
    for (auto fp : formalParameters) (fp.first)->accept(v);
    //block->accept(v); //To be uncommented once Block is implemented
}

ConstructorDecl::ConstructorDecl(std::shared_ptr<Identifier> v, std::shared_ptr<ConstructorDeclaratorRest> c, MemberType m) : MemberDecl(m), 
        variable(v), constructorDeclaratorRest(c) { 
    assert(m == MemberType::CONSTRUCTOR); 
    std::cout << "ConstructorDecl constructor" << std::endl;
}

void ConstructorDecl::accept(Visitor* v)  {
    constructorDeclaratorRest->accept(v);
    v->visit(shared_from_this());
}

ClassBodyDeclaration::ClassBodyDeclaration() : emptyClassBodyDeclaration(true) {
    std::cout << "Empty ClassBodyDeclaration constructor" << std::endl;
}

ClassBodyDeclaration::ClassBodyDeclaration(std::vector<Modifiers> m, std::shared_ptr<MemberDecl> md) : 
    emptyClassBodyDeclaration(false), modifiers(m), memberDecl(md) {
    std::cout << "ClassBodyDeclaration constructor" << std::endl;
}

void ClassBodyDeclaration::accept(Visitor* v) {
    if (!emptyClassBodyDeclaration) {
        memberDecl->accept(v);
    }
    v->visit(shared_from_this());
}

ClassBody::ClassBody(std::vector<std::shared_ptr<ClassBodyDeclaration>> cbd) : classBodyDeclarations(cbd) {
    std::cout << "ClassBody constructor" << std::endl;
}

void ClassBody::accept(Visitor* v) {
    for (auto cbd : classBodyDeclarations) cbd->accept(v);
    v->visit(shared_from_this());
}

TypeParameter::TypeParameter(std::shared_ptr<Identifier> tpn, std::vector<std::shared_ptr<Type>> e) 
: typeParameterName(tpn), extended(e) {
    std::cout << "TypeParameter constructor" << std::endl;
}

void TypeParameter::accept(Visitor* v) {
    for (auto e : extended) e->accept(v);
    v->visit(shared_from_this());
}

NormalClassDecl::NormalClassDecl(std::shared_ptr<Identifier> cn, std::vector<std::shared_ptr<TypeParameter>> tp, 
std::vector<std::shared_ptr<IdentifierType>> e, std::vector<std::shared_ptr<IdentifierType>> i, 
std::shared_ptr<ClassBody> cb) : className(cn), typeParams(tp), extended(e), implemented(i), classBody(cb) {
    std::cout << "NormalClassDecl constructor" << std::endl;
}

void NormalClassDecl::accept(Visitor* v) {
    for (auto i : typeParams) i->accept(v);
    for (auto i : extended) i->accept(v);
    for (auto i : implemented) i->accept(v);
    classBody->accept(v);
    v->visit(shared_from_this());
}

ClassDecl::ClassDecl(std::string m, std::shared_ptr<NormalClassDecl> n) : modifier(m), ncdecl(n) {
    std::cout << "ClassDecl constructor" << std::endl;
}

void ClassDecl::accept(Visitor* v) {
    ncdecl->accept(v);
    v->visit(shared_from_this());
}