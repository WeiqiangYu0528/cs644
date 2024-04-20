#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "ast.hpp"

extern std::unordered_map<std::shared_ptr<ClassOrInterfaceDecl>, 
                        std::unordered_set<std::shared_ptr<ClassOrInterfaceDecl>>> subtypeTable;