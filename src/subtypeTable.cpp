#include "subtypeTable.hpp"

std::unordered_map<std::shared_ptr<ClassOrInterfaceDecl>, std::unordered_set<std::shared_ptr<ClassOrInterfaceDecl>>> subtypeTable;