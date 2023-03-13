#pragma once

#include <string>

static bool no_deconstructor(const std::string &name) {
    return name == "int" || name == "float";
}
