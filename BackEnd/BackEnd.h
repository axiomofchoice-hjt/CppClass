#pragma once

#include <string>
#include <vector>

#include "../Fmt.h"
#include "../FrontEnd/FrontEnd.h"

namespace BackEnd {
class CodeGenerator {
   public:
    // size_t indent;
    std::vector<FrontEnd::Block> blocks;

    CodeGenerator(const std::vector<FrontEnd::Block> &blocks);
    void __enum_header(Fmt &fmt, const FrontEnd::Block &);
    void __class_header(Fmt &fmt, const FrontEnd::Block &);
    void __enum_source(Fmt &fmt, const FrontEnd::Block &);
    void __class_source(Fmt &fmt, const FrontEnd::Block &);
    std::string header();
    std::string source(const std::string &baseName);
};
}  // namespace BackEnd