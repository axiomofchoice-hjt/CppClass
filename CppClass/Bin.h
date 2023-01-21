#pragma once

#include <cstdint>
#include <vector>

namespace CppClass {
namespace Bin {
using Bytes = std::vector<uint8_t>;
using Iter = Bytes::const_iterator;

void __toBinary(Bytes &res, const uint32_t &data);
void __toBinary(Bytes &res, const uint64_t &data);
void __toBinary(Bytes &res, const int32_t &data);
void __toBinary(Bytes &res, const int64_t &data);
template <typename T>
Bytes toBinary(const T &__data) {
    Bytes __res;
    void __toBinary(Bytes &, const T &);
    __toBinary(__res, __data);
    return __res;
}

void __fromBinary(Iter &it, uint32_t &data);
void __fromBinary(Iter &it, uint64_t &data);
void __fromBinary(Iter &it, int32_t &data);
void __fromBinary(Iter &it, int64_t &data);
template <typename T>
T fromBinary(const Bytes &__bin) {
    T __res;
    Iter __tmp_iter = __bin.cbegin();
    void __fromBinary(Iter &, T &);
    __fromBinary(__tmp_iter, __res);
    return __res;
}
}  // namespace Bin

template <typename T>
Bin::Bytes toBinary(const T &__data) {
    return Bin::toBinary(__data);
}

template <typename T>
T fromBinary(const Bin::Bytes &__bin) {
    return Bin::fromBinary<T>(__bin);
}
}  // namespace CppClass