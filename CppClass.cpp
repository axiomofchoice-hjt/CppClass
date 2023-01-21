#include "CppClass.h"
#include<cstdio>

namespace CppClass {
void __toBinary(Bytes &res, uint32_t data) {
    res.push_back(data & 0xff);
    res.push_back((data >> 8) & 0xff);
    res.push_back((data >> 16) & 0xff);
    res.push_back((data >> 24) & 0xff);
}
void __toBinary(Bytes &res, uint64_t data) {
    __toBinary(res, uint32_t(data & 0xffffffff));
    __toBinary(res, uint32_t((data >> 32) & 0xffffffff));
}
void __toBinary(Bytes &res, int32_t data) { __toBinary(res, uint32_t(data)); }
void __toBinary(Bytes &res, int64_t data) { __toBinary(res, uint64_t(data)); }

void __fromBinary(Iter &it, uint32_t &data) {
    data = 0;
    data |= uint32_t(*it);
    data |= uint32_t(*(it + 1)) << 8;
    data |= uint32_t(*(it + 2)) << 16;
    data |= uint32_t(*(it + 3)) << 24;
    it += 4;
}
void __fromBinary(Iter &it, uint64_t &data) {
    data = 0;
    uint32_t tmp;
    __fromBinary(it, tmp);
    data |= tmp;
    __fromBinary(it, tmp);
    data |= uint64_t(tmp) << 32;
}
void __fromBinary(Iter &it, int32_t &data) {
    uint32_t tmp;
    __fromBinary(it, tmp);
    data = tmp;
}
void __fromBinary(Iter &it, int64_t &data) {
    uint64_t tmp;
    __fromBinary(it, tmp);
    data = tmp;
}
}  // namespace CppClass