#include <cstdio>
#include <utility>

#include "Def.h"

#define assert(state)                                   \
    do {                                                \
        if (!(state)) {                                 \
            printf("assert fail: line=%d\n", __LINE__); \
            throw;                                      \
        }                                               \
    } while (false)

int main() {
    assert(Message::Start() != Message::Reload());
    assert(Message::Start() == Message::Start());
    assert(!(Message::Start() != Message::Start()));
    printf("sizeof SEnum = %zu\n", sizeof(Message));
    printf("sizeof CEnum = %zu\n", sizeof(Result));
    Message a;
    assert(a.__tag == Message::__Tag::__UNDEF);
    a = Message::Start();
    assert(a == Message::Start());
    Message b = Message::Reload();
    a = b;
    assert(a == Message::Reload());
    assert(b == Message::Reload());
    a = Message::Start();
    a = std::move(b);
    assert(a == Message::Reload());
    assert(b == Message());
    return 0;
}