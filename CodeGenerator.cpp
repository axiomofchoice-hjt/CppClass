#include "CodeGenerator.h"

#include <cstring>
#include <initializer_list>

#include "BuiltinTypes.h"
#include "Fmt.h"

namespace Compiler {
CodeGenerator::CodeGenerator(const std::vector<Block> &blocks)
    : indent(4), blocks(blocks) {}

namespace Name {
const char *Tag = "__Tag";
const char *tag = "__tag";
const char *undef_tag = "__UNDEF";
const char *Data = "__Data";
const char *data = "__data";
const char *undef_data = "__UNDEF";
}  // namespace Name

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
                fmt.print("%s(%s &&);\n", block.name, block.name);
                fmt.print("%s(const %s &);\n", block.name, block.name);
                // deconstructor
                fmt.print("~%s();\n", block.name);
                // assign operator
                fmt.print("%s &operator=(%s &&);\n", block.name, block.name);
                fmt.print("%s &operator=(const %s &);\n", block.name,
                          block.name);

                // enum class
                fmt.print("enum class %s {\n", Name::Tag);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("__UNDEF,\n");
                    for (auto i : block.elements) {
                        fmt.print("%s,\n", i.key);
                    }
                }
                fmt.print("};\n");
                fmt.print("%s %s;\n", Name::Tag, Name::tag);
                // union class
                if (use_union) {
                    fmt.print("union %s {\n", Name::Data);
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("%s();\n", Name::Data);
                        fmt.print("~%s();\n", Name::Data);
                        fmt.print("struct {} %d;\n", Name::undef_data);
                        for (auto i : block.elements) {
                            if (!i.value.empty()) {
                                fmt.print("%s %s;\n", i.value, i.key);
                            }
                        }
                    }
                    fmt.print("};\n");
                    fmt.print("%s %s;\n", Name::Data, Name::data);
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
                // delete
                fmt.print("void __del();\n");
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
    fmt.print("#include \"%s.h\"\n\n", baseName);
    fmt.print("#include <utility>\n\n");
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            bool use_union = block.isComplexEnum();
            // constructor
            Recv t;
            t.append_format("%s(%s::%s)", Name::tag, Name::Tag,
                            Name::undef_tag);
            if (use_union) {
                t.append_format(", %s()", Name::data);
            }
            fmt.print("%s::%s() : %s {}\n", block.name, block.name, t.data);
            fmt.print(
                "%s::%s(%s &&other) : %s { *this = "
                "std::move(other); }\n",
                block.name, block.name, block.name, t.data);
            fmt.print("%s::%s(const %s &other) : %s { *this = other; }\n",
                      block.name, block.name, block.name, t.data);

            // deconstructor
            fmt.print("%s::~%s() { __del(); }\n", block.name, block.name);
            // assign operator
            fmt.print("%s &%s::operator=(%s &&other) {\n", block.name,
                      block.name, block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("__del();\n");
                fmt.print("%s = other.%s;\n", Name::tag, Name::tag);
                fmt.print("switch (%s) {\n", Name::tag);
                {
                    auto guard = fmt.indent_guard();
                    for (auto i : block.elements) {
                        if (!i.value.empty()) {
                            fmt.print("case %d::%d:\n", Name::Tag, i.key);
                            {
                                auto guard = fmt.indent_guard();
                                fmt.print("%s.%s = std::move(other.%s.%s);\n",
                                          Name::data, i.key, Name::data, i.key);
                                fmt.print("break;\n");
                            }
                        }
                    }
                    fmt.print("default:\n");
                    fmt.print("    break;\n");
                }
                fmt.print("}\n");
                fmt.print("other.__del();\n");
                fmt.print("return *this;\n");
            }
            fmt.print("}\n");
            fmt.print("%s &%s::operator=(const %s &other) {\n", block.name,
                      block.name, block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("if (this == &other) { return *this; }\n");
                fmt.print("__del();\n");
                fmt.print("%s = other.%s;\n", Name::tag, Name::tag);

                fmt.print("switch (%s) {\n", Name::tag);
                {
                    auto guard = fmt.indent_guard();
                    for (auto i : block.elements) {
                        if (!i.value.empty()) {
                            fmt.print("case %d::%d:\n", Name::Tag, i.key);
                            {
                                auto guard = fmt.indent_guard();
                                fmt.print("%s.%s = other.%s.%s;\n", Name::data,
                                          i.key, Name::data, i.key);
                                fmt.print("break;\n");
                            }
                        }
                    }
                    fmt.print("default:\n");
                    fmt.print("    break;\n");
                }
                fmt.print("}\n");
                fmt.print("return *this;\n");
            }
            fmt.print("}\n");

            // union constructor & deconstructor
            if (use_union) {
                fmt.print("%s::%s::%s() {}\n", block.name, Name::Data,
                          Name::Data);
                fmt.print("%s::%s::~%s() {}\n", block.name, Name::Data,
                          Name::Data);
            }
            // static construct functions
            for (auto i : block.elements) {
                if (i.value.empty()) {
                    fmt.print("const %s %s::%s() {\n", block.name, block.name,
                              i.key);
                    fmt.print("    %s res;\n", block.name);
                    fmt.print("    res.%s = %s::%s;\n", Name::tag, Name::Tag,
                              i.key);
                    fmt.print("    return res;\n");
                    fmt.print("}\n");
                } else {
                    fmt.print("const %s %s::%s(%s value) {\n", block.name,
                              block.name, i.key, i.value);
                    fmt.print("    %s res;\n", block.name);
                    fmt.print("    res.%s = %s::%s;\n", Name::tag, Name::Tag,
                              i.key);
                    fmt.print("    res.%s.%s = value;\n", Name::data, i.key);
                    fmt.print("    return res;\n");
                    fmt.print("}\n");
                }
            }
            // equal operator
            if (!use_union) {
                fmt.print("bool %s::operator==(%s other) const {\n", block.name,
                          block.name);
                fmt.print("    return %s == other.%s;\n", Name::tag, Name::tag);
                fmt.print("}\n");
                fmt.print("bool %s::operator!=(%s other) const {\n", block.name,
                          block.name);
                fmt.print("    return %s != other.%s;\n", Name::tag, Name::tag);
                fmt.print("}\n");
            }
            // delete
            fmt.print("void %s::__del() {\n", block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("switch (%s) {\n", Name::tag);
                {
                    auto guard = fmt.indent_guard();
                    for (auto i : block.elements) {
                        if (!i.value.empty() &&
                            !no_deconstructor.count(i.value)) {
                            fmt.print("case %d::%d:\n", Name::Tag, i.key);
                            {
                                auto guard = fmt.indent_guard();
                                fmt.print("%s.%s.~%s();\n", Name::data, i.key,
                                          i.value);
                                fmt.print("break;\n");
                            }
                        }
                    }
                    fmt.print("default:\n");
                    fmt.print("    break;\n");
                }
                fmt.print("}\n");
                fmt.print("%s = %s::%s;\n", Name::tag, Name::Tag,
                          Name::undef_tag);
            }
            fmt.print("}\n");
        } else if (block.type == BlockType::Class) {
            std::vector<int> a;
        }
    }
    return fmt.recv.data;
}
}  // namespace Compiler