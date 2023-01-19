#include "CodeGenerator.h"

#include <cstring>
#include <initializer_list>

#include "BuiltinTypes.h"
#include "Fmt.h"

namespace Compiler {
CodeGenerator::CodeGenerator(const std::vector<Block> &blocks)
    : indent(4), blocks(blocks) {}

const char *TAG_CLASS_NAME = "Tag";
const char *TAG_VAR_NAME = "tag";
const char *UNION_CLASS_NAME = "Data";
const char *UNION_VAR_NAME = "data";

std::string CodeGenerator::header() {
    Fmt fmt;
    fmt.print("#pragma once\n");
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            bool use_union = block.isComplexEnum();
            fmt.print("class %s {\n", block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print_public();
                // constructor
                fmt.print("%s();\n", block.name);
                // deconstructor
                if (use_union) {
                    fmt.print("~%s();\n", block.name);
                }
                // enum class
                fmt.print("enum class %s {\n", TAG_CLASS_NAME);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("UNDEF,\n");
                    for (auto i : block.elements) {
                        fmt.print("%s,\n", i.key);
                    }
                }
                fmt.print("};\n");
                fmt.print("%s %s;\n", TAG_CLASS_NAME, TAG_VAR_NAME);
                // union class
                if (use_union) {
                    fmt.print("union %s {\n", UNION_CLASS_NAME);
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("char UNDEF;\n");
                        for (auto i : block.elements) {
                            if (!i.value.empty()) {
                                fmt.print("%s %s;\n", i.value, i.key);
                            }
                        }
                    }
                    fmt.print("};\n");
                    fmt.print("%s %s;\n", UNION_CLASS_NAME, UNION_VAR_NAME);
                }
                // static construct functions
                for (auto i : block.elements) {
                    fmt.print("static const %s %s(%s);\n", block.name, i.key,
                              i.value);
                }
                // equal operator
                if (!use_union) {
                    fmt.print("bool operator==(%s other) const;\n", block.name);
                    fmt.print("bool operator!=(%s other) const;\n", block.name);
                }
            }
            fmt.print("};\n");
        } else if (block.type == BlockType::Class) {
            fmt.print("class %s {\n", block.name);
            fmt.print("   public:\n");
            for (auto i : block.elements) {
                fmt.print("    %s %s;\n", i.value, i.key);
            }
            fmt.print("};\n");
        }
    }
    return fmt.recv.data;
}

std::string CodeGenerator::source(const std::string &baseName) {
    Fmt fmt;
    fmt.print("#include \"%s.h\"\n", baseName);
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            bool use_union = block.isComplexEnum();
            // constructor
            if (!use_union) {
                fmt.print("%s::%s() : %s(%s::UNDEF) {}\n", block.name,
                          block.name, TAG_VAR_NAME, TAG_CLASS_NAME);
            } else {
                fmt.print("%s::%s() : %s(%s::UNDEF), %s({0}) {}\n", block.name,
                          block.name, TAG_VAR_NAME, TAG_CLASS_NAME,
                          UNION_VAR_NAME);
            }
            // deconstructor
            if (use_union) {
                fmt.print("%s::~%s() {\n", block.name, block.name);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("switch (%s) {\n", TAG_VAR_NAME);
                    {
                        auto guard = fmt.indent_guard();
                        for (auto i : block.elements) {
                            if (!i.value.empty() &&
                                !no_deconstructor.count(i.value)) {
                                fmt.print("case %d::%d:\n", TAG_CLASS_NAME,
                                          i.key);
                                {
                                    auto guard = fmt.indent_guard();
                                    fmt.print("%s.%s.~%s();\n", UNION_VAR_NAME,
                                              i.key, i.value);
                                    fmt.print("break;\n");
                                }
                            }
                        }
                        fmt.print("default:\n");
                        fmt.print("    break;\n");
                    }
                    fmt.print("}\n");
                }
                fmt.print("}\n");
            }
            // static construct functions
            for (auto i : block.elements) {
                if (i.value.empty()) {
                    fmt.print("const %s %s::%s() {\n", block.name, block.name,
                              i.key);
                    fmt.print("    %s res;\n", block.name);
                    fmt.print("    res.%s = %s::%s;\n", TAG_VAR_NAME,
                              TAG_CLASS_NAME, i.key);
                    fmt.print("    return res;\n");
                    fmt.print("}\n");
                } else {
                    fmt.print("const %s %s::%s(%s value) {\n", block.name,
                              block.name, i.key, i.value);
                    fmt.print("    %s res;\n", block.name);
                    fmt.print("    res.%s = %s::%s;\n", TAG_VAR_NAME,
                              TAG_CLASS_NAME, i.key);
                    fmt.print("    res.%s.%s = value;\n", UNION_VAR_NAME,
                              i.key);
                    fmt.print("    return res;\n");
                    fmt.print("}\n");
                }
            }
            // equal operator
            if (!use_union) {
                fmt.print("bool %s::operator==(%s other) const {\n", block.name,
                          block.name);
                fmt.print("    return %s == other.%s;\n", TAG_VAR_NAME,
                          TAG_VAR_NAME);
                fmt.print("}\n");
                fmt.print("bool %s::operator!=(%s other) const {\n", block.name,
                          block.name);
                fmt.print("    return %s != other.%s;\n", TAG_VAR_NAME,
                          TAG_VAR_NAME);
                fmt.print("}\n");
            }
        } else if (block.type == BlockType::Class) {
        }
    }
    return fmt.recv.data;
}
}  // namespace Compiler