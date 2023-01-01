#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

#include "Word.h"

enum class BlockType {
    Enum,
    Class,
};

std::vector<std::string> EnumContent(const char *&s) {
    std::vector<std::string> res;
    std::string word = nextWord(s);
    if (word != "{") {
        throw "enum need `{`";
    }
    while (true) {
        word = nextWord(s);
        if (word == "}") {
            break;
        } else if (isName(word)) {
            res.push_back(std::move(word));
            std::string t = nextWord(s);
            if (t == "}") {
                break;
            }
            if (t != ",") {
                throw "enum element need `,`";
            }
        } else {
            throw "enum element is not allowed";
        }
    }
    return res;
}

class ClassElement {
   public:
    std::string key;
    std::string value;
};

std::vector<ClassElement> ClassContent(const char *&s) {
    std::vector<ClassElement> res;
    if (nextWord(s) != "{") {
        throw "class need `{`";
    }
    while (true) {
        std::string key = nextWord(s);
        if (key == "}") {
            break;
        } else if (isName(key)) {
            if (nextWord(s) != ":") {
                throw "class element need `:`";
            }
            std::string value = nextWord(s);
            if (isName(value)) {
                res.push_back({std::move(key), std::move(value)});
            } else {
                throw "class element type is not allowed";
            }
            std::string t = nextWord(s);
            if (t == "}") {
                break;
            }
            if (t != ",") {
                throw "class element need `,`";
            }
        } else {
            throw "class element name is not allowed";
        }
    }
    return res;
}

void work(const char *s, FILE *f) {
    while (true) {
        BlockType blockType;
        std::string word = nextWord(s);
        if (word == "enum") {
            blockType = BlockType::Enum;
        } else if (word == "class") {
            blockType = BlockType::Class;
        } else if (word.empty()) {
            break;
        } else {
            throw "block should begin with `enum` or `class`";
        }
        word = nextWord(s);
        if (!isName(word)) {
            throw "block need a name";
        }
        std::string blockName = word;
        if (blockType == BlockType::Enum) {
            auto content = EnumContent(s);
            fprintf(f, "enum class %s {\n", blockName.c_str());
            for (auto i : content) {
                fprintf(f, "    %s,\n", i.c_str());
            }
            fprintf(f, "};\n");
        } else if (blockType == BlockType::Class) {
            auto content = ClassContent(s);
            fprintf(f, "class %s {\n", blockName.c_str());
            fprintf(f, "   public:\n");
            for (auto i : content) {
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
        work(input.c_str(), f);
    } catch (const char *error) {
        printf("error: %s\n", error);
    }
    return 0;
}