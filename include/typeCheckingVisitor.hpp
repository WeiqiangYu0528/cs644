#pragma once
#include <memory>
#include <set>
#include <map>
#include "scope.hpp"
#include "visitor.hpp"

enum class ExpType {
    Integer = 0,
    Short,
    Char,
    Boolean,
    Object,
    Array,
    String,
    Undefined = -1,
};

enum class ExpRuleType {
    ArithmeticOrBitwise,
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
        DataType currentArrayDataType;

        std::map<std::tuple<ExpRuleType, ExpType, ExpType>, ExpType> typeOperationRules = 
        {   // int + int
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Integer, ExpType::Integer}, ExpType::Integer},
            // int + char
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Integer, ExpType::Char}, ExpType::Integer},
            // int + short
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Integer, ExpType::Short}, ExpType::Integer},            
            // short + short           
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Short, ExpType::Short}, ExpType::Short},
            // short + char
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Short, ExpType::Char}, ExpType::Short},
            // char + char
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Char, ExpType::Char}, ExpType::Char},
                        
            // int < int
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Integer}, ExpType::Boolean},
            // int < char
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Char}, ExpType::Boolean},
            // int < short
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Short}, ExpType::Boolean},            
            // short < short           
            {{ExpRuleType::Comparison, ExpType::Short, ExpType::Short}, ExpType::Boolean},
            // short < char
            {{ExpRuleType::Comparison, ExpType::Short, ExpType::Char}, ExpType::Boolean},
            // char < char
            {{ExpRuleType::Comparison, ExpType::Char, ExpType::Char}, ExpType::Boolean},            
        };

        std::set<std::pair<ExpType, ExpType>> assginmentRules = {
            {ExpType::Integer, ExpType::Integer},
            {ExpType::Integer, ExpType::Short},
            {ExpType::Integer, ExpType::Char},
            {ExpType::Short, ExpType::Short},
            {ExpType::Short, ExpType::Char},            
            {ExpType::Boolean, ExpType::Boolean},   
            {ExpType::String, ExpType::String},        
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
            else if(typeOperationRules.contains({exp, rhs_type, lhs_type}))
                return typeOperationRules[{exp, rhs_type, lhs_type}];                
            else
                return ExpType::Undefined;
        }
        
        void visit(std::shared_ptr<IntegerLiteralExp> n) override;
        void visit(std::shared_ptr<CharLiteralExp> n) override;
        void visit(std::shared_ptr<BoolLiteralExp> n) override;        
        void visit(std::shared_ptr<StringLiteralExp> n) override;

        void visit(std::shared_ptr<PlusExp> n) override;
        void visit(std::shared_ptr<MinusExp> n) override;        
        void visit(std::shared_ptr<TimesExp> n) override;
        void visit(std::shared_ptr<DivideExp> n) override;
        void visit(std::shared_ptr<ModuloExp> n) override;

        void visit(std::shared_ptr<NotExp> n) override;
        void visit(std::shared_ptr<NegExp> n) override;
        void visit(std::shared_ptr<LessExp> n) override;
        void visit(std::shared_ptr<GreaterExp> n) override; 
        void visit(std::shared_ptr<LessEqualExp> n) override; 
        void visit(std::shared_ptr<GreaterEqualExp> n) override; 
        void visit(std::shared_ptr<EqualExp> n) override; 
        void visit(std::shared_ptr<NotEqualExp> n) override; 

        void visit(std::shared_ptr<AndExp> n) override;
        void visit(std::shared_ptr<XorExp> n) override;
        void visit(std::shared_ptr<OrExp> n) override;      

        void visit(std::shared_ptr<ConditionalAndExp> n) override;
        void visit(std::shared_ptr<ConditionalOrExp> n) override;        
    
};