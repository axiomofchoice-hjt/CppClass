#include "CodeGenerator.h"

namespace Compiler {
CodeGenerator::CodeGenerator(const std::vector<Block> &blocks)
    : indent(4), blocks(blocks) {}
}  // namespace Compiler