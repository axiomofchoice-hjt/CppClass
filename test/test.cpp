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
    assert(Calc::Add() != Calc::Sub());
    assert(Calc::Add() == Calc::Add());
    assert(!(Calc::Add() != Calc::Add()));
    printf("sizeof SEnum = %zu\n", sizeof(Calc));
    printf("sizeof CEnum = %zu\n", sizeof(Result));

    Calc a;
    assert(a == nullptr);
    assert(!(a != nullptr));
    assert(!a.is_Add());
    assert(!a.is_Sub());

    a = Calc::Add();
    assert(a != nullptr);
    assert(!(a == nullptr));
    assert(a.is_Add());
    assert(!a.is_Sub());

    Calc b = Calc::Sub();
    a = b;
    assert(a.is_Sub());
    assert(b.is_Sub());

    a = Calc::Add();
    a = std::move(b);
    assert(a.is_Sub());
    assert(b == nullptr);

    auto r = Result::Ok();
    assert(r != nullptr);
    assert(r.is_Ok());
    r = Result::Err(233);
    assert(r.is_Err());
    assert(r.get_Err() == 233);

    auto serial = CppClass::toBinary(r);
    for (auto i : serial) {
        printf("%u ", i);
    }
    puts("");
    auto r2 = CppClass::fromBinary<Result>(serial);
    assert(r2.is_Err());
    assert(r2.get_Err() == 233);

    return 0;
}