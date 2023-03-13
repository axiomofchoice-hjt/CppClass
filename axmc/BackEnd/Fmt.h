#pragma once
#include <fmt/core.h>

#include <string>
#include <utility>

class Fmt {
   public:
    class Recv {
       public:
        std::string data;

        void append(const std::string &arg) { data.append(arg); }

        template <class... Args>
        void append_format(Args &&...args) {
            data.append(fmt::format(std::move(args)...));
        }
    };

    Recv recv;
    size_t indent;
    Fmt() : recv(), indent() {}

    template <class... Args>
    void print_without_indent(const char *s, Args... args) {
        recv.append_format(s, args...);
    }

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
    void print_private() {
        recv.append(std::string(indent - 1, ' '));
        recv.append("private:\n");
    }
};