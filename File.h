#pragma once

#include <cstdio>
#include <string>

class File {
   public:
    std::string path;
    explicit File(const std::string &path);
    std::string read() const;
    void write(const std::string &content) const;

    std::string typeName() const;
    std::string baseName() const;
};