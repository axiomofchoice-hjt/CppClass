#include "BackEnd.h"
#include "BuiltinTypes.h"
#include "Fmt.h"

namespace BackEnd {
void CodeGenerator::__enum_source(Fmt &fmt, const FrontEnd::Block &block) {
    bool use_union = block.isComplexEnum();
    // constructor
    std::string t = "__tag(__Tag::__UNDEF)";
    if (use_union) {
        t.append(", __data()");
    }
    fmt.print("{}::{}() : {} {{}}\n", block.name, block.name, t);
    fmt.print("{}::{}({} &&other) : {} {{ __assign(std::move(other)); }}\n",
              block.name, block.name, block.name, t);
    fmt.print("{}::{}(const {} &other) : {} {{ __assign(other); }}\n",
              block.name, block.name, block.name, t);

    // deconstructor
    fmt.print("{}::~{}() {{ __del(); }}\n", block.name, block.name);
    // assign operator
    fmt.print("{} &{}::operator=({} &&other) {{\n", block.name, block.name,
              block.name);
    fmt.print("    return __assign(std::move(other));\n");
    fmt.print("}}\n");
    fmt.print("{} &{}::operator=(const {} &other) {{\n", block.name, block.name,
              block.name);
    fmt.print("    return __assign(other);\n");
    fmt.print("}}\n");
    fmt.print("{} &{}::operator=(std::nullptr_t) {{\n", block.name, block.name);
    fmt.print("    return __assign(nullptr);\n");
    fmt.print("}}\n");

    // union
    if (use_union) {
        fmt.print("{}::__Data::__Data() {{}}\n", block.name);
        fmt.print("{}::__Data::~__Data() {{}}\n", block.name);

        fmt.print(
            "void {}::__Data::__assign(__Tag __tag, __Data &&other) "
            "{{\n",
            block.name);
        {
            auto guard = fmt.indent_guard();
            fmt.print("switch (__tag) {{\n");
            {
                auto guard = fmt.indent_guard();
                for (auto i : block.elements) {
                    if (!i.value.empty()) {
                        fmt.print("case __Tag::{}:\n", i.key);
                        {
                            auto guard = fmt.indent_guard();
                            fmt.print(
                                "this->{} = "
                                "std::move(other.{});\n",
                                i.key, i.key);
                            fmt.print("break;\n");
                        }
                    }
                }
                fmt.print("default:\n");
                fmt.print("    break;\n");
            }
            fmt.print("}}\n");
        }
        fmt.print("}}\n");

        fmt.print(
            "void {}::__Data::__assign(__Tag __tag, const __Data &other) {{\n",
            block.name);
        {
            auto guard = fmt.indent_guard();
            fmt.print("switch (__tag) {{\n");
            {
                auto guard = fmt.indent_guard();
                for (auto i : block.elements) {
                    if (!i.value.empty()) {
                        fmt.print("case __Tag::{}:\n", i.key);
                        {
                            auto guard = fmt.indent_guard();
                            fmt.print("this->{} = other.{};\n", i.key, i.key);
                            fmt.print("break;\n");
                        }
                    }
                }
                fmt.print("default:\n");
                fmt.print("    break;\n");
            }
            fmt.print("}}\n");
        }
        fmt.print("}}\n");

        fmt.print("void {}::__Data::__del(__Tag __tag) {{\n", block.name);
        {
            auto guard = fmt.indent_guard();
            fmt.print("switch (__tag) {{\n");
            {
                auto guard = fmt.indent_guard();
                for (auto i : block.elements) {
                    if (!i.value.empty() && !no_deconstructor(i.value)) {
                        fmt.print("case __Tag::{}:\n", i.key);
                        {
                            auto guard = fmt.indent_guard();
                            fmt.print("this->{}.~{}();\n", i.key, i.value);
                            fmt.print("break;\n");
                        }
                    }
                }
                fmt.print("default:\n");
                fmt.print("    break;\n");
            }
            fmt.print("}}\n");
        }
        fmt.print("}}\n");
    }
    // static construct functions
    for (auto i : block.elements) {
        if (i.value.empty()) {
            fmt.print("const {} {}::{}() {{\n", block.name, block.name, i.key);
            fmt.print("    {} res;\n", block.name);
            fmt.print("    res.__tag = __Tag::{};\n", i.key);
            fmt.print("    return res;\n");
            fmt.print("}}\n");
        } else {
            fmt.print("const {} {}::{}({} value) {{\n", block.name, block.name,
                      i.key, i.value);
            fmt.print("    {} res;\n", block.name);
            fmt.print("    res.__tag = __Tag::{};\n", i.key);
            fmt.print("    res.__data.{} = value;\n", i.key);
            fmt.print("    return res;\n");
            fmt.print("}}\n");
        }
    }
    // is
    for (auto i : block.elements) {
        fmt.print("bool {}::is_{}() const {{\n", block.name, i.key);
        fmt.print("    return __tag == __Tag::{};\n", i.key);
        fmt.print("}}\n");
    }
    // get
    for (auto i : block.elements) {
        if (!i.value.empty()) {
            fmt.print("const {} &{}::get_{}() const {{ return __data.{}; }};\n",
                      i.value, block.name, i.key, i.key);
            fmt.print("{} &{}::get_{}() {{ return __data.{}; }};\n", i.value,
                      block.name, i.key, i.key);
        }
    }
    // to binary
    fmt.print("namespace AxMarshal {{\n");
    fmt.print("namespace Bin {{\n");
    fmt.print("void __toBinary(Bytes &__res, const {} &__object) {{\n",
              block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__toBinary(__res, uint32_t(__object.__tag));\n");
        fmt.print("switch (__object.__tag) {{\n");
        {
            auto guard = fmt.indent_guard();
            for (auto i : block.elements) {
                if (!i.value.empty()) {
                    fmt.print("case {}::__Tag::{}:\n", block.name, i.key);
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("__toBinary(__res, __object.__data.{});\n",
                                  i.key);
                        fmt.print("break;\n");
                    }
                }
            }
            fmt.print("default:\n");
            fmt.print("    break;\n");
        }
        fmt.print("}}\n");
    }
    fmt.print("}}\n");
    fmt.print("void __fromBinary(Iter &__it, {} &__object) {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("uint32_t __tmp;\n");
        fmt.print("__fromBinary(__it, __tmp);\n");
        fmt.print("__object.__tag = ({}::__Tag)__tmp;\n", block.name);
        fmt.print("switch (__object.__tag) {{\n");
        {
            auto guard = fmt.indent_guard();
            for (auto i : block.elements) {
                if (!i.value.empty()) {
                    fmt.print("case {}::__Tag::{}:\n", block.name, i.key);
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("__fromBinary(__it, __object.__data.{});\n",
                                  i.key);
                        fmt.print("break;\n");
                    }
                }
            }
            fmt.print("default:\n");
            fmt.print("    break;\n");
        }
        fmt.print("}}\n");
    }
    fmt.print("}}\n");
    fmt.print("}}\n");

    fmt.print("namespace Json {{\n");
    fmt.print("void __toJson(Str &__res, const {} &__object) {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__res += \"{{\";\n");
        fmt.print("switch (__object.__tag) {{\n");
        {
            auto guard = fmt.indent_guard();
            for (auto i : block.elements) {
                fmt.print("case {}::__Tag::{}:\n", block.name, i.key);
                {
                    auto guard = fmt.indent_guard();
                    fmt.print("__res += \"\\\"{}\\\":\";\n", i.key);
                    if (i.value.empty()) {
                        fmt.print("__res += \"null\";\n");
                    } else {
                        fmt.print("__toJson(__res, __object.__data.{});\n",
                                  i.key);
                    }
                }
                fmt.print("break;\n");
            }
            fmt.print("default:\n");
            fmt.print("    break;\n");
        }
        fmt.print("}}\n");
        fmt.print("__res += \"}}\";\n");
    }
    fmt.print("}}\n");
    fmt.print("void __fromJson(Iter &__it, {} &__object) {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__jsonGetFirstChar(__it);\n");
        fmt.print("std::string __tmp = __jsonGetFirstKey(__it);\n");
        fmt.print("__object.__del();\n");
        fmt.print("if (__tmp.empty()) {{\n");
        fmt.print("}}");
        for (auto i : block.elements) {
            fmt.print_without_indent(" else if (__tmp == \"{}\") {{\n", i.key);
            fmt.print("    __object.__tag = {}::__Tag::{};\n", block.name,
                      i.key);
            if (i.value.empty()) {
                fmt.print("    __jsonIgnoreSpace(__it);\n");
                fmt.print("    __it += 4;\n");
            } else {
                fmt.print("    __fromJson(__it, __object.__data.{});\n", i.key);
            }
            fmt.print("}}");
        }
        fmt.print("\n");
        fmt.print("__jsonGetFirstChar(__it);\n");
    }
    fmt.print("}}\n");
    fmt.print("}}\n");
    fmt.print("}}\n");
}
void CodeGenerator::__class_source(Fmt &fmt, const FrontEnd::Block &block) {
    fmt.print("namespace AxMarshal {{\n");
    fmt.print("namespace Bin {{\n");
    fmt.print("void __toBinary(Bytes &__res, const {} &__object) {{\n",
              block.name);
    {
        auto guard = fmt.indent_guard();
        for (auto i : block.elements) {
            if (!i.isList) {
                fmt.print("__toBinary(__res, __object.{});\n", i.key);
            } else {
                fmt.print("__toBinary(__res, (uint32_t)__object.{}.size());\n",
                          i.key);
                fmt.print("for (const auto &__i : __object.{}) {{\n", i.key);
                fmt.print("    __toBinary(__res, __i);\n");
                fmt.print("}}\n");
            }
        }
    }
    fmt.print("}}\n");
    fmt.print("void __fromBinary(Iter &__it, {} &__object) {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        for (auto i : block.elements) {
            if (!i.isList) {
                fmt.print("__fromBinary(__it, __object.{});\n", i.key);
            } else {
                fmt.print("uint32_t __size;\n");
                fmt.print("__fromBinary(__it, __size);\n");
                fmt.print("__object.{}.assign(__size, {}());\n", i.key,
                          i.value);
                fmt.print("for (auto &__i : __object.{}) {{\n", i.key);
                fmt.print("    __fromBinary(__it, __i);\n");
                fmt.print("}}\n");
            }
        }
    }
    fmt.print("}}\n");
    fmt.print("}}\n");

    fmt.print("namespace Json {{\n");
    fmt.print("void __toJson(Str &__res, const {} &__object) {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        bool flag = false;

        fmt.print("__res += '{{';\n");
        for (const auto &i : block.elements) {
            if (flag) {
                fmt.print("__res += ',';\n");
            }
            flag = true;
            fmt.print("__res += \"\\\"{}\\\":\";\n", i.key);
            if (!i.isList) {
                fmt.print("__toJson(__res, __object.{});\n", i.key);
            } else {
                fmt.print("__res += '[';\n");
                fmt.print("bool __flag = false;\n");
                fmt.print("for (const auto &__i : __object.{}) {{\n", i.key);
                fmt.print("    if (__flag) {{ __res += ','; }}\n");
                fmt.print("    __flag = true;\n");
                fmt.print("    __toJson(__res, __i);\n");
                fmt.print("}}\n");
                fmt.print("__res += ']';\n");
            }
        }
        fmt.print("__res += '}}';\n");
    }
    fmt.print("}}\n");
    fmt.print("void __fromJson(Iter &__it, {} &__object) {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print("__jsonGetFirstChar(__it);\n");
        fmt.print("while (true) {{\n");
        {
            auto guard = fmt.indent_guard();
            fmt.print("std::string __key = __jsonGetFirstKey(__it);\n");
            fmt.print("if (__key.empty()) {{ break; }}\n");
            for (auto i : block.elements) {
                fmt.print("if (__key == \"{}\") {{\n", i.key);
                if (!i.isList) {
                    auto guard = fmt.indent_guard();
                    fmt.print("__fromJson(__it, __object.{});\n", i.key);
                } else {
                    auto guard = fmt.indent_guard();
                    fmt.print("__jsonGetFirstChar(__it);\n");  // `[`
                    fmt.print("while (__jsonGetFirstChar(__it) != ']') {{\n");
                    {
                        auto guard = fmt.indent_guard();
                        fmt.print("--__it;\n");
                        fmt.print("__object.{}.push_back({}());\n", i.key,
                                  i.value);
                        fmt.print("__fromJson(__it, __object.{}.back());\n",
                                  i.key);
                        fmt.print(
                            "if (__jsonGetFirstChar(__it) != ',') {{ --__it; "
                            "}}\n");
                    }
                    fmt.print("}}\n");
                }
                fmt.print("}}\n");
            }
            fmt.print("if (__jsonGetFirstChar(__it) == '}}') {{ --__it; }}\n");
        }
        fmt.print("}}\n");
        fmt.print("__jsonGetFirstChar(__it);\n");
    }
    fmt.print("}}\n");
    fmt.print("}}\n");
    fmt.print("}}\n");
}

std::string CodeGenerator::source(const std::string &baseName) {
    Fmt fmt;
    fmt.print("#include \"{}.h\"\n\n", baseName);
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