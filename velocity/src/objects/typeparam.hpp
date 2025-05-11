#pragma once

#include "type.hpp"

namespace spade
{
    class TypeParam : public Type {
        Type *placeholder = null;

        void check_placeholder() const;

      public:
        TypeParam(Sign sign, ObjModule *module = null) : Type(sign, Kind::TYPE_PARAM, {}, {}, {}, module) {}

        /**
         * Changes the type parameter to the specified \p type
         * @param type the final type
         */
        void set_placeholder(Type *type) { placeholder = type; }

        Type *get_placeholder() const { return placeholder; }

        Kind get_kind() const override;

        const Table<NamedRef *> &get_type_params() const override;

        const Table<Type *> &get_supers() const override;

        const Table<MemberSlot> &get_member_slots() const override;

        Table<NamedRef *> &get_type_params() override;

        Table<Type *> &get_supers() override;

        Table<MemberSlot> &get_member_slots() override;

        ObjModule *get_module() const override;

        const Table<string> &get_meta() const override;

        const Sign &get_sign() const override;

        Type *get_type() const override;

        Obj *get_member(const string& name) const override;

        void set_member(const string& name, Obj *value) override;

        ObjMethod *get_super_class_method(const string& sign) override;

        Obj *get_static_member(const string& name) const override;

        void set_static_member(const string& name, Obj *value) override;

        Type *get_reified(Obj **args, uint8 count) override;

        TypeParam *get_type_param(const string& name) const override;

        NamedRef *capture_type_param(const string& name) override;

        Obj *copy() override;
    };
} // namespace spade
