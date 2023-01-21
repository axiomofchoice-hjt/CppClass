#ifndef __CPP_CLASS_H
#define __CPP_CLASS_H

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#define This static_cast<Derived *>(this)
#define CThis static_cast<const Derived *>(this)

namespace CppClass {
template <class Derived>
class SimpleEnum {
   public:
    Derived &__assign(Derived &&other) {
        This->__del();
        This->__tag = other.__tag;
        other.__del();
        return *This;
    }
    Derived &__assign(const Derived &other) {
        if (This == &other) {
            return *This;
        }
        This->__del();
        This->__tag = other.__tag;
        return *This;
    }
    Derived &__assign(std::nullptr_t) {
        This->__del();
        return *This;
    }
    void __del() { This->__tag = Derived::__Tag::__UNDEF; }
    bool operator==(const Derived &b) const { return CThis->__tag == b.__tag; }
    bool operator!=(const Derived &b) const { return CThis->__tag != b.__tag; }
    bool operator==(std::nullptr_t b) const {
        return CThis->__tag == Derived::__Tag::__UNDEF;
    }
    bool operator!=(std::nullptr_t b) const {
        return CThis->__tag != Derived::__Tag::__UNDEF;
    }
};
template <class Derived>
class ComplexEnum {
   public:
    Derived &__assign(Derived &&other) {
        This->__del();
        This->__tag = other.__tag;
        This->__data.__assign(This->__tag, std::move(other.__data));
        other.__del();
        return *This;
    }
    Derived &__assign(const Derived &other) {
        if (This == &other) {
            return *This;
        }
        This->__del();
        This->__tag = other.__tag;
        This->__data.__assign(This->__tag, other.__data);
        return *This;
    }
    Derived &__assign(std::nullptr_t) {
        This->__del();
        return *This;
    }
    void __del() {
        This->__data.__del(This->__tag);
        This->__tag = Derived::__Tag::__UNDEF;
    }
    bool operator==(std::nullptr_t b) const {
        return CThis->__tag == Derived::__Tag::__UNDEF;
    }
    bool operator!=(std::nullptr_t b) const {
        return CThis->__tag != Derived::__Tag::__UNDEF;
    }
};

using Bytes = std::vector<uint8_t>;

void __toBinary(Bytes &res, uint32_t data);
void __toBinary(Bytes &res, uint64_t data);
void __toBinary(Bytes &res, int32_t data);
void __toBinary(Bytes &res, int64_t data);

template <typename T>
Bytes toBinary(const T &__data) {
    Bytes __res;
    __toBinary(__res, __data);
    return __res;
}

void __fromBinary(Bytes::iterator &it, uint32_t &data);
void __fromBinary(Bytes::iterator &it, uint64_t &data);
void __fromBinary(Bytes::iterator &it, int32_t &data);
void __fromBinary(Bytes::iterator &it, int64_t &data);

template <typename T>
T fromBinary(const Bytes &__bin) {
    T __res;
    __fromBinary(__bin.begin(), __res);
    return __res;
}
}  // namespace CppClass

#undef CThis
#undef This

#endif