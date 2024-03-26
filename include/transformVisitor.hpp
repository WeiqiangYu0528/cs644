// #pragma once
// #include <memory>
// #include "ast.hpp"
// #include "NodeFactory_c.hpp"

// class TransformVisitor {
//     private:
//         std::shared_ptr<NodeFactory> nodeFactory;
//         int labelCounter;
//     public:
//         TransformVisitor(std::shared_ptr<NodeFactory> nf);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<PlusExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<MinusExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<TimesExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<DivideExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<ModuloExp> n);
//         // void visit(std::shared_ptr<NotExp> n);
//         void visit(std::shared_ptr<IdentifierExp> n);
//         std::shared_ptr<Const> visit(std::shared_ptr<IntegerLiteralExp> n);
//         std::shared_ptr<Const> visit(std::shared_ptr<BoolLiteralExp> n);
//         std::shared_ptr<Const> visit(std::shared_ptr<CharLiteralExp> n);
//         // void visit(std::shared_ptr<StringLiteralExp> n);
//         // void visit(std::shared_ptr<NulLiteralExp> n);
//         void visit(std::shared_ptr<ArrayAccessExp> n);
//         // void visit(std::shared_ptr<ThisExp> n);
//         // void visit(std::shared_ptr<CastExp> n);
//         // void visit(std::shared_ptr<ArrayType> n);
//         // void visit(std::shared_ptr<IdentifierType> n);
//         // void visit(std::shared_ptr<FieldAccessExp> n);
//         void visit(std::shared_ptr<NewArrayExp> n);
//         // void visit(std::shared_ptr<NegExp> n);
//         std::shared_ptr<Expr> visit(std::shared_ptr<ParExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<LessExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<GreaterExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<LessEqualExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<GreaterEqualExp> n);       
//         // void visit(std::shared_ptr<InstanceOfExp> n);  
//         std::shared_ptr<BinOp> visit(std::shared_ptr<EqualExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<NotEqualExp> n);
//         // void visit(std::shared_ptr<AndExp> n);
//         // void visit(std::shared_ptr<XorExp> n);
//         // void visit(std::shared_ptr<OrExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<ConditionalAndExp> n);
//         std::shared_ptr<BinOp> visit(std::shared_ptr<ConditionalOrExp> n);     
//         std::shared_ptr<Move> visit(std::shared_ptr<Assignment> n);
//         std::shared_ptr<Call> visit(std::shared_ptr<MethodInvocation> n);
//         // void visit(std::shared_ptr<ClassInstanceCreationExp> n);
//         // void visit(std::shared_ptr<Identifier> n);
//         // void visit(std::shared_ptr<FormalParameter> n);
//         // void visit(std::shared_ptr<Constructor> n);
//         std::shared_ptr<FuncDecl> visit(std::shared_ptr<Method> n);
//         // void visit(std::shared_ptr<Field> n);
//         std::shared_ptr<CompUnit> visit(std::shared_ptr<ClassDecl> n);
//         std::shared_ptr<CompUnit> visit(std::shared_ptr<InterfaceDecl> n);
//         std::shared_ptr<CompUnit> visit(std::shared_ptr<Program> n);
//         std::shared_ptr<Seq> visit(std::shared_ptr<BlockStatement> n);
//         std::shared_ptr<Seq> visit(std::shared_ptr<BlockStatements> n);
//         std::shared_ptr<Seq> visit(std::shared_ptr<SemicolonStatement> n);
//         std::shared_ptr<Seq> visit(std::shared_ptr<IfStatement> n);
//         std::shared_ptr<Seq> visit(std::shared_ptr<WhileStatement> n);
//         std::shared_ptr<Seq> visit(std::shared_ptr<ForStatement> n);
//         std::shared_ptr<Return> visit(std::shared_ptr<ReturnStatement> n);
//         std::shared_ptr<Node> visit(std::shared_ptr<ExpressionStatement> n);
//         std::shared_ptr<Move> visit(std::shared_ptr<LocalVariableDeclarationStatement> n);
// };
