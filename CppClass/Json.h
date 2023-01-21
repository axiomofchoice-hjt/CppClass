#pragma once

#include <string>

namespace CppClass {
namespace Json {
using Str = std::string;
using Iter = Str::iterator;
void __toJson(Str &res, const uint32_t &data);
void __toJson(Str &res, const uint64_t &data);
void __toJson(Str &res, const int32_t &data);
void __toJson(Str &res, const int64_t &data);
template <typename T>
Str toJson(const T &__data) {
    Str __res;
    void __toJson(Str &, const T &);
    __toJson(__res, __data);
    return __res;
}

void __fromJson(Iter &it, uint32_t &data);
void __fromJson(Iter &it, uint64_t &data);
void __fromJson(Iter &it, int32_t &data);
void __fromJson(Iter &it, int64_t &data);
template <typename T>
T fromJson(const Str &__json) {
    T __res;
    Iter __tmp_iter = __json.cbegin();
    void __fromJson(Iter &, T &);
    __fromJson(__tmp_iter, __res);
    return __res;
}
}  // namespace Json

template <typename T>
Json::Str toJson(const T &__data) {
    return Json::toJson(__data);
}

template <typename T>
T fromJson(const Json::Str &__json) {
    return Json::fromJson<T>(__json);
}
}  // namespace CppClass