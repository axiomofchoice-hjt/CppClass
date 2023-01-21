#include "Fmt.h"

const char *find_placeholder(const char *s) {
    for (; *s != 0; s++) {
        if (*s == '%' && (s[1] == 's' || s[1] == 'd')) {
            return s;
        }
    }
    throw "placeholder not found";
}

void Recv::append(const std::string &s) { data += s; }
void Recv::append(const char *s) { data += s; }
void Recv::append_format(const char *s) { append(s); }
