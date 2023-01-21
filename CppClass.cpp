#include "CppClass.h"

namespace CppClass {
void __appendBinary(Bytes &res, uint32_t data) {
    res.push_back(data & 0xff);
    res.push_back((data >> 8) & 0xff);
    res.push_back((data >> 16) & 0xff);
    res.push_back((data >> 24) & 0xff);
}
void __appendBinary(Bytes &res, uint64_t data) {
    __appendBinary(res, uint32_t(data & 0xffffffff));
    __appendBinary(res, uint32_t((data >> 32) & 0xffffffff));
}
}  // namespace CppClass