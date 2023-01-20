#ifndef __CPP_CLASS_H
#define __CPP_CLASS_H

#define This static_cast<Derive *>(this)

template <class Derive>
class SinpleEnum {
   public:
    Derive &operator=(Derive &&other) {
        This->__del();
        This->__tag = other.__tag;
        other.__del();
        return *This;
    }
    Derive &operator=(const Derive &other) {
        This->__del();
        This->__tag = other.__tag;
        return *This;
    }
};
template <class Derive>
class ComplexEnum {
   public:
    Derive &operator=(Derive &&other) {
        This->__del();
        This->__tag = other.__tag;
        This->__data = std::move(other.__data);
        other.__del();
        return *This;
    }
    Derive &operator=(Derive &&other) {
        This->__del();
        This->__tag = other.__tag;
        This->__data = other.__data;
        return *This;
    }
};
#undef This

#endif