#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

std::unordered_set<std::string> no_deconstructor = {"int", "float"};

std::unordered_map<std::string, std::string> BuiltinTypes = {
    {"int", "int"},
    {"float", "float"},
    {"string", "std::string"},
};