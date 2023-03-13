#include "BackEnd.h"

namespace BackEnd {
CodeGenerator::CodeGenerator(const std::vector<FrontEnd::Block> &blocks)
    : blocks(blocks) {}
}  // namespace BackEnd