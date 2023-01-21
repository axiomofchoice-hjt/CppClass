#include "CodeGenerator.h"

#include <cstring>
#include <initializer_list>

#include "BuiltinTypes.h"
#include "Fmt.h"

namespace Compiler {
CodeGenerator::CodeGenerator(const std::vector<Block> &blocks)
    : indent(4), blocks(blocks) {}

std::string CodeGenerator::header() {
    Fmt fmt;
    fmt.print("#pragma once\n\n");
    fmt.print("#include <string>\n\n");
    fmt.print("#include \"../CppClass.h\"\n\n");
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            bool use_union = block.isComplexEnum();
            fmt.print("class %s;\n", block.name);
            fmt.print("namespace CppClass {\n");
            fmt.print("void __appendBinary(Bytes &, const %s &);\n", block.name);
            fmt.print("}\n");
            fmt.print("class %s : public CppClass::%s<%s> {\n", block.name,
                      use_union ? "ComplexEnum" : "SimpleEnum", block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print_private();
                // enum class
                fmt.print("enum class __Tag {\n");
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("__UNDEF,\n");
                    for (auto i : block.elements) {
                        fmt.print("%s,\n", i.key);
                    }
                }
                fmt.print("};\n");
                fmt.print("__Tag __tag;\n");
                // union class
                if (use_union) {
                    fmt.print("union __Data {\n");
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("__Data();\n");
                        fmt.print("~__Data();\n");
                        fmt.print(
                            "void __assign(__Tag tag, __Data &&other);\n");
                        fmt.print(
                            "void __assign(__Tag tag, const __Data &other);\n");
                        fmt.print("void __del(__Tag tag);\n");
                        fmt.print("struct {} __UNDEF;\n");
                        for (auto i : block.elements) {
                            if (!i.value.empty()) {
                                fmt.print("%s %s;\n", i.value, i.key);
                            }
                        }
                    }
                    fmt.print("};\n");
                    fmt.print("__Data __data;\n");
                }
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
                fmt.print("%s &operator=(std::nullptr_t);\n", block.name);

                // static construct functions
                for (auto i : block.elements) {
                    fmt.print("static const %s %s(%s);\n", block.name, i.key,
                              i.value);
                }
                // is
                for (auto i : block.elements) {
                    fmt.print("bool is_%s() const;\n", i.key);
                }
                // get
                for (auto i : block.elements) {
                    if (!i.value.empty()) {
                        fmt.print("const %s &get_%s() const;\n", i.value,
                                  i.key);
                    }
                }
                // to binary
                fmt.print(
                    "friend void CppClass::__appendBinary(CppClass::Bytes &, "
                    "const %s &);\n",
                    block.name);
                // friend
                fmt.print("friend CppClass::%s<%s>;\n",
                          use_union ? "ComplexEnum" : "SimpleEnum", block.name);
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
            t.append_format("__tag(__Tag::__UNDEF)");
            if (use_union) {
                t.append_format(", __data()");
            }
            fmt.print("%s::%s() : %s {}\n", block.name, block.name, t.data);
            fmt.print(
                "%s::%s(%s &&other) : %s { __assign(std::move(other)); }\n",
                block.name, block.name, block.name, t.data);
            fmt.print("%s::%s(const %s &other) : %s { __assign(other); }\n",
                      block.name, block.name, block.name, t.data);

            // deconstructor
            fmt.print("%s::~%s() { __del(); }\n", block.name, block.name);
            // assign operator
            fmt.print("%s &%s::operator=(%s &&other) {\n", block.name,
                      block.name, block.name);
            fmt.print("    return __assign(std::move(other));\n");
            fmt.print("}\n");
            fmt.print("%s &%s::operator=(const %s &other) {\n", block.name,
                      block.name, block.name);
            fmt.print("    return __assign(other);\n");
            fmt.print("}\n");
            fmt.print("%s &%s::operator=(std::nullptr_t) {\n", block.name,
                      block.name);
            fmt.print("    return __assign(nullptr);\n");
            fmt.print("}\n");

            // union
            if (use_union) {
                fmt.print("%s::__Data::__Data() {}\n", block.name);
                fmt.print("%s::__Data::~__Data() {}\n", block.name);

                fmt.print(
                    "void %s::__Data::__assign(__Tag __tag, __Data &&other) "
                    "{\n",
                    block.name);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("switch (__tag) {\n");
                    {
                        auto guard = fmt.indent_guard();
                        for (auto i : block.elements) {
                            if (!i.value.empty()) {
                                fmt.print("case __Tag::%s:\n", i.key);
                                {
                                    auto guard = fmt.indent_guard();
                                    fmt.print(
                                        "this->%s = "
                                        "std::move(other.%s);\n",
                                        i.key, i.key);
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

                fmt.print(
                    "void %s::__Data::__assign(__Tag __tag, const __Data "
                    "&other) "
                    "{\n",
                    block.name);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("switch (__tag) {\n");
                    {
                        auto guard = fmt.indent_guard();
                        for (auto i : block.elements) {
                            if (!i.value.empty()) {
                                fmt.print("case __Tag::%s:\n", i.key);
                                {
                                    auto guard = fmt.indent_guard();
                                    fmt.print("this->%s = other.%s;\n", i.key,
                                              i.key);
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

                fmt.print("void %s::__Data::__del(__Tag __tag) {\n",
                          block.name);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("switch (__tag) {\n");
                    {
                        auto guard = fmt.indent_guard();
                        for (auto i : block.elements) {
                            if (!i.value.empty() &&
                                !no_deconstructor.count(i.value)) {
                                fmt.print("case __Tag::%s:\n", i.key);
                                {
                                    auto guard = fmt.indent_guard();
                                    fmt.print("this->%s.~%s();\n", i.key,
                                              i.value);
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
                    fmt.print("    res.__tag = __Tag::%s;\n", i.key);
                    fmt.print("    return res;\n");
                    fmt.print("}\n");
                } else {
                    fmt.print("const %s %s::%s(%s value) {\n", block.name,
                              block.name, i.key, i.value);
                    fmt.print("    %s res;\n", block.name);
                    fmt.print("    res.__tag = __Tag::%s;\n", i.key);
                    fmt.print("    res.__data.%s = value;\n", i.key);
                    fmt.print("    return res;\n");
                    fmt.print("}\n");
                }
            }
            // is
            for (auto i : block.elements) {
                fmt.print("bool %s::is_%s() const {\n", block.name, i.key);
                fmt.print("    return __tag == __Tag::%s;\n", i.key);
                fmt.print("}\n");
            }
            // get
            for (auto i : block.elements) {
                if (!i.value.empty()) {
                    fmt.print(
                        "const %s &%s::get_%s() const { return __data.%s; };\n",
                        i.value, block.name, i.key, i.key);
                }
            }
            // to binary
            fmt.print("namespace CppClass {\n");
            fmt.print(
                "void __appendBinary(Bytes &__res, const %s &__object) {\n",
                block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print(
                    "__appendBinary(__res, uint32_t(__object.__tag));\n");
                fmt.print("switch (__object.__tag) {\n");
                {
                    auto guard = fmt.indent_guard();
                    for (auto i : block.elements) {
                        if (!i.value.empty()) {
                            fmt.print("case %s::__Tag::%s:\n", block.name,
                                      i.key);
                            {
                                auto guard = fmt.indent_guard();
                                fmt.print(
                                    "__appendBinary(__res, __object.__data.%s);\n",
                                    i.key);
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
            fmt.print("}\n");
        } else if (block.type == BlockType::Class) {
            std::vector<int> a;
        }
    }
    return fmt.recv.data;
}
}  // namespace Compiler