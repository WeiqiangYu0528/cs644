#pragma once
#include <iostream>
#include <memory>
#include <string>
#include "weeder.hpp"

class Visitor;

class Exp
{
public:
    ~Exp() = default;
    virtual void accept(Visitor *v);
};

class Statement
{
public:
    virtual ~Statement() = default;
    virtual void accept(Visitor *v) = 0;
};

class BlockStatement : public Statement,
                       std::enable_shared_from_this<BlockStatement>
{
public:
    std::shared_ptr<StatementList> statementList;
    BlockStatement(std::shared_ptr<StatementList> sl);
    void accept(Visitor *v) override;
}

// This is for if/then and if/else
class IfStatement : public Statement,
                    std::enable_shared_from_this<IfThenStatement>
{
public:
    std::shared_ptr<ParExp> parExp;
    std::shared_ptr<Statement> statement1, statement2;
    IfStatement(std::shared_ptr<ParExp> e, std::shared_ptr<Statement> s1, std::shared_ptr<Statement> s2);
    void accept(Visitor *v) override;
}

class WhileStatement : public Statement,
                       std::enable_shared_from_this<WhileStatement>
{
    std::shared_ptr<ParExp> parExp;
    std::shared_ptr<Statement> statement;
    WhileStatement(std::shared_ptr<ParExp> e, std::shared_ptr<Statement>);
    void accept(Visitor *v) override;
}

class ForStatement : public Statement, std::enable_shared_from_this<ForStatement> {
public:
    using ForInit = std::variant<std::shared_ptr<ExpressionStatement>, std::shared_ptr<LocalVariableDeclarationStatement>>;
    ForInit forInit;
    std::shared_ptr<Exp> exp; // ForExpression
    std::shared_ptr<ExpressionStatement> expStmt2; // ForUpdate
    std::shared_ptr<Statement> stmt;

    // Constructor for ExpressionStatement initialization
    ForStatement(std::shared_ptr<ExpressionStatement> es1, std::shared_ptr<Exp> e, std::shared_ptr<ExpressionStatement> es2, std::shared_ptr<Statement> s)

    // Constructor for LocalVariableDeclarationStatement initialization
    ForStatement(std::shared_ptr<LocalVariableDeclarationStatement> lvds, std::shared_ptr<Exp> e, std::shared_ptr<ExpressionStatement> es2, std::shared_ptr<Statement> s)

    void accept(Visitor *v) override;
};

class ReturnStatement : public Statement, 
                        std::enable_shared_from_this<ReturnStatement>
{
    std::shared_ptr<Exp> exp;
    ReturnStatement(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
}

class ExpressionStatement : public Statement, 
                            std::enable_shared_from_this<ExpressionStatement>
{
    std::shared_ptr<Exp> exp;
    ExpressionStatement(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
}

class LocalVariableDeclarationStatement : public Statement, 
                                          std::enable_shared_from_this<LocalVariableDeclarationStatement>
{
    std::shared_ptr<Type> type;
    std::shared_ptr<Identifier> id;
    std::shared_ptr<Exp> exp;
    LocalVariableDeclarationStatement(std::shared_ptr<Type> t, std::shared_ptr<Identifier> i, std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
}

class BlockStatements
{
public:
    std::vector<std::shared_ptr<Statement>> statements;
    BlockStatements() = default;
    void addStatement(std::shared_ptr<Statement> stmt)
    {
        statements.push_back(stmt);
    }
};

class Type
{
public:
    DataType type;
    // should delete this default constructor later, currently used as a placeholder for basic case types
    Type() = default;
    Type(DataType t);
    ~Type() = default;
    virtual void accept(Visitor *v);
};

class Ast
{
private:
    std::shared_ptr<Exp> exp;

public:
    void setAst(std::shared_ptr<Exp> exp);
    std::shared_ptr<Exp> getAst() const;
};

class Identifier
{
public:
    std::string name;
    Identifier(std::string &s);
};

class IdentifierType : public Type, public std::enable_shared_from_this<IdentifierType>
{
public:
    std::shared_ptr<Identifier> id;
    IdentifierType(std::shared_ptr<Identifier> i);
    void accept(Visitor *v) override;
};

class ArrayType : public Type, public std::enable_shared_from_this<ArrayType>
{
public:
    std::shared_ptr<Type> dataType;
    ArrayType(std::shared_ptr<Type> t);
    void accept(Visitor *v) override;
};

// class CompoundType : public Type, public std::enable_shared_from_this<CompoundType> {
//     public:
//         std::shared_ptr<Type> type;
//         std::shared_ptr<IdentifierType> subType;
//         CompoundType(std::shared_ptr<Type> t1, std::shared_ptr<IdentifierType> t2);
//         void accept(Visitor* v) override;
// };

class PlusExp : public Exp, public std::enable_shared_from_this<PlusExp>
{
public:
    std::shared_ptr<Exp> exp1, exp2;
    PlusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
    void accept(Visitor *v) override;
};

class MinusExp : public Exp, public std::enable_shared_from_this<MinusExp>
{
public:
    std::shared_ptr<Exp> exp1, exp2;
    MinusExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
    void accept(Visitor *v) override;
};

class TimesExp : public Exp, public std::enable_shared_from_this<TimesExp>
{
public:
    std::shared_ptr<Exp> exp1, exp2;
    TimesExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
    void accept(Visitor *v) override;
};

class DivideExp : public Exp, public std::enable_shared_from_this<DivideExp>
{
public:
    std::shared_ptr<Exp> exp1, exp2;
    DivideExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
    void accept(Visitor *v) override;
};

class ModuloExp : public Exp, public std::enable_shared_from_this<ModuloExp>
{
public:
    std::shared_ptr<Exp> exp1, exp2;
    ModuloExp(std::shared_ptr<Exp> e1, std::shared_ptr<Exp> e2);
    void accept(Visitor *v) override;
};

class NotExp : public Exp, public std::enable_shared_from_this<NotExp>
{
public:
    std::shared_ptr<Exp> exp;
    NotExp(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
};

class IdentifierExp : public Exp, public std::enable_shared_from_this<IdentifierExp>
{
public:
    std::shared_ptr<Identifier> id;
    IdentifierExp(std::shared_ptr<Identifier> i);
    void accept(Visitor *v) override;
};

class IntegerLiteralExp : public Exp, public std::enable_shared_from_this<IntegerLiteralExp>
{
public:
    long value;
    IntegerLiteralExp(long val);
    void accept(Visitor *v) override;
};

class BoolLiteralExp : public Exp, public std::enable_shared_from_this<BoolLiteralExp>
{
public:
    bool value;
    BoolLiteralExp(bool val);
    void accept(Visitor *v) override;
};

class CharLiteralExp : public Exp, public std::enable_shared_from_this<CharLiteralExp>
{
public:
    char value;
    CharLiteralExp(char val);
    void accept(Visitor *v) override;
};

class StringLiteralExp : public Exp, public std::enable_shared_from_this<StringLiteralExp>
{
public:
    std::string value;
    StringLiteralExp(std::string &val);
    void accept(Visitor *v) override;
};

class NulLiteralExp : public Exp, public std::enable_shared_from_this<NulLiteralExp>
{
public:
    NulLiteralExp();
    void accept(Visitor *v) override;
};

class ArrayAccessExp : public Exp, public std::enable_shared_from_this<ArrayAccessExp>
{
public:
    std::shared_ptr<Exp> array, index;
    ArrayAccessExp(std::shared_ptr<Exp> arr, std::shared_ptr<Exp> idx);
    void accept(Visitor *v) override;
};

class ThisExp : public Exp, public std::enable_shared_from_this<ThisExp>
{
public:
    ThisExp();
    void accept(Visitor *v) override;
};

class CastExp : public Exp, public std::enable_shared_from_this<CastExp>
{
public:
    std::shared_ptr<Exp> exp;
    std::shared_ptr<Type> type;
    CastExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t);
    void accept(Visitor *v) override;
};

class FieldAccessExp : public Exp, public std::enable_shared_from_this<FieldAccessExp>
{
public:
    std::shared_ptr<Exp> exp;
    std::shared_ptr<Identifier> id;
    FieldAccessExp(std::shared_ptr<Exp> e, std::shared_ptr<Identifier> i);
    void accept(Visitor *v) override;
};

class NewArrayExp : public Exp, public std::enable_shared_from_this<NewArrayExp>
{
public:
    std::shared_ptr<Exp> exp;
    std::shared_ptr<Type> type;
    NewArrayExp(std::shared_ptr<Exp> e, std::shared_ptr<Type> t);
    void accept(Visitor *v) override;
};

class NegExp : public Exp, public std::enable_shared_from_this<NegExp>
{
public:
    std::shared_ptr<Exp> exp;
    NegExp(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
};

class ParExp : public Exp, public std::enable_shared_from_this<ParExp>
{
public:
    std::shared_ptr<Exp> exp;
    ParExp(std::shared_ptr<Exp> e);
    void accept(Visitor *v) override;
};
