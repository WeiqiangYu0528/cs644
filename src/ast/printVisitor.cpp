#include "printVisitor.hpp"

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

void PrintVisitor::visit(std::shared_ptr<LessExp> n) {
    std::cout << "Visiting LessExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<GreaterExp> n) {
    std::cout << "Visiting GreaterExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<LessEqualExp> n) {
    std::cout << "Visiting LessEqualExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<GreaterEqualExp> n) {
    std::cout << "Visiting GreaterEqualExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<InstanceOfExp> n) {
    std::cout << "Visiting InstanceOfExp" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<EqualExp> n) {
    std::cout << "Visiting Equal Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<NotEqualExp> n) {
    std::cout << "Visiting Not Equal Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<AndExp> n) {
    std::cout << "Visiting And Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<XorExp> n) {
    std::cout << "Visiting Xor Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<OrExp> n) {
    std::cout << "Visiting Or Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ConditionalAndExp> n) {
    std::cout << "Visiting Conditional And Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ConditionalOrExp> n) {
    std::cout << "Visiting Conditional Or Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Assignment> n) {
    std::cout << "Visiting Assignment Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<MethodInvocation> n) {
    std::cout << "Visiting MethodInvocation Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ClassInstanceCreationExp> n) {
    std::cout << "Visiting ClassInstanceCreation Expression" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<IntType> n) { 
    std::cout << "Visiting IntType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ShortType> n) { 
    std::cout << "Visiting ShortType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<CharType> n) { 
    std::cout << "Visiting CharType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<BooleanType> n) { 
    std::cout << "Visiting BooleanType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<VoidType> n) { 
    std::cout << "Visiting VoidType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ByteType> n) { 
    std::cout << "Visiting ByteType" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Package> n) { 
    std::cout << "Visiting Package" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ImportStatement> n) { 
    std::cout << "Visiting ImportStatement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Identifier> n) { 
    std::cout << n->name << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<FormalParameter> n) {
    std::cout << "Visiting FormalParameter" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Constructor> n) {
    std::cout << "Visiting Constructor" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Method> n) {
    std::cout << "Visiting Method" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Field> n) {
    std::cout << "Visiting Field" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ClassDecl> n) {
    std::cout << "Visiting ClassDecl" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<InterfaceDecl> n) { 
    std::cout << "Visiting InterfaceDecl" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<Program> n) { 
    std::cout << "Visiting Program" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<BlockStatement> n)
{
    std::cout << "Visiting Block Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<BlockStatements> n)
{
    std::cout << "Visiting Block Statements" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<IfStatement> n)
{
    std::cout << "Visiting If Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ForStatement> n)
{
    std::cout << "Visiting For Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ReturnStatement> n)
{
    std::cout << "Visiting Return Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<ExpressionStatement> n)
{
    std::cout << "Visiting Expression Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<LocalVariableDeclarationStatement> n)
{
    std::cout << "Visiting Local Variable Declaration Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<WhileStatement> n)
{
    std::cout << "Visiting While Statement" << std::endl;
}

void PrintVisitor::visit(std::shared_ptr<SemicolonStatement> n)
{
    std::cout << "Visiting Semicolon Statement" << std::endl;
}
