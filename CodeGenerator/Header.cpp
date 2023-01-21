#include "../CodeGenerator.h"
#include "../Fmt.h"

namespace Compiler {
std::string CodeGenerator::header() {
    Fmt fmt;
    fmt.print("#pragma once\n\n");
    fmt.print("#include <string>\n\n");
    fmt.print("#include \"../CppClass.h\"\n\n");
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            bool use_union = block.isComplexEnum();
            fmt.print("class %s;\n", block.name);
            fmt.print("namespace CppClass{\n");
            fmt.print("namespace Bin {\n");
            fmt.print("void __toBinary(Bytes &, const %s &);\n", block.name);
            fmt.print("void __fromBinary(Iter &, %s &);\n", block.name);
            fmt.print("}\n");
            fmt.print("namespace Json {\n");
            fmt.print("void __toJson(Str &, const %s &);\n", block.name);
            fmt.print("void __fromJson(Iter &, %s &);\n", block.name);
            fmt.print("}\n");
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
                    "friend void "
                    "CppClass::Bin::__toBinary(CppClass::"
                    "Bin::Bytes &, "
                    "const %s &);\n",
                    block.name);
                fmt.print(
                    "friend void "
                    "CppClass::Bin::__fromBinary(CppClass::"
                    "Bin::Iter &, "
                    "%s &);\n",
                    block.name);
                // to json
                fmt.print(
                    "friend void CppClass::Json::__toJson(CppClass::Json::Str "
                    "&, const %s &);\n",
                    block.name);
                fmt.print(
                    "friend void "
                    "CppClass::Json::__fromJson(CppClass::Json::Iter &, %s "
                    "&);\n",
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
}  // namespace Compiler