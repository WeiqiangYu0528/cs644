#include "symbolTable.hpp"
#include "scope.hpp"

void SymbolTable::putField(const std::string& key, const std::shared_ptr<Field> value) {
    ftable[key] = value;
}

std::shared_ptr<Field> SymbolTable::getField(const std::string& key) const {
    return ftable.contains(key) ? ftable.at(key) : nullptr;
}

void SymbolTable::putMethod(const std::string& key, const std::shared_ptr<Method> value) {
    mtable[key].push_back(value);

}

std::vector<std::shared_ptr<Method>> SymbolTable::getMethod(const std::string& key) const {
    std::vector<std::shared_ptr<Method>> methods;
    if (mtable.contains(key)) {
        auto m = mtable.at(key);
        std::for_each(m.begin(), m.end(), [&methods](std::shared_ptr<Method> method){methods.push_back(method);});
    }
    if (iscmtable.contains(key)) {
        auto m = iscmtable.at(key);
        std::for_each(m.begin(), m.end(), [&methods](std::shared_ptr<Method> method){methods.push_back(method);});
    }
    return methods;
}

void SymbolTable::putConstuctor(const std::shared_ptr<Constructor> value) {
    ctable.push_back(value);
}

std::vector<std::shared_ptr<Constructor>>& SymbolTable::getConstructor() {
    return ctable;
}

void SymbolTable::putVar(const std::string& key, const std::shared_ptr<AstNode> value) {
    stack_t.push(key);
    ltable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getVar(const std::string& key) const {
    return ltable.contains(key) ? ltable.at(key) : nullptr;
}

void SymbolTable::beginScope(ScopeType st) {
    switch (st) {
        case ScopeType::LOCALVARIABLE:
            stack_t.push("#");
            break;
        case ScopeType::STATIC:
            staticScope = true;
            break;
        case ScopeType::FIELDINITIALIZER:
            fieldInitializerScope = true;
            break;
        case ScopeType::ASSIGNMENT:
            assignmentScope = true;
            break;
    }
}

void SymbolTable::endScope(ScopeType st) {
    switch(st) {
        case ScopeType::LOCALVARIABLE:
            while (stack_t.top() != "#") {
                ltable.erase(stack_t.top());
                stack_t.pop();
            }
            stack_t.pop();
            break;
        case ScopeType::STATIC:
            staticScope = false;
            break;
        case ScopeType::FIELDINITIALIZER:
            fieldInitializerScope = false;
            break;
        case ScopeType::ASSIGNMENT:
            assignmentScope = false;
            break;
    }
}

void SymbolTable::setAst(std::shared_ptr<Program> a) {
    ast = a;
}

std::shared_ptr<Program> SymbolTable::getAst() const {
    return ast;
}

void SymbolTable::setPackage(std::string& p) {
    pkg = p;
}

std::string SymbolTable::getPackage() const {
    return pkg;
}

void SymbolTable::setClassOrInterfaceDecl(std::shared_ptr<ClassOrInterfaceDecl> c) {
    cdecl = c;
}

std::shared_ptr<ClassOrInterfaceDecl> SymbolTable::getClassOrInterfaceDecl() const {
    return cdecl;
}

std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& SymbolTable::getMTable() {
    return mtable;
}
/*std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& SymbolTable::getIMTable() {
    return imtable;
}*/
std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& SymbolTable::getISCMTable() {
    return iscmtable;
}
std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& SymbolTable::getISIMTable() {
    return isimtable;
}

std::unordered_map<std::string, std::shared_ptr<Field>>& SymbolTable::getFieldTable() {
    return ftable;
}

void SymbolTable::setScope(std::shared_ptr<Scope> s) {
    scope = s;
}

std::shared_ptr<Scope> SymbolTable::getScope() const {
    return scope;
}

void SymbolTable::setFieldDecl(const std::string& key) {
    fieldDecls.insert(key);
}

bool SymbolTable::isFieldDeclared(const std::string& key) const {
    return fieldDecls.contains(key);
}

bool SymbolTable::getScopeType(ScopeType st) const {
    switch(st) {
        case ScopeType::STATIC:
            return staticScope;
        case ScopeType::FIELDINITIALIZER:
            return fieldInitializerScope;
        case ScopeType::ASSIGNMENT:
            return assignmentScope;
        default:
            return false;
    }
}
