#ifndef VELOCITY_TYPEPARAM_HPP
#define VELOCITY_TYPEPARAM_HPP

#include "type.hpp"

namespace spade
{
    class TypeParam : public Type {
        Type *placeholder = null;

        void checkPlaceholder() const;

      public:
        TypeParam(Sign sign, ObjModule *module = null) : Type(sign, Kind::TYPE_PARAM, {}, {}, {}, module) {}

        /**
         * Changes the type parameter to the specified \p type
         * @param type the final type
         */
        void setPlaceholder(Type *type) { placeholder = type; }

        Type *getPlaceholder() const { return placeholder; }

        Kind getKind() const override;

        const Table<NamedRef *> &getTypeParams() const override;

        const Table<Type *> &getSupers() const override;

        const Table<MemberSlot> &getMemberSlots() const override;

        Table<NamedRef *> &getTypeParams() override;

        Table<Type *> &getSupers() override;

        Table<MemberSlot> &getMemberSlots() override;

        ObjModule *getModule() const override;

        const Table<string> &getMeta() const override;

        const Sign &getSign() const override;

        Type *getType() const override;

        Obj *getMember(const string& name) const override;

        void setMember(const string& name, Obj *value) override;

        ObjMethod *getSuperClassMethod(const string& sign) override;

        Obj *getStaticMember(const string& name) const override;

        void setStaticMember(const string& name, Obj *value) override;

        Type *getReified(Obj **args, uint8 count) override;

        TypeParam *getTypeParam(const string& name) const override;

        NamedRef *captureTypeParam(const string& name) override;

        Obj *copy() override;
    };
} // namespace spade

#endif // VELOCITY_TYPEPARAM_HPP
