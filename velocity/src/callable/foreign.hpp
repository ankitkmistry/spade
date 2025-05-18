#pragma once

#include "loader/foreign_loader.hpp"
#include "callable.hpp"

namespace spade
{
    class ObjForeign final : public ObjCallable {
        Library *library;
        string name;
        Obj *self;

      public:
        ObjForeign(const Sign &sign, Kind kind, Type *type, ObjModule *module) : ObjCallable(sign, kind, type, module) {}

        void link_library();

        void call(const vector<Obj *> &args) override;

        void call(Obj **args) override;

        Obj *copy() override {
            return (Obj *) this;
        }

        string to_string() const override;
    };
}    // namespace spade
