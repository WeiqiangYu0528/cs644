#pragma once
#include <memory>
#include <map>
#include "scope.hpp"
#include "visitor.hpp"

enum class ExpType {
    Integer = 0,
    Char,
    Short,
    Boolean,
    Object,
    Undefined = -1,

};

enum class ExpRuleType {
    Arithmetic,
    Comparison,
    Undefined = -1
};

class TypeCheckingVisitor : public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        bool error;
        // used to determine a scope for field initialization
        bool initialized;

        ExpType currentExpType {ExpType::Undefined};
        std::string currentObjectTypeName;

        std::map<std::tuple<ExpRuleType, ExpType, ExpType>, ExpType> typeOperationRules = 
        {   // int + int
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Integer}, ExpType::Integer},
            // int + char
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Char}, ExpType::Integer},
            {{ExpRuleType::Arithmetic, ExpType::Char, ExpType::Integer}, ExpType::Integer},
            // int + short
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Short}, ExpType::Integer},            
            {{ExpRuleType::Arithmetic, ExpType::Short, ExpType::Integer}, ExpType::Integer},
            // int < int
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Integer}, ExpType::Boolean},
            // int < char
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Char}, ExpType::Boolean},
            {{ExpRuleType::Comparison, ExpType::Char, ExpType::Integer}, ExpType::Boolean},            
            // int < short
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Short}, ExpType::Boolean},            
            {{ExpRuleType::Comparison, ExpType::Short, ExpType::Integer}, ExpType::Boolean},
            

        };


    public:
        TypeCheckingVisitor(std::shared_ptr<Scope> s);
        void visit(std::shared_ptr<FormalParameter> n) override;
        void visit(std::shared_ptr<LocalVariableDeclarationStatement> n) override;
        void visit(std::shared_ptr<Constructor> n) override;
        void visit(std::shared_ptr<Method> n) override;
        void visit(std::shared_ptr<Field> n) override;
        void visit(std::shared_ptr<BlockStatement> n) override;
        void visit(std::shared_ptr<IfStatement> n) override;
        void visit(std::shared_ptr<WhileStatement> n) override;
        void visit(std::shared_ptr<ForStatement> n) override;
        void visit(std::shared_ptr<IdentifierExp> n) override;
        void visit(std::shared_ptr<FieldAccessExp> n) override;
        void visit(std::shared_ptr<MethodInvocation> n) override;
        void visit(std::shared_ptr<Assignment> n) override;
        bool isError() const;

        std::string currentPackageName;
        std::string currentClassName;

        ExpType GetExpType(std::shared_ptr<Exp> n) {
            currentExpType = ExpType::Undefined;
            n->accept(this);
            return currentExpType;
        }

        ExpType CalcExpType(ExpRuleType exp, ExpType lhs_type, ExpType rhs_type) {
            if (lhs_type == ExpType::Undefined || rhs_type == ExpType::Undefined) {
                return ExpType::Undefined;
            }
            if (typeOperationRules.contains({exp, lhs_type, rhs_type}))
                return typeOperationRules[{exp, lhs_type, rhs_type}];
            else
                return ExpType::Undefined;
        }

        void visit(std::shared_ptr<IdentifierType>) override;
        
        void visit(std::shared_ptr<IntegerLiteralExp> n) override;
        void visit(std::shared_ptr<CharLiteralExp> n) override;

        void visit(std::shared_ptr<PlusExp> n) override;
        void visit(std::shared_ptr<MinusExp> n) override;        
        void visit(std::shared_ptr<TimesExp> n) override;
        void visit(std::shared_ptr<DivideExp> n) override;
        // void visit(std::shared_ptr<StringLiteralExp> n) override;

        void visit(std::shared_ptr<LessExp> n) override;
        void visit(std::shared_ptr<GreaterExp> n) override; 
        
};
