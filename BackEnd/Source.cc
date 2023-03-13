#include "../BuiltinTypes.h"
#include "../Fmt.h"
#include "BackEnd.h"

namespace BackEnd {
void CodeGenerator::__enum_source(Fmt &fmt, const FrontEnd::Block &block) {
    bool use_union = block.isComplexEnum();
    // constructor
    Recv t;
    t.append_format("__tag(__Tag::__UNDEF)");
    if (use_union) {
        t.append_format(", __data()");
    }
    fmt.print("%s::%s() : %s {}\n", block.name, block.name, t.data);
    fmt.print("%s::%s(%s &&other) : %s { __assign(std::move(other)); }\n",
              block.name, block.name, block.name, t.data);
    fmt.print("%s::%s(const %s &other) : %s { __assign(other); }\n", block.name,
              block.name, block.name, t.data);

    // deconstructor
    fmt.print("%s::~%s() { __del(); }\n", block.name, block.name);
    // assign operator
    fmt.print("%s &%s::operator=(%s &&other) {\n", block.name, block.name,
              block.name);
    fmt.print("    return __assign(std::move(other));\n");
    fmt.print("}\n");
    fmt.print("%s &%s::operator=(const %s &other) {\n", block.name, block.name,
              block.name);
    fmt.print("    return __assign(other);\n");
    fmt.print("}\n");
    fmt.print("%s &%s::operator=(std::nullptr_t) {\n", block.name, block.name);
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
            "void %s::__Data::__assign(__Tag __tag, const __Data &other) {\n",
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
                            fmt.print("this->%s = other.%s;\n", i.key, i.key);
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

        fmt.print("void %s::__Data::__del(__Tag __tag) {\n", block.name);
        {
            auto guard = fmt.indent_guard();
            fmt.print("switch (__tag) {\n");
            {
                auto guard = fmt.indent_guard();
                for (auto i : block.elements) {
                    if (!i.value.empty() && !no_deconstructor.count(i.value)) {
                        fmt.print("case __Tag::%s:\n", i.key);
                        {
                            auto guard = fmt.indent_guard();
                            fmt.print("this->%s.~%s();\n", i.key, i.value);
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
            fmt.print("const %s %s::%s() {\n", block.name, block.name, i.key);
            fmt.print("    %s res;\n", block.name);
            fmt.print("    res.__tag = __Tag::%s;\n", i.key);
            fmt.print("    return res;\n");
            fmt.print("}\n");
        } else {
            fmt.print("const %s %s::%s(%s value) {\n", block.name, block.name,
                      i.key, i.value);
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
            fmt.print("const %s &%s::get_%s() const { return __data.%s; };\n",
                      i.value, block.name, i.key, i.key);
            fmt.print("%s &%s::get_%s() { return __data.%s; };\n", i.value,
                      block.name, i.key, i.key);
        }
    }
    // to binary
    fmt.print("namespace AxMarshal {\n");
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
                    fmt.print("case %s::__Tag::%s:\n", block.name, i.key);
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("__toBinary(__res, __object.__data.%s);\n",
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
    fmt.print("void __fromBinary(Iter &__it, %s &__object) {\n", block.name);
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
                    fmt.print("case %s::__Tag::%s:\n", block.name, i.key);
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("__fromBinary(__it, __object.__data.%s);\n",
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
    fmt.print("void __toJson(Str &__res, const %s &__object) {\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__res += \"{\";\n");
        fmt.print("switch (__object.__tag) {\n");
        {
            auto guard = fmt.indent_guard();
            for (auto i : block.elements) {
                fmt.print("case %s::__Tag::%s:\n", block.name, i.key);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("__res += \"\\\"%s\\\":\";\n", i.key);
                    if (i.value.empty()) {
                        fmt.print("__res += \"null\";\n");
                    } else {
                        fmt.print("__toJson(__res, __object.__data.%s);\n",
                                  i.key);
                    }
                }
                fmt.print("break;\n");
            }
            fmt.print("default:\n");
            fmt.print("    break;\n");
        }
        fmt.print("}\n");
        fmt.print("__res += \"}\";\n");
    }
    fmt.print("}\n");
    fmt.print("void __fromJson(Iter &__it, %s &__object) {\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__jsonGetFirstChar(__it);\n");
        fmt.print("std::string __tmp = __jsonGetFirstKey(__it);\n");
        fmt.print("__object.__del();\n");
        fmt.print("if (__tmp.empty()) {\n");
        fmt.print("}");
        for (auto i : block.elements) {
            fmt.print_without_indent(" else if (__tmp == \"%s\") {\n", i.key);
            fmt.print("    __object.__tag = %s::__Tag::%s;\n", block.name,
                      i.key);
            if (i.value.empty()) {
                fmt.print("    __jsonIgnoreSpace(__it);\n");
                fmt.print("    __it += 4;\n");
            } else {
                fmt.print("    __fromJson(__it, __object.__data.%s);\n", i.key);
            }
            fmt.print("}");
        }
        fmt.print("\n");
        fmt.print("__jsonGetFirstChar(__it);\n");
    }
    fmt.print("}\n");
    fmt.print("}\n");
    fmt.print("}\n");
}
void CodeGenerator::__class_source(Fmt &fmt, const FrontEnd::Block &block) {
    fmt.print("namespace AxMarshal {\n");
    fmt.print("namespace Bin {\n");
    fmt.print("void __toBinary(Bytes &__res, const %s &__object) {\n",
              block.name);
    {
        auto guard = fmt.indent_guard();
        for (auto i : block.elements) {
            if (!i.isList) {
                fmt.print("__toBinary(__res, __object.%s);\n", i.key);
            } else {
                fmt.print("__toBinary(__res, (uint32_t)__object.%s.size());\n",
                          i.key);
                fmt.print("for (const auto &__i : __object.%s) {\n", i.key);
                fmt.print("    __toBinary(__res, __i);\n");
                fmt.print("}\n");
            }
        }
    }
    fmt.print("}\n");
    fmt.print("void __fromBinary(Iter &__it, %s &__object) {\n", block.name);
    {
        auto guard = fmt.indent_guard();
        for (auto i : block.elements) {
            if (!i.isList) {
                fmt.print("__fromBinary(__it, __object.%s);\n", i.key);
            } else {
                fmt.print("uint32_t __size;\n");
                fmt.print("__fromBinary(__it, __size);\n");
                fmt.print("__object.%s.assign(__size, %s());\n", i.key,
                          i.value);
                fmt.print("for (auto &__i : __object.%s) {\n", i.key);
                fmt.print("    __fromBinary(__it, __i);\n");
                fmt.print("}\n");
            }
        }
    }
    fmt.print("}\n");
    fmt.print("}\n");

    fmt.print("namespace Json {\n");
    fmt.print("void __toJson(Str &__res, const %s &__object) {\n", block.name);
    {
        auto guard = fmt.indent_guard();
        bool flag = false;

        fmt.print("__res += '{';\n");
        for (const auto &i : block.elements) {
            if (flag) {
                fmt.print("__res += ',';\n");
            }
            flag = true;
            fmt.print("__res += \"\\\"%s\\\":\";\n", i.key);
            if (!i.isList) {
                fmt.print("__toJson(__res, __object.%s);\n", i.key);
            } else {
                fmt.print("__res += '[';\n");
                fmt.print("bool __flag = false;\n");
                fmt.print("for (const auto &__i : __object.%s) {\n", i.key);
                fmt.print("    if (__flag) { __res += ','; }\n");
                fmt.print("    __flag = true;\n");
                fmt.print("    __toJson(__res, __i);\n");
                fmt.print("}\n");
                fmt.print("__res += ']';\n");
            }
        }
        fmt.print("__res += '}';\n");
    }
    fmt.print("}\n");
    fmt.print("void __fromJson(Iter &__it, %s &__object) {\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__jsonGetFirstChar(__it);\n");
        fmt.print("while (true) {\n");
        {
            auto guard = fmt.indent_guard();
            fmt.print("std::string __key = __jsonGetFirstKey(__it);\n");
            fmt.print("if (__key.empty()) { break; }\n");
            for (auto i : block.elements) {
                fmt.print("if (__key == \"%s\") {\n", i.key);
                if (!i.isList) {
                    auto guard = fmt.indent_guard();
                    fmt.print("__fromJson(__it, __object.%s);\n", i.key);
                } else {
                    auto guard = fmt.indent_guard();
                    fmt.print("__jsonGetFirstChar(__it);\n");  // `[`
                    fmt.print("while (__jsonGetFirstChar(__it) != ']') {\n");
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("--__it;\n");
                        fmt.print("__object.%s.push_back(%s());\n", i.key,
                                  i.value);
                        fmt.print("__fromJson(__it, __object.%s.back());\n",
                                  i.key);
                        fmt.print(
                            "if (__jsonGetFirstChar(__it) != ',') { --__it; "
                            "}\n");
                    }
                    fmt.print("}\n");
                }
                fmt.print("}\n");
            }
            fmt.print("if (__jsonGetFirstChar(__it) == '}') { --__it; }\n");
        }
        fmt.print("}\n");
        fmt.print("__jsonGetFirstChar(__it);\n");
    }
    fmt.print("}\n");
    fmt.print("}\n");
    fmt.print("}\n");
}

std::string CodeGenerator::source(const std::string &baseName) {
    Fmt fmt;
    fmt.print("#include \"%s.h\"\n\n", baseName);
    fmt.print("#include <utility>\n\n");
    for (auto block : blocks) {
        if (block.type == FrontEnd::Block::Type::Enum) {
            __enum_source(fmt, block);
        } else if (block.type == FrontEnd::Block::Type::Class) {
            __class_source(fmt, block);
        }
    }
    return fmt.recv.data;
}
}  // namespace BackEnd