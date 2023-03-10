#include "BackEnd.h"
#include "Fmt.h"

namespace BackEnd {
static void print_serialize_declare(Fmt &fmt, const FrontEnd::Block &block) {
    fmt.print("class {};\n", block.name);
    fmt.print("namespace AxMarshal {{\n");
    fmt.print("namespace Bin {{\n");
    fmt.print("void __toBinary(Bytes &, const {} &);\n", block.name);
    fmt.print("void __fromBinary(Iter &, {} &);\n", block.name);
    fmt.print("}}\n");
    fmt.print("namespace Json {{\n");
    fmt.print("void __toJson(Str &, const {} &);\n", block.name);
    fmt.print("void __fromJson(Iter &, {} &);\n", block.name);
    fmt.print("}}\n");
    fmt.print("}}\n");
}
static void print_serialize_friend(Fmt &fmt, const FrontEnd::Block &block) {
    // to binary
    fmt.print(
        "friend void AxMarshal::Bin::__toBinary(AxMarshal::Bin::Bytes &, const "
        "{} &);\n",
        block.name);
    fmt.print(
        "friend void AxMarshal::Bin::__fromBinary(AxMarshal::Bin::Iter &, {} "
        "&);\n",
        block.name);
    // to json
    fmt.print(
        "friend void AxMarshal::Json::__toJson(AxMarshal::Json::Str &, const "
        "{} "
        "&);\n",
        block.name);
    fmt.print(
        "friend void AxMarshal::Json::__fromJson(AxMarshal::Json::Iter &, {} "
        "&);\n",
        block.name);
}

void CodeGenerator::__enum_header(Fmt &fmt, const FrontEnd::Block &block) {
    bool use_union = block.isComplexEnum();
    print_serialize_declare(fmt, block);
    fmt.print("class {} : public AxMarshal::{}<{}> {{\n", block.name,
              use_union ? "ComplexEnum" : "SimpleEnum", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print_private();
        // enum class
        fmt.print("enum class __Tag {{\n");
        {
            auto guard = fmt.indent_guard();
            fmt.print("__UNDEF,\n");
            for (auto i : block.elements) {
                fmt.print("{},\n", i.key);
            }
        }
        fmt.print("}};\n");
        fmt.print("__Tag __tag;\n");
        // union class
        if (use_union) {
            fmt.print("union __Data {{\n");
            {
                auto guard = fmt.indent_guard();
                fmt.print("__Data();\n");
                fmt.print("~__Data();\n");
                fmt.print("void __assign(__Tag, __Data &&);\n");
                fmt.print("void __assign(__Tag, const __Data &);\n");
                fmt.print("void __del(__Tag tag);\n");
                fmt.print("struct {{}} __UNDEF;\n");
                for (auto i : block.elements) {
                    if (!i.value.empty()) {
                        fmt.print("{} {};\n", i.value, i.key);
                    }
                }
            }
            fmt.print("}};\n");
            fmt.print("__Data __data;\n");
        }
        fmt.print_public();
        // constructor
        fmt.print("{}();\n", block.name);
        fmt.print("{}({} &&);\n", block.name, block.name);
        fmt.print("{}(const {} &);\n", block.name, block.name);
        // deconstructor
        fmt.print("~{}();\n", block.name);
        // assign operator
        fmt.print("{} &operator=({} &&);\n", block.name, block.name);
        fmt.print("{} &operator=(const {} &);\n", block.name, block.name);
        fmt.print("{} &operator=(std::nullptr_t);\n", block.name);

        // static construct functions
        for (auto i : block.elements) {
            fmt.print("static const {} {}({});\n", block.name, i.key, i.value);
        }
        // is
        for (auto i : block.elements) {
            fmt.print("bool is_{}() const;\n", i.key);
        }
        // get
        for (auto i : block.elements) {
            if (!i.value.empty()) {
                fmt.print("const {} &get_{}() const;\n", i.value, i.key);
                fmt.print("{} &get_{}();\n", i.value, i.key);
            }
        }
        print_serialize_friend(fmt, block);
        // friend
        fmt.print("friend AxMarshal::{}<{}>;\n",
                  use_union ? "ComplexEnum" : "SimpleEnum", block.name);
    }
    fmt.print("}};\n");
}

void CodeGenerator::__class_header(Fmt &fmt, const FrontEnd::Block &block) {
    print_serialize_declare(fmt, block);
    fmt.print("class {} {{\n", block.name);
    {
        auto guard = fmt.indent_guard();
        fmt.print_public();
        for (auto i : block.elements) {
            if (i.isList) {
                fmt.print("std::vector<{}> {};\n", i.value, i.key);
            } else {
                fmt.print("{} {};\n", i.value, i.key);
            }
        }
        print_serialize_friend(fmt, block);
    }
    fmt.print("}};\n");
}

std::string CodeGenerator::header() {
    Fmt fmt;
    fmt.print("#pragma once\n\n");
    fmt.print("#include <string>\n");
    fmt.print("#include <vector>\n\n");
    fmt.print("#include \"axm/AxMarshal.h\"\n\n");
    for (const FrontEnd::Block &block : blocks) {
        if (block.type == FrontEnd::Block::Type::Enum) {
            __enum_header(fmt, block);
        } else if (block.type == FrontEnd::Block::Type::Class) {
            __class_header(fmt, block);
        }
    }
    return fmt.recv.data;
}
}  // namespace BackEnd