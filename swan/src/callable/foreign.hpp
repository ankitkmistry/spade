#pragma once

#include "callable.hpp"

namespace spade
{
    class ObjForeign final : public ObjCallable {
        void *handle;
        bool has_self;

      public:
        ObjForeign(const Sign &sign, void *handle, bool has_self)
            : ObjCallable(ObjTag::FOREIGN, Kind::FOREIGN, sign), handle(handle), has_self(has_self) {}

        void call(Obj *self, const vector<Obj *> &args) override;
        void call(Obj *self, Obj **args) override;

      private:
        void foreign_call(Obj *self, const vector<Obj *> &args);
    };
}    // namespace spade
