#include <cmath>

#include "int.hpp"
#include "float.hpp"
#include "ee/vm.hpp"
#include "memory/memory.hpp"

namespace spade
{
    ObjInt::ObjInt(int64 val) : ObjNumber(), val(val) {
        this->tag = ObjTag::INT;
        set_type(SpadeVM::current()->get_vm_type(tag));
    }

    bool ObjInt::truth() const {
        return val != 0;
    }

    string ObjInt::to_string() const {
        return std::to_string(val);
    }

    ObjInt *ObjInt::operator~() const {
        return halloc_mgr<ObjInt>(info.manager, ~val);
    }

    ObjInt *ObjInt::operator<<(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val << n.val);
    }

    ObjInt *ObjInt::operator>>(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val >> n.val);
    }

    ObjInt *ObjInt::unsigned_right_shift(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val & 0x7fffffff >> n.val);
    }

    ObjInt *ObjInt::operator%(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val % n.val);
    }

    ObjInt *ObjInt::operator&(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val & n.val);
    }

    ObjInt *ObjInt::operator|(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val | n.val);
    }

    ObjInt *ObjInt::operator^(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val ^ n.val);
    }

    int32 ObjInt::compare(const Obj *rhs) const {
        if (!is<const ObjInt>(rhs))
            return 0;
        return val - cast<const ObjInt>(rhs)->val;
    }

    Obj *ObjInt::operator-() const {
        return halloc_mgr<ObjInt>(info.manager, -val);
    }

    Obj *ObjInt::power(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, std::pow(val, cast<const ObjInt>(n)->val));
    }

    Obj *ObjInt::operator+(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val + cast<const ObjInt>(n)->val);
    }

    Obj *ObjInt::operator-(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val - cast<const ObjInt>(n)->val);
    }

    Obj *ObjInt::operator*(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val * cast<const ObjInt>(n)->val);
    }

    Obj *ObjInt::operator/(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val / cast<const ObjInt>(n)->val);
    }
}    // namespace spade