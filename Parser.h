#pragma once

#include <string>
#include <vector>

#include "Lexer.h"

namespace Compiler {
class Element {
   public:
    std::string key;
    std::string value;
    bool isList;
    explicit Element(const std::string &key);
    Element(const std::string &key, const std::string &value);
};

enum class BlockType {
    Enum,
    Class,
};

class Block {
   public:
    BlockType type;
    std::string name;
    std::vector<Element> elements;

    void parseEnumContent(std::vector<Word>::const_iterator l,
                          std::vector<Word>::const_iterator r);
    void parseClassContent(std::vector<Word>::const_iterator l,
                           std::vector<Word>::const_iterator r);
    bool isComplexEnum() const;
};

std::vector<Block> Parser(const std::vector<Word> &words);
}  // namespace Compiler