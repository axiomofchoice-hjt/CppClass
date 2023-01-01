#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

#include "Lexer.h"
#include "Parser.h"

void work(std::string s, FILE *f) {
    auto words = Lexer(s);
    auto blocks = Parser(words);
    for (auto block : blocks) {
        if (block.type == BlockType::Enum) {
            fprintf(f, "class %s {\n", block.name.c_str());
            fprintf(f, "   public:\n");
            fprintf(f, "    enum class __TYPE {\n");
            for (auto i : block.elements) {
                fprintf(f, "        %s,\n", i.key.c_str());
            }
            fprintf(f, "    };\n");
            fprintf(f, "    __TYPE __type;\n");
            fprintf(f, "    %s(__TYPE __type) : __type(__type) {}\n",
                    block.name.c_str());
            for (auto i : block.elements) {
                fprintf(f, "    static const %s %s;\n", block.name.c_str(),
                        i.key.c_str());
            }
            fprintf(f,
                    "    bool operator==(%s other) const { return __type == "
                    "other.__type; }\n",
                    block.name.c_str());
            fprintf(f,
                    "    bool operator!=(%s other) const { return __type != "
                    "other.__type; }\n",
                    block.name.c_str());
            fprintf(f, "};\n");
            for (auto i : block.elements) {
                fprintf(f, "const %s %s::%s = %s(%s::__TYPE::%s);\n",
                        block.name.c_str(), block.name.c_str(), i.key.c_str(),
                        block.name.c_str(), block.name.c_str(), i.key.c_str());
            }
        } else if (block.type == BlockType::Class) {
            fprintf(f, "class %s {\n", block.name.c_str());
            fprintf(f, "   public:\n");
            for (auto i : block.elements) {
                fprintf(f, "    %s %s;\n", i.value.c_str(), i.key.c_str());
            }
            fprintf(f, "};\n");
        }
    }
}

int main() {
    freopen("../test.txt", "r", stdin);
    FILE *f = fopen("../output.txt", "w");
    std::string line;
    std::string input;
    while (std::getline(std::cin, line)) {
        input += line;
        input += '\n';
    }
    try {
        work(input, f);
    } catch (const char *error) {
        printf("error: %s\n", error);
    }
    return 0;
}