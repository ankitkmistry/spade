#include <cmath>

#include "float.hpp"
#include "ee/vm.hpp"
#include "memory/memory.hpp"

namespace spade
{
    ObjFloat::ObjFloat(double val) : ObjNumber(), val(val) {
        this->tag = ObjTag::FLOAT;
        set_type(SpadeVM::current()->get_vm_type(tag));
    }

    bool ObjFloat::truth() const {
        return val != 0.0;
    }

    string ObjFloat::to_string() const {
        return std::to_string(val);
    }

    int32 ObjFloat::compare(const Obj *rhs) const {
        if (!is<const ObjFloat>(rhs))
            return 0;
        return val - cast<const ObjFloat>(rhs)->val;
    }

    Obj *ObjFloat::operator-() const {
        return halloc_mgr<ObjFloat>(info.manager, -val);
    }

    Obj *ObjFloat::power(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, std::pow(val, cast<const ObjFloat>(n)->val));
    }

    Obj *ObjFloat::operator+(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val + cast<const ObjFloat>(n)->val);
    }

    Obj *ObjFloat::operator-(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val - cast<const ObjFloat>(n)->val);
    }

    Obj *ObjFloat::operator*(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val * cast<const ObjFloat>(n)->val);
    }

    Obj *ObjFloat::operator/(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val / cast<const ObjFloat>(n)->val);
    }
}    // namespace spade