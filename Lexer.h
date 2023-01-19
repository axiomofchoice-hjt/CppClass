#pragma once

#include <string>
#include <vector>

namespace Compiler {
class Word {
   public:
    std::string value;

    Word(const std::string &value) : value(value) {}
    bool isName() const;
};

std::vector<Word> Lexer(const std::string &s);
}  // namespace Compiler