#pragma once

#include "callable.hpp"
#include "frame.hpp"

namespace spade
{
    class SWAN_EXPORT ObjMethod final : public ObjCallable {
      private:
        // The table of all reified types in the form of [type_arg_specifier -> type]
        // static std::unordered_map<vector<Type *>, ObjMethod *> reification_table;

        FrameTemplate frame_template;

      public:
        ObjMethod(Kind kind, const Sign &sign, const FrameTemplate &frame);

        FrameTemplate &get_frame_template() {
            return frame_template;
        }

        void call(Obj *self, const vector<Obj *> &args) override;

        void call(Obj *self, Obj **args) override;

        TypeParam *get_type_param(const string &name) const;

        Obj *copy() const override {
            return (Obj *) this;
        }

        string to_string() const override;
    };
}    // namespace spade
