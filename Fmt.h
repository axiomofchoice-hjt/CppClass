#pragma once
#include <string>

const char *find_placeholder(const char *s) {
    for (; *s != 0; s++) {
        if (*s == '%' && (s[1] == 's' || s[1] == 'd')) {
            return s;
        }
    }
    throw "placeholder not found";
}

class Recv {
   public:
    std::string data;
    void append(const std::string &s) { data += s; }
    void append(const char *s) { data += s; }
    template <class T>
    void append(const T &s) {
        data += std::to_string(s);
    }
    void append_format(const char *s) { append(s); }

    template <class T, class... Args>
    void append_format(const char *s, const T &arg, Args... rest) {
        const char *t = find_placeholder(s);
        append(std::string(s, t));
        append(arg);
        append_format(t + 2, rest...);
    }
};

class Fmt {
   public:
    Recv recv;
    size_t indent;
    Fmt() : recv(), indent() {}

    template <class... Args>
    void print(const char *s, Args... args) {
        recv.append(std::string(indent, ' '));
        recv.append_format(s, args...);
    }

    class IndentGuard {
       public:
        Fmt &fmt;
        IndentGuard(Fmt &fmt) : fmt(fmt) { fmt.indent += 4; }
        ~IndentGuard() { fmt.indent -= 4; }
    };

    IndentGuard indent_guard() { return IndentGuard(*this); }

    void print_public() {
        recv.append(std::string(indent - 1, ' '));
        recv.append("public:\n");
    }
};