#ifndef __CPP_CLASS_H
#define __CPP_CLASS_H

#include "AxMarshal/Bin.h"
#include "AxMarshal/Json.h"

#define This static_cast<Derived *>(this)
#define CThis static_cast<const Derived *>(this)

namespace AxMarshal {
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
}  // namespace AxMarshal

#undef CThis
#undef This

#endif