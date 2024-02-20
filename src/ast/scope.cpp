#include "scope.hpp"

Scope::Scope(std::shared_ptr<SymbolTable> c, std::shared_ptr<PackageTrie> p) : current(c), pkgTrie(p){
}

std::shared_ptr<SymbolTable> Scope::getNameInScope(const std::string& name, bool simple) const {
    if (simple) return getUnqualifiedNameInScope(name);
    return getQualifiedNameInScope(name);
}

std::shared_ptr<SymbolTable> Scope::getQualifiedNameInScope(const std::string& name) const {
    auto pos = std::find(name.begin(), name.end(), '.');
    std::string simpleName {name.begin(), pos};
    if (getUnqualifiedNameInScope(simpleName)) return nullptr;
    return pkgTrie->getSymbolTable(name);
}

std::shared_ptr<SymbolTable> Scope::getUnqualifiedNameInScope(const std::string& name) const {
    //try the enclosing class or interface 
    if (name == current->getClassOrInterfaceDecl()->id->name) return current;
    //try any single-type-import (A.B.C.D)
    if (singleImported.contains(name)) return singleImported.at(name);
    //try the same package 
    if (packageMembers.contains(name)) return packageMembers.at(name);
    //try any import-on-demand package (A.B.C.*) including java.lang.*
    if (onDemandImported.contains(name) && !ambiguousNames.contains(name)) return onDemandImported.at(name);
    return nullptr;
}
