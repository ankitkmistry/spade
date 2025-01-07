#ifndef VELOCITY_INT_HPP
#define VELOCITY_INT_HPP

#include "inbuilt_types.hpp"

namespace spade {
    class ObjInt : public ObjNumber {
    private:
        int64 val;
    public:
        ObjInt(int64 val, ObjModule *module = null)
                : ObjNumber(Sign("int"), module), val(val) {}

        Obj *copy() override;

        bool truth() const override;

        string toString() const override;

        int32 compare(const Obj *rhs) const override;

        Obj *operator-() const override;

        Obj *power(const ObjNumber *n) const override;

        Obj *operator+(const ObjNumber *n) const override;

        Obj *operator-(const ObjNumber *n) const override;

        Obj *operator*(const ObjNumber *n) const override;

        Obj *operator/(const ObjNumber *n) const override;

        ObjInt *operator~() const;

        ObjInt *operator%(const ObjInt &n) const;

        ObjInt *operator<<(const ObjInt &n) const;

        ObjInt *operator>>(const ObjInt &n) const;

        ObjInt *operator&(const ObjInt &n) const;

        ObjInt *operator|(const ObjInt &n) const;

        ObjInt *operator^(const ObjInt &n) const;

        ObjInt *unsignedRightShift(const ObjInt &n) const;

        int64 value() const { return val; }
    };
}

#endif //VELOCITY_INT_HPP
