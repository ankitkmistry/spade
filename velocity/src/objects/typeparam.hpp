#pragma once

#include <unordered_set>

#include "type.hpp"

namespace spade
{
    class TypeParam : public Type {
        std::unordered_set<Obj *> claimed_objs;
        Type *placeholder = null;

        void check_placeholder() const;

      public:
        TypeParam(const Sign &sign, ObjModule *module = null) : Type(sign, Kind::TYPE_PARAM, {}, {}, {}, module) {}

        string get_tp_sign() const {
            return sign.to_string();
        }

        void claim(Obj *obj) {
            claimed_objs.insert(obj);
        }

        void unclaim(Obj *obj) {
            claimed_objs.erase(obj);
        }

        /**
         * Changes the type parameter to the specified \p type
         * @param type the final type
         */
        void set_placeholder(Type *type);

        /**
         * Get the placeholder object
         * @return Type*
         */
        Type *get_placeholder() const {
            return placeholder;
        }

        Kind get_kind() const override;

        const Table<TypeParam *> &get_type_params() const override;

        const Table<Type *> &get_supers() const override;

        const Table<MemberSlot> &get_member_slots() const override;

        Table<TypeParam *> &get_type_params() override;

        Table<Type *> &get_supers() override;

        Table<MemberSlot> &get_member_slots() override;

        ObjModule *get_module() const override;

        const Table<string> &get_meta() const override;

        const Sign &get_sign() const override;

        Type *get_type() const override;

        Obj *get_member(const string &name) const override;

        void set_member(const string &name, Obj *value) override;

        ObjMethod *get_super_class_method(const string &sign) override;

        Type *get_reified(Type *const *args, uint8 count) const override;

        TypeParam *get_type_param(const string &name) const override;

        Obj *copy() const override;

        string to_string() const override;
    };
}    // namespace spade
