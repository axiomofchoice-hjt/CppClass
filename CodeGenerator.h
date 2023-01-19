#pragma once

#include <string>
#include <vector>

#include "Parser.h"

class CodeGenerator {
   public:
    size_t indent;

    CodeGenerator();
    std::string header(const std::vector<Block> &blocks);
};