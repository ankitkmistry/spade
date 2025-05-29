#pragma once

#include "obj.hpp"

namespace spade
{
    class ObjPointer : public Obj {
      private:
        Obj *value;

      public:
        ObjPointer(Obj *value = null);

        Obj *get() const {
            return value;
        }

        void set(Obj *value) {
            this->value = value;
        }

        Obj *copy() const override {
            return (Obj *) this;
        }

        bool truth() const override;
        string to_string() const override;
    };
}    // namespace spade