#pragma once
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "PackageTrie.hpp"
#include "symbolTable.hpp"

class Scope {
    public:
        std::shared_ptr<SymbolTable> current;
        std::shared_ptr<PackageTrie> pkgTrie;
        std::unordered_set<std::string> ambiguousNames;
        std::unordered_map<std::string, std::shared_ptr<SymbolTable>> packageMembers;
        std::unordered_map<std::string, std::shared_ptr<SymbolTable>> onDemandImported;
        std::unordered_map<std::string, std::shared_ptr<SymbolTable>> singleImported; 

        std::vector<std::shared_ptr<SymbolTable>> supers; //includes superclasses and interfaces

        Scope(std::shared_ptr<SymbolTable> c, std::shared_ptr<PackageTrie> p);
        std::shared_ptr<SymbolTable> getNameInScope(const std::string& name, bool simple) const;
        std::shared_ptr<SymbolTable> getQualifiedNameInScope(const std::string& name) const;
        std::shared_ptr<SymbolTable> getUnqualifiedNameInScope(const std::string& name) const;
};
