#include "typeparam.hpp"
#include "memory/memory.hpp"
#include "type.hpp"

namespace spade
{
    void TypeParam::set_placeholder(Type *type) {
        placeholder = type;
        // Trigger changes in all the objects
        for (const auto obj: claimed_objs) obj->set_type(this);
    }

    Type::Kind TypeParam::get_kind() const {
        check_placeholder();
        return placeholder->get_kind();
    }

    const Table<TypeParam *> &TypeParam::get_type_params() const {
        check_placeholder();
        return placeholder->get_type_params();
    }

    const Table<Type *> &TypeParam::get_supers() const {
        check_placeholder();
        return placeholder->get_supers();
    }

    const Table<MemberSlot> &TypeParam::get_member_slots() const {
        check_placeholder();
        return placeholder->get_member_slots();
    }

    Table<TypeParam *> &TypeParam::get_type_params() {
        check_placeholder();
        return placeholder->get_type_params();
    }

    Table<Type *> &TypeParam::get_supers() {
        check_placeholder();
        return placeholder->get_supers();
    }

    Table<MemberSlot> &TypeParam::get_member_slots() {
        check_placeholder();
        return placeholder->get_member_slots();
    }

    ObjModule *TypeParam::get_module() const {
        check_placeholder();
        return placeholder->get_module();
    }

    const Table<string> &TypeParam::get_meta() const {
        check_placeholder();
        return placeholder->get_meta();
    }

    const Sign &TypeParam::get_sign() const {
        check_placeholder();
        return placeholder->get_sign();
    }

    Type *TypeParam::get_type() const {
        check_placeholder();
        return placeholder->get_type();
    }

    Obj *TypeParam::copy() const {
        const auto obj = halloc_mgr<TypeParam>(info.manager, sign, module);
        // No need to clear as `claimed_objs` is initialized as empty
        // obj->claimed_objs.clear();
        obj->set_placeholder(placeholder);
        return obj;
    }

    string TypeParam::to_string() const {
        return placeholder ? placeholder->to_string() : Type::to_string();
    }

    void TypeParam::check_placeholder() const {
        if (placeholder == null)
            throw IllegalTypeParamAccessError(sign.to_string());
    }

    Obj *TypeParam::get_member(const string &name) const {
        check_placeholder();
        return placeholder->get_member(name);
    }

    void TypeParam::set_member(const string &name, Obj *value) {
        check_placeholder();
        placeholder->set_member(name, value);
    }

    ObjMethod *TypeParam::get_super_class_method(const string &sign) {
        check_placeholder();
        return placeholder->get_super_class_method(sign);
    }

    Type *TypeParam::get_reified(Type *const *args, uint8 count) const {
        check_placeholder();
        return placeholder->get_reified(args, count);
    }

    TypeParam *TypeParam::get_type_param(const string &name) const {
        check_placeholder();
        return placeholder->get_type_param(name);
    }
}    // namespace spade