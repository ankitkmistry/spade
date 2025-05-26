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
        ObjForeign(const Sign &sign, Kind kind) : ObjCallable(sign, kind) {}

        void link_library();

        void call(const vector<Obj *> &args) override;

        void call(Obj **args) override;

        Obj *copy() const override {
            // Immutable state
            return (Obj *) this;
        }

        string to_string() const override;
    };
}    // namespace spade
