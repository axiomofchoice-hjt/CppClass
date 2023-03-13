#include <cstdio>
#include <exception>
#include <iostream>
#include <vector>

#include "Compiler.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: %s [source file] [target dir]\n", argv[0]);
        return 0;
    }
    try {
        Compiler::Compile(argv[1], argv[2]);
        puts("ok");
    } catch (const char *error) {
        printf("error: %s\n", error);
    }
    return 0;
}