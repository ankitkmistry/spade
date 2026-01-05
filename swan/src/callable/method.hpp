#pragma once

#include "callable.hpp"
#include "frame.hpp"

namespace spade
{
    class ObjMethod final : public ObjCallable {
      private:
        // The table of all reified types in the form of [type_arg_specifier -> type]
        // static std::unordered_map<vector<Type *>, ObjMethod *> reification_table;

        FrameTemplate frame_template;
        Table<Type *> type_params;

      public:
        ObjMethod(Kind kind, const Sign &sign, const FrameTemplate &frame, const Table<Type *> &type_params);

        FrameTemplate &get_frame_template() {
            return frame_template;
        }

        const Table<Type *> &get_type_params() const {
            return type_params;
        }

        Table<Type *> &get_type_params() {
            return type_params;
        }

        void call(Obj *self, const vector<Obj *> &args) override;

        void call(Obj *self, Obj **args) override;

        /**
         * Reifies this type and returns the reified type.
         * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array @p args
         * must be positioned according to the type params present in the signature
         * @throws ArgumentError if count is not correct
         * @throws CastError if objects in the array are not types
         * @param args the type args
         * @param count count of type args
         * @return the reified type
         */
        // ObjMethod *get_reified(Obj **args, uint8_t count) const;

        /**
         * Reifies this type and returns the reified type.
         * The returned type may be newly reified or previously reified
         * so as to maintain type uniqueness. The type to be reified not always
         * has to be a generic type. The objects in the array @p args
         * must be positioned according to the type params present in the signature
         * @throws ArgumentError if args.size() > 256
         * @throws ArgumentError if number of arguments provided is not correct
         * @throws CastError if objects in the array are not types
         * @param args the type args
         * @param count count of type args
         * @return the reified type
         */
        // ObjMethod *get_reified(const vector<Type *> &args) const;

        TypeParam *get_type_param(const string &name) const;

        Obj *copy() const override {
            return (Obj *) this;
        }

        string to_string() const override;

        Obj *force_copy() const;
    };
}    // namespace spade
