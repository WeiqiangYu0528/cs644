#include "visitor.hpp"

void Visitor::visit(std::shared_ptr<PlusExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<MinusExp> n) { 
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<TimesExp> n) { 
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<DivideExp> n) { 
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<ModuloExp> n) { 
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<NotExp> n) { 
    n->exp->accept(this);
}

void Visitor::visit(std::shared_ptr<IdentifierExp> n) {
}

void Visitor::visit(std::shared_ptr<IntegerLiteralExp> n) { 
}

void Visitor::visit(std::shared_ptr<BoolLiteralExp> n) { 
}

void Visitor::visit(std::shared_ptr<CharLiteralExp> n) { 
}

void Visitor::visit(std::shared_ptr<StringLiteralExp> n) { 
}

void Visitor::visit(std::shared_ptr<NulLiteralExp> n) { 
}

void Visitor::visit(std::shared_ptr<ArrayAccessExp> n) { 
    n->array->accept(this);
    n->index->accept(this);
}

void Visitor::visit(std::shared_ptr<ThisExp> n) { 
}

void Visitor::visit(std::shared_ptr<CastExp> n) { 
    n->exp->accept(this);
    n->type->accept(this);
}

void Visitor::visit(std::shared_ptr<ArrayType> n) { 
    n->dataType->accept(this);
}

void Visitor::visit(std::shared_ptr<IdentifierType> n) { 
}

void Visitor::visit(std::shared_ptr<FieldAccessExp> n) { 
    n->exp->accept(this);
    n->field->accept(this);
}

void Visitor::visit(std::shared_ptr<NewArrayExp> n) {
    n->exp->accept(this);
    n->type->accept(this); 
}

void Visitor::visit(std::shared_ptr<NegExp> n) {
    n->exp->accept(this);
}

void Visitor::visit(std::shared_ptr<ParExp> n) {
    n->exp->accept(this);
}

void Visitor::visit(std::shared_ptr<LessExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<GreaterExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<LessEqualExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<GreaterEqualExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<InstanceOfExp> n) {
    n->exp->accept(this);
    n->type->accept(this);
}

void Visitor::visit(std::shared_ptr<EqualExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<NotEqualExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<AndExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<XorExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<OrExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    n->exp1->accept(this);
    n->exp2->accept(this);
}

void Visitor::visit(std::shared_ptr<Assignment> n) {
    n->left->accept(this);
    n->right->accept(this);
}

void Visitor::visit(std::shared_ptr<MethodInvocation> n) {
    if (n->primary != nullptr) {
        n->primary->accept(this);
        n->primaryMethodName->accept(this);
    }
    if (n->ambiguousName != nullptr) {
        n->ambiguousName->accept(this);
        n->ambiguousMethodName->accept(this);
    }
    for (auto& arg : n->arguments) {
        arg->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    n->classType->accept(this);

    for (auto& arg : n->arguments) {
        arg->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<IntType> n) { 
}

void Visitor::visit(std::shared_ptr<ShortType> n) { 
}

void Visitor::visit(std::shared_ptr<CharType> n) { 
}

void Visitor::visit(std::shared_ptr<BooleanType> n) { 
}

void Visitor::visit(std::shared_ptr<VoidType> n) { 
}

void Visitor::visit(std::shared_ptr<ByteType> n) {
}

void Visitor::visit(std::shared_ptr<Package> n) {
}

void Visitor::visit(std::shared_ptr<ImportStatement> n) {
    n->package->accept(this);
    n->id->accept(this);
}

void Visitor::visit(std::shared_ptr<ImportStatements> n) {
    for (auto stmt: n->stmts) {
        stmt->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<Identifier> n) { 
}

void Visitor::visit(std::shared_ptr<FormalParameter> n) {
    n->type->accept(this);
}

void Visitor::visit(std::shared_ptr<Constructor> n) {
    for (auto fp : n->formalParameters) fp->accept(this);
    if (n->block) n->block->accept(this);
}

void Visitor::visit(std::shared_ptr<Method> n) {
    n->type->accept(this);
    for (auto fp : n->formalParameters) fp->accept(this);
    if (n->block) n->block->accept(this);
}

void Visitor::visit(std::shared_ptr<Field> n) {
    n->type->accept(this);
    if (n->initializer) n->initializer->accept(this);
}

void Visitor::visit(std::shared_ptr<ClassDecl> n) {
    for (auto e : n->extended) e->accept(this);
    for (auto i : n->implemented) i->accept(this);
    for (int i = 0; i < 3; i++) {
        for (auto d : n->declarations[i]) d->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<InterfaceDecl> n) { 
    n->id->accept(this);
    for (auto& e : n->extended) {
        e->accept(this);
    }
    for (auto& m : n->methods) {
        m->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<Program> n) { 
    n->package->accept(this);
    n->importStatements->accept(this);
    if(n->classOrInterfaceDecl) n->classOrInterfaceDecl->accept(this);
}

void Visitor::visit(std::shared_ptr<BlockStatement> n)
{
    n->blockStatements->accept(this);
}

void Visitor::visit(std::shared_ptr<BlockStatements> n)
{
    for (auto stmt : n->statements)
    {
        if (stmt) stmt->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<IfStatement> n)
{
    n->exp->accept(this);
    n->statement1->accept(this);
    if (n->statement2) 
    {
        n->statement2->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<ForStatement> n)
{
    if(n->stmt1) 
    {
        n->stmt1->accept(this);
    }

    if (n->exp) 
    {
        n->exp->accept(this);
    }

    if (n->expStmt2) 
    {
        n->expStmt2->accept(this);
    }

    if (n->stmt2) 
    {
        n->stmt2->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<ReturnStatement> n)
{
    if (n->exp) 
    {
       n->exp->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<ExpressionStatement> n)
{
    if (n->exp) 
    {
        n->exp->accept(this);
    }
}

void Visitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n)
{
    n->type->accept(this);
    n->id->accept(this);
    if (n->exp) n->exp->accept(this);
}

void Visitor::visit(std::shared_ptr<WhileStatement> n)
{
    n->exp->accept(this);
    if (n->statement)
        n->statement->accept(this);
}

void Visitor::visit(std::shared_ptr<SemicolonStatement> n)
{
}
