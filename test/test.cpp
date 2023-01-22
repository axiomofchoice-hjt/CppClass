#include <cstdio>
#include <utility>

#include "Def.h"

#define assert(state)                                            \
    do {                                                         \
        if (!(state)) {                                          \
            printf("assert fail: line=%d\n", __LINE__);          \
            throw;                                               \
        } else {                                                 \
            if (0) printf("assert ok: line=%d\n", __LINE__); \
        }                                                        \
    } while (false)

void enum_test() {
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

    printf("%s\n", CppClass::toJson(Result()).c_str());
    printf("%s\n", CppClass::toJson(Result::Ok()).c_str());
    printf("%s\n", CppClass::toJson(Result::Err(233)).c_str());

    std::string str = "  {  }  ";
    assert(CppClass::fromJson<Result>(str) == nullptr);
    str = "  {  \"Ok\"  :  null  }  ";
    assert(CppClass::fromJson<Result>(str).is_Ok());
    str = "  {  \"Err\"  :  233  }  ";
    assert(CppClass::fromJson<Result>(str).is_Err());
    assert(CppClass::fromJson<Result>(str).get_Err() == 233);
    str = "{\"Err\":-111111111}";
    assert(CppClass::fromJson<Result>(str).is_Err());
    assert(CppClass::fromJson<Result>(str).get_Err() == -111111111);
}

void class_test() {
    std::vector<uint8_t> bin;
    {
        User user;
        user.id = 1;
        user.follows.push_back(2);
        user.follows.push_back(3);
        user.follows.push_back(4);
        user.state = Result::Err(233);
        bin = CppClass::toBinary(user);
        for (auto i : bin) {
            printf("%u ", i);
        }
        puts("");
    }
    {
        User user = CppClass::fromBinary<User>(bin);
        assert(user.id == 1);
        assert(user.follows.size() == 3);
        assert(user.follows[0] == 2);
        assert(user.follows[1] == 3);
        assert(user.follows[2] == 4);
        assert(user.state.is_Err());
        assert(user.state.get_Err() == 233);

        printf("%s\n", CppClass::toJson(user).c_str());
    }
    {
        User user = CppClass::fromJson<User>(
            "  {  "
            "\"id\"  :  1  ,  "
            "\"follows\"  :  [  2  ,  3  ,  4  ]  ,  "
            "\"state\"  :  {  \"Err\"  :  233  }  "
            "}  ");
        assert(user.id == 1);
        assert(user.follows.size() == 3);
        assert(user.follows[0] == 2);
        assert(user.follows[1] == 3);
        assert(user.follows[2] == 4);
        assert(user.state.is_Err());
        assert(user.state.get_Err() == 233);
    }
    {
        User user = CppClass::fromJson<User>(
            "{"
            "\"id\":21,"
            "\"follows\":[],"
            "\"state\":{}"
            "}");
        assert(user.id == 21);
        assert(user.follows.size() == 0);
        assert(user.state == nullptr);
    }
}

int main() {
    try {
        enum_test();
        class_test();
    } catch (const char* error) {
        printf("error: %s\n", error);
    }
    return 0;
}