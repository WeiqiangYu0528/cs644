#include "subtypeTable.hpp"

std::unordered_map<std::shared_ptr<SymbolTable>, std::unordered_set<std::shared_ptr<SymbolTable>>> subtypeTable;
std::unordered_map<std::string, std::unordered_set<std::string>> stringsubtypeTable;