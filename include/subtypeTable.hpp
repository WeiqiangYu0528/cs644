#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "ast.hpp"

extern std::unordered_map<std::shared_ptr<SymbolTable>, 
                        std::unordered_set<std::shared_ptr<SymbolTable>>> subtypeTable;

extern std::unordered_map<std::string, std::unordered_set<std::string>> stringsubtypeTable;