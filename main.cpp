#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

#include "Compiler.h"

int main() {
    try {
        Compiler::Compile("../def/Def.txt", "../def/");
    } catch (const char *error) {
        printf("error: %s\n", error);
    }
    return 0;
}