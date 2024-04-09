#pragma once
#include "IRAst.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <sstream>
#include <unordered_set>

void PrintCompUnit(std::shared_ptr<TIR::CompUnit> root, const std::string& filename);