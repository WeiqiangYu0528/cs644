#include "symbolTable.hpp"

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
    return mtable.contains(key) ? mtable.at(key) : std::vector<std::shared_ptr<Method>>{};
}

void SymbolTable::putConstuctor(const std::string& key, const std::shared_ptr<Constructor> value) {
    ctable[key].push_back(value);
}

std::vector<std::shared_ptr<Constructor>> SymbolTable::getConstructor(const std::string& key) const {
    return ctable.contains(key) ? ctable.at(key) : std::vector<std::shared_ptr<Constructor>>{};
}

void SymbolTable::putVar(const std::string& key, const std::shared_ptr<AstNode> value) {
    stack_t.push(key);
    ltable[key] = value;
}

std::shared_ptr<AstNode> SymbolTable::getVar(const std::string& key) const {
    return ltable.contains(key) ? ltable.at(key) : nullptr;
}

void SymbolTable::beginScope() {
    stack_t.push("#");
}

void SymbolTable::endScope() {
    while (stack_t.top() != "#") {
        ltable.erase(stack_t.top());
        stack_t.pop();
    }
    stack_t.pop();
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
std::unordered_map<std::string, std::vector<std::shared_ptr<Method>>>& SymbolTable::getIMTable() {
    return imtable;
}
