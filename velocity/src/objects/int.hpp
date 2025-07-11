#pragma once

#include "inbuilt_types.hpp"

namespace spade
{
    class ObjInt : public ObjNumber {
      private:
        int64 val;

      public:
        ObjInt(int64 val);

        Obj *copy() const override {
            // Immutable state
            return (Obj *) this;
        }

        bool truth() const override;

        string to_string() const override;

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

        ObjInt *unsigned_right_shift(const ObjInt &n) const;

        int64 value() const {
            return val;
        }
    };
}    // namespace spade