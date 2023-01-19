#include "Lexer.h"

#include <cctype>
#include <functional>

namespace Compiler {
static const char *match(const char *s, const std::function<bool(char)> &f) {
    while (f(*s)) {
        s++;
    }
    return s;
}

static bool isNameChar(char c) { return isalnum(c) || c == '_' || c == '$'; }
static bool isNameStartChar(char c) {
    return isalpha(c) || c == '_' || c == '$';
}

std::string nextWord(const char *&s) {
    s = match(s, isspace);
    std::string res;
    if (*s == '\0') {
        res.clear();
    } else if (isNameChar(*s)) {
        const char *t = match(s, isNameChar);
        res.assign(s, t);
        s = t;
    } else {
        const char *t = s + 1;
        res.assign(s, t);
        s = t;
    }
    return res;
}

bool Word::isName() const {
    return !value.empty() && isNameStartChar(value[0]);
}

std::vector<Word> Lexer(const std::string &s) {
    std::vector<Word> res;
    const char *ptr = s.c_str();
    while (true) {
        std::string word = nextWord(ptr);
        if (word.empty()) {
            break;
        }
        res.emplace_back(word);
    }
    return res;
}
}  // namespace Compiler