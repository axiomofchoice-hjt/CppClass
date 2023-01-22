#pragma once

#include <string>
#include <vector>

#include "Fmt.h"
#include "Parser.h"

namespace Compiler {
class CodeGenerator {
   public:
    // size_t indent;
    std::vector<Block> blocks;

    CodeGenerator(const std::vector<Block> &blocks);
    void __enum_header(Fmt &fmt, const Block &);
    void __class_header(Fmt &fmt, const Block &);
    void __enum_source(Fmt &fmt, const Block &);
    void __class_source(Fmt &fmt, const Block &);
    std::string header();
    std::string source(const std::string &baseName);
};
}  // namespace Compiler