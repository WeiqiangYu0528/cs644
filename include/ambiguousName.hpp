#include "PackageTrie.hpp"
#include "ast.hpp"
#include "symbolTable.hpp"
#include "weeder.hpp"

enum class AmbiguousNamesType {
    EXPRESSION = 0,
    TYPE,
    PACKAGE,
    ERROR,
    UNINITIALIZED
};

class AmbiguousName {
    public:
        AmbiguousNamesType type;
        std::shared_ptr<SymbolTable> symbolTable;
        std::shared_ptr<PackageTrieNode> trie;
        AmbiguousName();
        AmbiguousName(AmbiguousNamesType t, std::shared_ptr<SymbolTable> s);
        AmbiguousName(std::shared_ptr<PackageTrieNode> p);
};
