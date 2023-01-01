#pragma once

#include <string>
#include <vector>

#include "Lexer.h"

class Element {
   public:
    std::string key;
    std::string value;
    Element(std::string key, std::string value = "") : key(key), value(value) {}
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
};

std::vector<Block> Parser(const std::vector<Word> &words);