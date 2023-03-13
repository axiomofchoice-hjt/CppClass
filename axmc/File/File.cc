#include "File.h"

#include <cstdio>

File::File(const std::string &path) : path(path) {}

std::string File::read() const {
    FILE *f = fopen(path.c_str(), "r");
    if (f == nullptr) {
        throw "read file failed";
    }
    std::string res;
    while (true) {
        char c = fgetc(f);
        if (c == EOF) {
            break;
        }
        res.push_back(c);
    }
    fclose(f);
    return res;
}

void File::write(const std::string &content) const {
    FILE *f = fopen(path.c_str(), "w");
    if (f == nullptr) {
        throw "write file failed";
    }
    fprintf(f, "%s", content.c_str());
    fclose(f);
}

std::string File::typeName() const {
    return path.substr(path.rfind('.') + 1, std::string::npos);
}

std::string File::baseName() const {
    size_t pos = 0;
    for (size_t i = 0; i < path.length(); i++) {
        if (path[i] == '/' || path[i] == '\\') {
            pos = i + 1;
        }
    }
    return path.substr(pos, path.rfind('.') - pos);
}