#include "../BuiltinTypes.h"
#include "../CodeGenerator.h"
#include "../Fmt.h"

namespace Compiler {
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
                    fmt.print("%s &%s::get_%s() { return __data.%s; };\n",
                              i.value, block.name, i.key, i.key);
                }
            }
            // to binary
            fmt.print("namespace CppClass {\n");
            fmt.print("namespace Bin {\n");
            fmt.print("void __toBinary(Bytes &__res, const %s &__object) {\n",
                      block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("__toBinary(__res, uint32_t(__object.__tag));\n");
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
                                    "__toBinary(__res, __object.__data.%s);\n",
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
            fmt.print("void __fromBinary(Iter &__it, %s &__object) {\n",
                      block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("uint32_t __tmp;\n");
                fmt.print("__fromBinary(__it, __tmp);\n");
                fmt.print("__object.__tag = (%s::__Tag)__tmp;\n", block.name);
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
                                    "__fromBinary(__it, __object.__data.%s);\n",
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

            fmt.print("namespace Json {\n");
            fmt.print("void __toJson(Str &__res, const %s &__object) {\n",
                      block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("__res += \"{\\\"%s.\";\n", block.name);
                fmt.print("switch (__object.__tag) {\n");
                {
                    auto guard = fmt.indent_guard();
                    for (auto i : block.elements) {
                        fmt.print("case %s::__Tag::%s:\n", block.name, i.key);
                        {
                            auto guard = fmt.indent_guard();
                            fmt.print("__res += \"%s\\\":\";\n", i.key);
                            if (i.value.empty()) {
                                fmt.print("__res += \"null\";\n");
                            } else {
                                fmt.print(
                                    "__toJson(__res, __object.__data.%s);\n",
                                    i.key);
                            }
                        }
                        fmt.print("break;\n");
                    }
                    fmt.print("default:\n");
                    fmt.print("    __res += \"#\\\":null\";\n");
                    fmt.print("    break;\n");
                }
                fmt.print("}\n");
                fmt.print("__res += \"}\";\n");
            }
            fmt.print("}\n");
            fmt.print("void __fromJson(Iter &__it, %s &__object) {\n",
                      block.name);
            {
                auto guard = fmt.indent_guard();
                fmt.print("std::string __tmp = __jsonEnumGet(__it);\n");
                fmt.print("__object.__del();\n");
                fmt.print("if (__tmp == \"#\") {\n");
                fmt.print("    __it += 4;\n");
                fmt.print("}");
                for (auto i : block.elements) {
                    fmt.print_without_indent(" else if (__tmp == \"%s\") {\n",
                                             i.key);
                    fmt.print("    __object.__tag = %s::__Tag::%s;\n",
                              block.name, i.key);
                    if (i.value.empty()) {
                        fmt.print("    __it += 4;\n");
                    } else {
                        fmt.print("    __fromJson(__it, __object.__data.%s);\n",
                                  i.key);
                    }
                    fmt.print("}");
                }
                fmt.print("\n");
                fmt.print("++__it;\n");
            }
            fmt.print("}\n");
            fmt.print("}\n");
            fmt.print("}\n");
        } else if (block.type == BlockType::Class) {
            std::vector<int> a;
        }
    }
    return fmt.recv.data;
}
}  // namespace Compiler