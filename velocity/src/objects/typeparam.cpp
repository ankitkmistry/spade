#include "typeparam.hpp"

namespace spade
{
    Type::Kind TypeParam::get_kind() const {
        check_placeholder();
        return placeholder->get_kind();
    }

    const Table<NamedRef *> &TypeParam::get_type_params() const {
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

    Table<NamedRef *> &TypeParam::get_type_params() {
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

    Obj *TypeParam::copy() {
        auto newTypeParam = halloc<TypeParam>(info.manager, sign, module);
        newTypeParam->set_placeholder(placeholder);
        return newTypeParam;
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

    Obj *TypeParam::get_static_member(const string &name) const {
        check_placeholder();
        return placeholder->get_static_member(name);
    }

    void TypeParam::set_static_member(const string &name, Obj *value) {
        check_placeholder();
        placeholder->set_static_member(name, value);
    }

    Type *TypeParam::get_reified(Obj **args, uint8 count) {
        check_placeholder();
        return placeholder->get_reified(args, count);
    }

    TypeParam *TypeParam::get_type_param(const string &name) const {
        check_placeholder();
        return placeholder->get_type_param(name);
    }

    NamedRef *TypeParam::capture_type_param(const string &name) {
        check_placeholder();
        return placeholder->capture_type_param(name);
    }
}    // namespace spade