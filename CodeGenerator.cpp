#include "CodeGenerator.h"

#include <cstring>
#include <initializer_list>

#include "Fmt.h"

namespace Compiler {
CodeGenerator::CodeGenerator(const std::vector<Block> &blocks)
    : indent(4), blocks(blocks) {}

const char *TAG_CLASS_NAME = "Tag";
const char *TAG_VAR_NAME = "tag";
const char *UNION_CLASS_NAME = "Data";
const char *UNION_VAR_NAME = "data";

std::string CodeGenerator::header() {
    Fmt res;
    res.print("#pragma once\n");
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            bool complex = block.isComplexEnum();
            res.print("class %s {\n", block.name);
            {
                auto guard = res.indent_guard();
                res.print_public();
                res.print("%s();\n", block.name);
                res.print("enum class %s {\n", TAG_CLASS_NAME);
                {
                    auto guard = res.indent_guard();
                    res.print("UNDEF,\n");
                    for (auto i : block.elements) {
                        res.print("%s,\n", i.key);
                    }
                }
                res.print("};\n");
                res.print("%s %s;\n", TAG_CLASS_NAME, TAG_VAR_NAME);
                if (complex) {
                    res.print("union %s {\n", UNION_CLASS_NAME);
                    res.print("};\n");
                    res.print("%s %s;\n", UNION_CLASS_NAME, UNION_VAR_NAME);
                }
                for (auto i : block.elements) {
                    res.print("static const %s %s(%s);\n", block.name, i.key,
                              i.value);
                }
                res.print("bool operator==(%s other) const;\n", block.name);
                res.print("bool operator!=(%s other) const;\n", block.name);
            }
            res.print("};\n");
        } else if (block.type == BlockType::Class) {
            res.print("class %s {\n", block.name);
            res.print("   public:\n");
            for (auto i : block.elements) {
                res.print("    %s %s;\n", i.value, i.key);
            }
            res.print("};\n");
        }
    }
    return res.recv.data;
}

std::string CodeGenerator::source(const std::string &baseName) {
    Fmt res;
    res.print("#include \"%s.h\"\n", baseName);
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            res.print("%s::%s() : %s(%s::%s::UNDEF) {}\n", block.name,
                      block.name, TAG_VAR_NAME, block.name, TAG_CLASS_NAME);
            for (auto i : block.elements) {
                res.print("const %s %s::%s(%s) {\n", block.name, block.name,
                          i.key, i.value);
                res.print("    %s res;\n", block.name);
                res.print("    res.%s = %s::%s::%s;\n", TAG_VAR_NAME,
                          block.name, TAG_CLASS_NAME, i.key);
                res.print("    return res;\n");
                res.print("}\n");
            }
            res.print(
                "bool %s::operator==(%s other) const { return %s == "
                "other.%s; }\n",
                block.name, block.name, TAG_VAR_NAME, TAG_VAR_NAME);
            res.print(
                "bool %s::operator!=(%s other) const { return %s != "
                "other.%s; }\n",
                block.name, block.name, TAG_VAR_NAME, TAG_VAR_NAME);
        } else if (block.type == BlockType::Class) {
        }
    }
    return res.recv.data;
}
}  // namespace Compiler