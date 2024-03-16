#pragma once
#include <memory>
#include <set>
#include <map>
#include <typeinfo>
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
    Any,
    Void
};

extern std::string expTypeString[];

enum class ExpRuleType {
    Arithmetic,
    Comparison,
    Equality,
    StringPlus,
    Undefined = -1
};

struct argumentExp {
    ExpType expType = ExpType::Undefined;
    std::string objectName;
    DataType arrayType;

    argumentExp& operator=(const argumentExp& other) {
        expType = other.expType;
        objectName = other.objectName;
        arrayType = other.arrayType;

        if (expType == ExpType::Object && objectName == "String") {
            expType = ExpType::String;
        }
        return *this;
    }
};

class TypeCheckingVisitor : public Visitor {
    private:
        std::shared_ptr<Scope> scope;
        bool error;

        argumentExp currentExpInfo, methodTypeInfo;

        std::map<std::tuple<ExpRuleType, ExpType, ExpType>, ExpType> typeOperationRules = 
        {   // int + int
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Integer}, ExpType::Integer},
            // int + char
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Char}, ExpType::Integer},
            // int + short
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Short}, ExpType::Integer},    
            // int + byte
            {{ExpRuleType::Arithmetic, ExpType::Integer, ExpType::Byte}, ExpType::Integer},    
            // short + short           
            {{ExpRuleType::Arithmetic, ExpType::Short, ExpType::Short}, ExpType::Short},
            // short + char
            {{ExpRuleType::Arithmetic, ExpType::Short, ExpType::Char}, ExpType::Short},
            // short + byte
            {{ExpRuleType::Arithmetic, ExpType::Short, ExpType::Byte}, ExpType::Short},
            // byte + byte
            {{ExpRuleType::Arithmetic, ExpType::Byte, ExpType::Byte}, ExpType::Byte},                       
            // char + char
            {{ExpRuleType::Arithmetic, ExpType::Char, ExpType::Char}, ExpType::Integer},
                        

            {{ExpRuleType::StringPlus, ExpType::Integer, ExpType::String}, ExpType::String},    
            {{ExpRuleType::StringPlus, ExpType::Short, ExpType::String}, ExpType::String},
            {{ExpRuleType::StringPlus, ExpType::Byte, ExpType::String}, ExpType::String},    
            {{ExpRuleType::StringPlus, ExpType::Char, ExpType::String}, ExpType::String},   
            {{ExpRuleType::StringPlus, ExpType::Null, ExpType::String}, ExpType::String},                                               
            {{ExpRuleType::StringPlus, ExpType::Array, ExpType::String}, ExpType::String}, 
            {{ExpRuleType::StringPlus, ExpType::Object, ExpType::String}, ExpType::String},                                                           
            {{ExpRuleType::StringPlus, ExpType::String, ExpType::String}, ExpType::String},
            {{ExpRuleType::StringPlus, ExpType::Boolean, ExpType::String}, ExpType::String},                                                                       


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
            {ExpType::String, ExpType::Object},               
            {ExpType::Object, ExpType::Object},
            {ExpType::Object, ExpType::String},            
            {ExpType::Object, ExpType::Null},                     
            {ExpType::Array, ExpType::Array},
            {ExpType::Array, ExpType::Object},
        };      

    public:
        TypeCheckingVisitor(std::shared_ptr<Scope> s);
        void visit(std::shared_ptr<Program> n) override;
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

        argumentExp GetExpInfo(std::shared_ptr<Exp> n) {
            currentExpInfo.expType = ExpType::Undefined;
            n->accept(this);  
            if (currentExpInfo.expType == ExpType::Object && currentExpInfo.objectName == "String") {
                currentExpInfo.expType = ExpType::String;
            }            
            return currentExpInfo;
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

        void visit(std::shared_ptr<ArrayAccessExp> n) override;   
        void visit(std::shared_ptr<ReturnStatement> n) override;      

        void SetCurrentExpTypebyAmbiguousName(std::shared_ptr<Type> typeNode);
        AmbiguousName AssignmentTypeCheckingLogic(argumentExp& left_type, argumentExp& right_type);

        std::shared_ptr<Method> getClosestMatchMethod(std::vector<std::shared_ptr<Method>>& methods, std::vector<argumentExp>& arguments);
        std::shared_ptr<Constructor> getClosestMatchConstructor(std::vector<std::shared_ptr<Constructor>>& constructors, std::vector<argumentExp>& arguments);
        bool isTypeCompatible(std::shared_ptr<Type> dataType, argumentExp& argument);

        bool checkIsSubclassByName(std::string o1, std::string o2);
        AmbiguousName visitClassInstanceCreationExp(std::shared_ptr<ClassInstanceCreationExp> n);
        AmbiguousName visitCastExp(std::shared_ptr<CastExp> n);
        AmbiguousName visitIdentifierExp(std::shared_ptr<IdentifierExp> n);
        AmbiguousName visitParExp(std::shared_ptr<ParExp> n);
        AmbiguousName visitThisExp(std::shared_ptr<ThisExp> n);
        AmbiguousName visitFieldAccessExp(std::shared_ptr<FieldAccessExp> n);
        AmbiguousName visitMethodInvocation(std::shared_ptr<MethodInvocation> n);
        AmbiguousName visitStringLiteralExp(std::shared_ptr<StringLiteralExp> n);
        AmbiguousName visitNewArrayExp(std::shared_ptr<NewArrayExp> n);
        AmbiguousName visitArrayAccessExp(std::shared_ptr<ArrayAccessExp> n);
        AmbiguousName visitAssignment(std::shared_ptr<Assignment> n);
        
        template<typename BinOpExp>
        void visitBinaryOpExp(std::shared_ptr<BinOpExp> n, ExpRuleType rule_type1 = ExpRuleType::Undefined, ExpRuleType rule_type2 = ExpRuleType::Undefined);
        
        std::shared_ptr<SymbolTable> getSymbolTableByClassName(std::string name);
        bool checkIsSameSymbolTable(std::string o1, std::string o2);
};
