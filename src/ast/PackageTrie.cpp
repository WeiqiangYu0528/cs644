#include "PackageTrie.hpp"

PackageTrieNode::PackageTrieNode() : symbolTable(nullptr) {
}

PackageTrie::PackageTrie() : root(std::make_shared<PackageTrieNode>()) {
}

bool PackageTrie::insert(const std::string& qualifiedName, std::shared_ptr<SymbolTable> table) {
    std::shared_ptr<PackageTrieNode> current = root;
    std::istringstream packageStream(qualifiedName);
    std::string segment;

    while (std::getline(packageStream, segment, '.')) {
        if (current->children.find(segment) == current->children.end()) {
            current->children[segment] = std::make_shared<PackageTrieNode>();
        }
        current = current->children[segment];
        if (current->symbolTable) {
            return false;
        }
    }

    if (!current->children.empty()) {
        return false;
    }

    current->symbolTable = table;
    return true;
}

bool PackageTrie::isPackage(const std::string& qualifiedName){
    std::shared_ptr<PackageTrieNode> current = root;
    std::istringstream packageStream(qualifiedName);
    std::string segment;

    while (std::getline(packageStream, segment, '.')) {
        if (current->children.find(segment) == current->children.end()) {
            return false;
        }
        current = current->children[segment];
    }

    return current->symbolTable == nullptr; // Check if it's a valid package
}

std::shared_ptr<SymbolTable> PackageTrie::getSymbolTable(const std::string& qualifiedName){
    std::shared_ptr<PackageTrieNode> current = root;
    std::istringstream packageStream(qualifiedName);
    std::string segment;

    while (std::getline(packageStream, segment, '.')) {
        if (current->children.find(segment) == current->children.end()) {
            return nullptr;
        }
        current = current->children[segment];
    }

    return current->symbolTable;
}

std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> PackageTrie::getMemberTypes(const std::string& packageName) {
    std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> memberSet;
    std::shared_ptr<PackageTrieNode> current = root;
    std::istringstream packageStream(packageName);
    std::string segment;

    if (packageName == "") current = root->children[""];

    while (std::getline(packageStream, segment, '.')) {
        if (current->children.find(segment) == current->children.end()) {
            return {};
        }
        current = current->children[segment];
    }

    for (auto& [k, v] : current->children) {
        if (v->symbolTable)
            memberSet.push_back({k, v->symbolTable});
    }

    return memberSet;
}

// std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> PackageTrie::getAllMemberTypes() const {
//     std::vector<std::pair<std::string, std::shared_ptr<SymbolTable>>> memberSet;
//     std::shared_ptr<PackageTrieNode> current = root;

//     auto traverse = [&](const std::shared_ptr<PackageTrieNode>& node) {
//         for (auto& [k, v] : node->children) {
//             if (v->symbolTable) {
//                 memberSet.push_back({k, v->symbolTable});
//             }
//             traverse(v);
//         }
//     };

//     traverse(current);

//     return memberSet;
// }
