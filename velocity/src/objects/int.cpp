#include <math.h>
#include "int.hpp"
#include "float.hpp"

namespace spade {
    Obj *ObjInt::copy() {
        return (Obj *) this;
    }

    bool ObjInt::truth() const {
        return val != 0;
    }

    string ObjInt::toString() const {
        return std::to_string(val);
    }

    ObjInt *ObjInt::operator~() const {
        return halloc<ObjInt>(info.manager, ~val);
    }

    ObjInt *ObjInt::operator<<(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val << n.val);
    }

    ObjInt *ObjInt::operator>>(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val >> n.val);
    }

    ObjInt *ObjInt::unsignedRightShift(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val & 0x7fffffff >> n.val);
    }

    ObjInt *ObjInt::operator%(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val % n.val);
    }

    ObjInt *ObjInt::operator&(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val & n.val);
    }

    ObjInt *ObjInt::operator|(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val | n.val);
    }

    ObjInt *ObjInt::operator^(const ObjInt &n) const {
        return halloc<ObjInt>(info.manager, val ^ n.val);
    }

    int32 ObjInt::compare(const Obj *rhs) const {
        if (!is<const ObjInt *>(rhs))return 0;
        return val - cast<const ObjInt *>(rhs)->val;
    }

    Obj *ObjInt::operator-() const {
        return halloc<ObjInt>(info.manager, -val);
    }

    Obj *ObjInt::power(const ObjNumber *n) const {
        return halloc<ObjFloat>(info.manager, pow(val, cast<const ObjInt *>(n)->val));
    }

    Obj *ObjInt::operator+(const ObjNumber *n) const {
        return halloc<ObjInt>(info.manager, val + cast<const ObjInt *>(n)->val);
    }

    Obj *ObjInt::operator-(const ObjNumber *n) const {
        return halloc<ObjInt>(info.manager, val - cast<const ObjInt *>(n)->val);
    }

    Obj *ObjInt::operator*(const ObjNumber *n) const {
        return halloc<ObjInt>(info.manager, val * cast<const ObjInt *>(n)->val);
    }

    Obj *ObjInt::operator/(const ObjNumber *n) const {
        return halloc<ObjInt>(info.manager, val / cast<const ObjInt *>(n)->val);
    }
}