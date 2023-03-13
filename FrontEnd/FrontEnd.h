#pragma once

#include <string>
#include <vector>

namespace FrontEnd {
class Word {
   public:
    std::string value;

    Word(const std::string &value) : value(value) {}
    bool isName() const;
};

std::vector<Word> Lexer(const std::string &s);

class Element {
   public:
    std::string key;
    std::string value;
    bool isList;
    explicit Element(const std::string &key);
    Element(const std::string &key, const std::string &value);
};

class Block {
   public:
    enum class Type {
        Enum,
        Class,
    };

    Type type;
    std::string name;
    std::vector<Element> elements;

    void parseEnumContent(std::vector<Word>::const_iterator l,
                          std::vector<Word>::const_iterator r);
    void parseClassContent(std::vector<Word>::const_iterator l,
                           std::vector<Word>::const_iterator r);
    bool isComplexEnum() const;
};

std::vector<Block> Parser(const std::vector<Word> &words);
}  // namespace FrontEnd