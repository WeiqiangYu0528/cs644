#pragma once
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include "ast.hpp"
class Scope;

enum class ScopeType {
    LOCALVARIABLE = 0,
    STATIC,
    FIELDINITIALIZER,
    ASSIGNMENT
};

class SymbolTable {
    // program AST
    std::shared_ptr<Program> ast;
    // package
    std::string pkg;
    // member type declaration
    std::shared_ptr<ClassOrInterfaceDecl> cdecl;
    // field table
    std::unordered_map<std::string, std::shared_ptr<Field>> ftable;
    // field table used to verify the forward reference of fields, Section 8.3.2.3 
    std::unordered_set<std::string> fieldDecls;
    // declared method table
    std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>> mtable;
    // constructor table
    std::vector<std::shared_ptr<Constructor>> ctable;
    // local variable table
    std::unordered_map<std::string, std::shared_ptr<AstNode>> ltable;
    std::stack<std::string> stack_t;
    std::shared_ptr<Scope> scope;

    bool staticScope;
    bool fieldInitializerScope;
    bool assignmentScope;

    public:
        // non-static field (including parent class's field)
        std::vector<std::shared_ptr<Field>> fields;
        void putField(const std::string& key, const std::shared_ptr<Field> value);
        std::shared_ptr<Field> getField(const std::string& key) const;
        void putMethod(const std::string& key, const std::shared_ptr<Method> value);
        std::vector<std::shared_ptr<Method>> getMethod(const std::string& key) const;
        void putConstuctor(const std::shared_ptr<Constructor> value);
        std::vector<std::shared_ptr<Constructor>>& getConstructor();
        void putVar(const std::string& key, const std::shared_ptr<AstNode> value);
        std::shared_ptr<AstNode> getVar(const std::string& key) const;
        void setAst(std::shared_ptr<Program> a);
        std::shared_ptr<Program> getAst() const;
        void setPackage(std::string& p);
        std::string getPackage() const;
        void setClassOrInterfaceDecl(std::shared_ptr<ClassOrInterfaceDecl> c);
        std::shared_ptr<ClassOrInterfaceDecl> getClassOrInterfaceDecl() const;
        void setScope(std::shared_ptr<Scope> s);
        std::shared_ptr<Scope> getScope() const;
        void setFieldDecl(const std::string& key);
        bool isFieldDeclared(const std::string& key) const;
        void beginScope(ScopeType st);
        void endScope(ScopeType st);
        bool getScopeType(ScopeType st) const;
        
        //inherited from superclass
        std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>> iscmtable;
        //inherited from superinterface
        std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>> isimtable;

        bool imtablesPopulated = false;
        SymbolTable() : staticScope(false), fieldInitializerScope(false), assignmentScope(false), imtablesPopulated(false) {}

        std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& getMTable();
        std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& getISCMTable();
        std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& getISIMTable();
};
