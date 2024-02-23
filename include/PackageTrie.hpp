#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "symbolTable.hpp"

class PackageTrieNode {
public:
    std::unordered_map<std::string, std::shared_ptr<PackageTrieNode>> children;
    std::shared_ptr<SymbolTable> symbolTable;

    PackageTrieNode();
};

class PackageTrie {
    public:
        std::shared_ptr<PackageTrieNode> root;
        PackageTrie();
        bool insert(const std::string& qualifiedName, std::shared_ptr<SymbolTable> table);
        bool isPackage(const std::string& qualifiedName);
        std::shared_ptr<SymbolTable> getSymbolTable(const std::string& qualifiedName);
        std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> getMemberTypes(const std::string& packageName);
        // std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> getAllMemberTypes() const;
};
