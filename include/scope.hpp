#pragma once
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include "PackageTrie.hpp"
#include "ambiguousName.hpp"
#include "symbolTable.hpp"

class Scope {
    AmbiguousName createAmbiguousName(std::shared_ptr<Type> typeNode, std::shared_ptr<SymbolTable> st);
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
        bool isNameValidInScope(const std::string& name) const;
        AmbiguousName reclassifySimpleAmbiguousName(const std::string& name, std::vector<ExpressionObject>* exprs = nullptr);
        AmbiguousName reclassifyQualifiedAmbiguousName(const std::string& name, std::vector<ExpressionObject>* exprs = nullptr);
        AmbiguousName reclassifyAmbiguousName(const std::string& name, bool simple, std::vector<ExpressionObject>* exprs = nullptr);
        AmbiguousName reclassifyAmbiguousNameByLocal(const std::string& name, std::vector<ExpressionObject>* exprs = nullptr);
        AmbiguousName reclassifyAmbiguousNameByField(const std::string& name, std::shared_ptr<SymbolTable> s, bool staticField, std::vector<ExpressionObject>* exprs = nullptr);
        void updateExpressionObject(const std::string& name, std::shared_ptr<SymbolTable> st, Expression expr, std::shared_ptr<Type> type, std::vector<ExpressionObject>* exprs = nullptr);
        bool superBFS(std::shared_ptr<SymbolTable>& start, std::shared_ptr<SymbolTable>& end, bool strictSubclass = true);
};
