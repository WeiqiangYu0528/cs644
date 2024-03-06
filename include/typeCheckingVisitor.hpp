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
    Byte,
    Boolean,
    Object,
    Array,
    String,
    Null,
    Undefined,
    Any
};

extern std::string expTypeString[];

enum class ExpRuleType {
    ArithmeticOrBitwise,
    Comparison,
    Equality,
    Undefined = -1
};

struct argumentExp {
    ExpType expType = ExpType::Undefined;
    std::string objectName;
    DataType arrayType;
};

class TypeCheckingVisitor : public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        bool error;
        // used to determine a scope for field initialization
        bool initialized;
        // used to determine a scope for static method
        bool staticMethod;

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
            // int + byte
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Integer, ExpType::Byte}, ExpType::Integer},    
            // short + short           
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Short, ExpType::Short}, ExpType::Short},
            // short + char
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Short, ExpType::Char}, ExpType::Short},
            // short + byte
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Short, ExpType::Byte}, ExpType::Short},
            // byte + byte
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Byte, ExpType::Byte}, ExpType::Byte},                       
            // char + char
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Char, ExpType::Char}, ExpType::Char},
                        

            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Integer, ExpType::String}, ExpType::String},    
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Short, ExpType::String}, ExpType::String},
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Byte, ExpType::String}, ExpType::String},    
            {{ExpRuleType::ArithmeticOrBitwise, ExpType::Char, ExpType::String}, ExpType::String},                                   


            // int < int
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Integer}, ExpType::Boolean},
            // int < char
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Char}, ExpType::Boolean},
            // int < short
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Short}, ExpType::Boolean},
            // int < byte
            {{ExpRuleType::Comparison, ExpType::Integer, ExpType::Short}, ExpType::Boolean},                        
            // short < short           
            {{ExpRuleType::Comparison, ExpType::Short, ExpType::Short}, ExpType::Boolean},
            // short < char
            {{ExpRuleType::Comparison, ExpType::Short, ExpType::Char}, ExpType::Boolean},
            // short < byte
            {{ExpRuleType::Comparison, ExpType::Short, ExpType::Byte}, ExpType::Boolean},              
            // byte < byte
            {{ExpRuleType::Comparison, ExpType::Byte, ExpType::Byte}, ExpType::Boolean},   
            // byte < char
            {{ExpRuleType::Comparison, ExpType::Byte, ExpType::Char}, ExpType::Boolean},                             
            // char < char
            {{ExpRuleType::Comparison, ExpType::Char, ExpType::Char}, ExpType::Boolean},

            {{ExpRuleType::Equality, ExpType::Boolean, ExpType::Boolean}, ExpType::Boolean},
            {{ExpRuleType::Equality, ExpType::Object, ExpType::Object}, ExpType::Boolean},
            {{ExpRuleType::Equality, ExpType::Array, ExpType::Array}, ExpType::Boolean},
            {{ExpRuleType::Equality, ExpType::String, ExpType::String}, ExpType::Boolean},
            {{ExpRuleType::Equality, ExpType::Object, ExpType::Null}, ExpType::Boolean},
            {{ExpRuleType::Equality, ExpType::String, ExpType::Null}, ExpType::Boolean},            
            {{ExpRuleType::Equality, ExpType::Array, ExpType::Null}, ExpType::Boolean},            
        };

        std::set<std::pair<ExpType, ExpType>> assginmentRules = {
            // first <= second
            {ExpType::Integer, ExpType::Integer},
            {ExpType::Integer, ExpType::Short},
            {ExpType::Integer, ExpType::Char},
            {ExpType::Integer, ExpType::Byte},            
            {ExpType::Short, ExpType::Short}, 
            {ExpType::Short, ExpType::Byte},                        
            {ExpType::Byte, ExpType::Byte},                                    
            {ExpType::Char, ExpType::Char},                                                
            {ExpType::Boolean, ExpType::Boolean},   
            {ExpType::String, ExpType::String},                       
            {ExpType::String, ExpType::Null},              
        };

        std::set<std::pair<ExpType, ExpType>> castingRules = {
            // first <= second
            {ExpType::Integer, ExpType::Integer},
            {ExpType::Integer, ExpType::Short},    
            {ExpType::Integer, ExpType::Byte},    
            {ExpType::Integer, ExpType::Char},    

            {ExpType::Short, ExpType::Integer},
            {ExpType::Short, ExpType::Short},
            {ExpType::Short, ExpType::Char},      
            {ExpType::Short, ExpType::Byte},      

            {ExpType::Char, ExpType::Integer},
            {ExpType::Char, ExpType::Short},
            {ExpType::Char, ExpType::Char},      
            {ExpType::Char, ExpType::Byte},    

            {ExpType::Byte, ExpType::Integer},
            {ExpType::Byte, ExpType::Short},
            {ExpType::Byte, ExpType::Byte},
            {ExpType::Byte, ExpType::Char},
                        
            {ExpType::Boolean, ExpType::Boolean},
            {ExpType::String, ExpType::String},
            {ExpType::Object, ExpType::Object},
            {ExpType::Array, ExpType::Array},
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

        ExpType CalcExpType(ExpRuleType exp, ExpType lhs_type, ExpType rhs_type);
        
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

        void visit(std::shared_ptr<ClassInstanceCreationExp> n) override;
        void visit(std::shared_ptr<NulLiteralExp> n) override;
        void visit(std::shared_ptr<ThisExp> n) override;
        void visit(std::shared_ptr<CastExp> n) override;
        void visit(std::shared_ptr<NewArrayExp> n) override;
        void visit(std::shared_ptr<ParExp> n) override;
        void visit(std::shared_ptr<InstanceOfExp> n) override;

        void SetCurrentExpTypebyAmbiguousName(std::shared_ptr<Type> typeNode);
        void AssignmentTypeCheckingLogic(ExpType left_type, ExpType right_type, std::string left_obj_name, 
            std::string right_obj_name, DataType left_array_type, DataType right_array_type);

        std::shared_ptr<Method> getClosestMatchMethod(std::vector<std::shared_ptr<Method>>& methods, std::vector<argumentExp>& arguments);
        std::shared_ptr<Constructor> getClosestMatchConstructor(std::vector<std::shared_ptr<Constructor>>& constructors, std::vector<argumentExp>& arguments);
        bool isTypeCompatible(std::shared_ptr<Type> dataType, argumentExp& argument);

        std::shared_ptr<SymbolTable> visitClassInstanceCreationExp(std::shared_ptr<ClassInstanceCreationExp> n);
};
