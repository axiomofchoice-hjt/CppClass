#include "Parser.h"

void Block::parseEnumContent(std::vector<Word>::const_iterator l,
                             std::vector<Word>::const_iterator r) {
    auto word = l;
    while (word < r) {
        if (!word->isName()) {
            throw "enum element is not allowed";
        }
        elements.emplace_back(word->value);
        word++;
        if (word->value == "(" && word[1].value == ")") {
            word += 2;
        }
        if (word->value == "(" && word[1].isName() && word[2].value == ")") {
            elements.back().value = word[1].value;
            word += 3;
        }
        if (word == r) {
            continue;
        }
        if (word->value != ",") {
            throw "enum element need `,`";
        }
        word++;
    }
}

void Block::parseClassContent(std::vector<Word>::const_iterator l,
                              std::vector<Word>::const_iterator r) {
    auto word = l;
    while (word < r) {
        if (!word->isName()) {
            throw "class element name is not allowed";
        }
        if ((word + 1)->value != ":") {
            throw "class element need `:`";
        }
        if (!(word + 2)->isName()) {
            throw "class element type is not allowed";
        }
        elements.emplace_back(word->value, (word + 2)->value);
        word += 3;
        if (word == r) {
            continue;
        }
        if (word->value != ",") {
            throw "enum element need `,`";
        }
        word++;
    }
}

std::vector<Block> Parser(const std::vector<Word> &words) {
    auto word = words.begin();
    std::vector<Block> res;
    while (true) {
        Block block;
        if (word == words.end()) {
            break;
        }

        if (word->value == "enum") {
            block.type = BlockType::Enum;
        } else if (word->value == "class") {
            block.type = BlockType::Class;
        } else {
            throw "block should begin with `enum` or `class`";
        }
        word++;
        if (!word->isName()) {
            throw "block need a name";
        }
        block.name = word->value;
        word++;
        if (word->value != "{") {
            throw "block need `{`";
        }
        word++;
        auto rbrace = word;
        while (rbrace != words.end() && rbrace->value != "}") {
            rbrace++;
        }
        if (rbrace == words.end()) {
            throw "block need `}`";
        }
        if (block.type == BlockType::Enum) {
            block.parseEnumContent(word, rbrace);
        } else if (block.type == BlockType::Class) {
            block.parseClassContent(word, rbrace);
        }
        res.push_back(block);
        word = rbrace + 1;
    }
    return res;
}