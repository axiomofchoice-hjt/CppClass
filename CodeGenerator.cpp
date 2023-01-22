#include "CodeGenerator.h"

namespace Compiler {
CodeGenerator::CodeGenerator(const std::vector<Block> &blocks)
    : blocks(blocks) {}
}  // namespace Compiler