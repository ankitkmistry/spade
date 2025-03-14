#pragma once

#include "inbuilt_types.hpp"

namespace spade
{
    class ObjFloat : public ObjNumber {
      private:
        double val;

      public:
        ObjFloat(double val, ObjModule *module = null) : ObjNumber(Sign("float"), module), val(val) {}

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

        double value() const {
            return val;
        }
    };
}    // namespace spade
