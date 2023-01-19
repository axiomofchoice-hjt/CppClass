#pragma once

#include <string>
#include <vector>

#include "Parser.h"

namespace Compiler {
class CodeGenerator {
   public:
    size_t indent;
    std::vector<Block> blocks;

    CodeGenerator(const std::vector<Block> &blocks);
    std::string header();
    std::string source(const std::string &baseName);
};
}  // namespace Compiler