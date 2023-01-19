#include "CodeGenerator.h"

#include <cstring>
#include <initializer_list>

CodeGenerator::CodeGenerator() : indent(4) {}

void string_append(std::string &res, const std::string &s) { res += s; }

template <class T>
void string_append(std::string &res, const T &s) {
    res += std::to_string(s);
}

template <class T>
void format_one(std::string &res, const char *&s, const T &arg) {
    for (; *s != 0; s++) {
        if (*s == '%' && s[1] == 's') {
            break;
        } else {
            res.push_back(*s);
        }
    }
    if (*s == 0) {
        throw "error: in format, `%s` not found";
    }
    string_append(res, arg);
    s += 2;
}

void format(std::string &res, const char *s) { res += s; }

template <class T, class... Args>
void format(std::string &res, const char *s, const T &arg, Args... rest) {
    format_one(res, s, arg);
    format(res, s, rest...);
}

std::string CodeGenerator::header(const std::vector<Block> &blocks) {
    std::string res;
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            format(res, "class %s {\n", block.name);
            format(res, "   public:\n");
            format(res, "    enum class Tag {\n");
            for (auto i : block.elements) {
                format(res, "        %s,\n", i.key);
            }
            format(res, "    };\n");
            format(res, "    Tag tag;\n");
            format(res, "    explicit %s(Tag tag) : tag(tag) {}\n", block.name);
            for (auto i : block.elements) {
                format(res, "    static const %s %s(%s);\n", block.name, i.key,
                       i.value);
            }
            format(res,
                   "    bool operator==(%s other) const { return __value == "
                   "other.__value; }\n",
                   block.name);
            format(res,
                   "    bool operator!=(%s other) const { return __value != "
                   "other.__value; }\n",
                   block.name);
            format(res, "};\n");
        } else if (block.type == BlockType::Class) {
            format(res, "class %s {\n", block.name);
            format(res, "   public:\n");
            for (auto i : block.elements) {
                format(res, "    %s %s;\n", i.value, i.key);
            }
            format(res, "};\n");
        }
    }
    return res;
}