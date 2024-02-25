#include "ambiguousName.hpp"

AmbiguousName::AmbiguousName() : type(AmbiguousNamesType::UNINITIALIZED), symbolTable(nullptr), trie(nullptr) {
}

AmbiguousName::AmbiguousName(AmbiguousNamesType t, std::shared_ptr<SymbolTable> s) : type(t), symbolTable(s), trie(nullptr) {
}

AmbiguousName::AmbiguousName(std::shared_ptr<PackageTrieNode> p) : type(AmbiguousNamesType::PACKAGE), symbolTable(nullptr), trie(p) {
}

DataType AmbiguousName::getDataType() const {
    return typeNode->type;
}
