#ifndef __CPP_CLASS_H
#define __CPP_CLASS_H

#include <utility>

#define This static_cast<Derive *>(this)
#define CThis static_cast<const Derive *>(this)

namespace CppClass {
template <class Derive>
class SimpleEnum {
   public:
    void __assign(Derive &&other) {
        This->__del();
        This->__tag = other.__tag;
        other.__del();
    }
    void __assign(const Derive &other) {
        if (This == &other) {
            return;
        }
        This->__del();
        This->__tag = other.__tag;
    }
    void __del() { This->__tag = Derive::__Tag::__UNDEF; }
    bool operator==(const Derive &other) const {
        return CThis->__tag == other.__tag;
    }
    bool operator!=(const Derive &other) const {
        return CThis->__tag != other.__tag;
    }
};
template <class Derive>
class ComplexEnum {
   public:
    void __assign(Derive &&other) {
        This->__del();
        This->__tag = other.__tag;
        This->__data.__assign(This->__tag, std::move(other.__data));
        other.__del();
    }
    void __assign(const Derive &other) {
        if (This == &other) {
            return;
        }
        This->__del();
        This->__tag = other.__tag;
        This->__data.__assign(This->__tag, other.__data);
    }
    void __del() {
        This->__data.__del(This->__tag);
        This->__tag = Derive::__Tag::__UNDEF;
    }
};
}  // namespace CppClass

#undef This
#undef CThis

#endif