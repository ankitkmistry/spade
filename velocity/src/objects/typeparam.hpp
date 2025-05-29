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
        TypeParam(const Sign &sign) : Type(sign, Kind::TYPE_PARAM, {}, {}, {}) {
            this->tag = ObjTag::TYPE_PARAM;
        }

        // Type param specific methods

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
         * Changes the type parameter to the specified @p type
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

        // Obj specific
        Type *get_type() const override;
        const Table<MemberSlot> &get_member_slots() const override;
        Table<MemberSlot> &get_member_slots() override;
        void set_member_slots(const Table<MemberSlot> &member_slots) override;
        Obj *copy() const override;
        string to_string() const override;
        Obj *get_member(const string &name) const override;
        void set_member(const string &name, Obj *value) override;

        // Type specific
        Kind get_kind() const override;
        const Sign &get_sign() const override;
        void set_sign(const Sign &sign) override;
        const Table<TypeParam *> &get_type_params() const override;
        void set_type_params(const Table<TypeParam *> &type_params) override;
        Table<TypeParam *> &get_type_params() override;
        const Table<Type *> &get_supers() const override;
        void set_supers(const Table<Type *> &supers) override;
        Table<Type *> &get_supers() override;
        Type *get_reified(Type *const *args, uint8 count) const override;
        TypeParam *get_type_param(const string &name) const override;
    };
}    // namespace spade
