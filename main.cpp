#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

#include "CodeGenerator.h"
#include "Lexer.h"
#include "Parser.h"

void work(std::string s, FILE *f) {
    auto words = Lexer(s);
    auto blocks = Parser(words);
    CodeGenerator gen;
    s = gen.header(blocks);
    fprintf(f, "%s", s.c_str());
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