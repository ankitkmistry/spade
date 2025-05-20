#include "obj.hpp"
#include "inbuilt_types.hpp"
#include "memory/memory.hpp"
#include "module.hpp"
#include "type.hpp"
#include "typeparam.hpp"
#include "ee/vm.hpp"

namespace spade
{
    static Table<MemberSlot> type_get_all_members(Type *type, Table<ObjMethod *> &super_methods) {
        if (const auto tp = dynamic_cast<TypeParam *>(type))
            if (!tp->get_placeholder()) {
                super_methods = {};
                return {};
            }

        Table<MemberSlot> result;

        for (const auto super: type->get_supers() | std::views::values)
            for (const auto &[name, member]: type_get_all_members(super, super_methods))
                result[name] = MemberSlot{Obj::create_copy(member.get_value()), member.get_flags()};

        for (const auto &[name, member]: type->get_member_slots()) {
            // Save methods that are being overrode
            if (is<ObjMethod>(member.get_value()) && result.contains(name)) {
                const auto method = cast<ObjMethod>(member.get_value());
                super_methods[method->get_sign().to_string()] = method;
            }
            result[name] = MemberSlot{Obj::create_copy(member.get_value()), member.get_flags()};
        }
        return result;
    }

    Obj::Obj(const Sign &sign, Type *type, ObjModule *module) : module(module), sign(sign), type(type) {
        if (this->module == null)
            this->module = ObjModule::current();
        if (type) {
            if (const auto tp = dynamic_cast<TypeParam *>(type))
                // Claim this object by the type param
                tp->claim(this);
            member_slots = type_get_all_members(type, super_class_methods);
        }
    }

    void Obj::set_type(Type *destType) {
        if (type == destType) {
            member_slots = type_get_all_members(type, super_class_methods);
            return;
        }
        // Unclaim the object from the previous type if it was a type param
        if (const auto tp = dynamic_cast<TypeParam *>(type))
            tp->unclaim(this);
        type = destType;
        if (type) {
            if (const auto tp = dynamic_cast<TypeParam *>(type))
                // Claim this object by the type param
                tp->claim(this);
            member_slots = type_get_all_members(type, super_class_methods);
        } else {
            member_slots.clear();
            super_class_methods.clear();
        }
    }

    void Obj::reify(Obj *obj, const Table<TypeParam *> &old_tps, const Table<TypeParam *> &new_tps) {
        struct Reifier {
            void reify_non_rec(Obj *obj, const Table<TypeParam *> &old_tps, const Table<TypeParam *> &new_tps) const {
                // Change the type if it is a type parameter
                if (const auto tp = dynamic_cast<TypeParam *>(obj->get_type()))
                    if (const auto it = old_tps.find(tp->get_tp_sign()); it != old_tps.end())
                        obj->set_type(it->second);
            }

            void reify_method(ObjMethod *method, const Table<TypeParam *> &old_tps, const Table<TypeParam *> &new_tps) const {
                const auto &frame = method->get_frame_template();
                // Reify the args
                const auto &args = frame.get_args();
                for (size_t i = 0; i < args.count(); i++) reify_non_rec(args.get(i), old_tps, new_tps);
                // Reify the locals
                const auto &locals = frame.get_locals();
                for (size_t i = 0; i < locals.count(); i++) reify_non_rec(locals.get(i), old_tps, new_tps);
                // Reify the matches
                for (const auto &match: frame.get_matches()) {
                    for (const auto &kase: match.get_cases()) {
                        reify_non_rec(kase.get_value(), old_tps, new_tps);
                    }
                }
                // Reify the members as well
                for (const auto &[name, slot]: method->get_member_slots()) {
                    reify_non_rec(slot.get_value(), old_tps, new_tps);
                }
            }

            void operator()(Obj *obj, const Table<TypeParam *> &old_tps, const Table<TypeParam *> &new_tps) const {
                // Reify the object itself
                reify_non_rec(obj, old_tps, new_tps);
                // Check all other different places in the case of ObjMethod
                if (const auto method = dynamic_cast<ObjMethod *>(obj))
                    reify_method(method, old_tps, new_tps);
                else {
                    // Reify the members if it was not a method
                    for (const auto &[name, slot]: obj->get_member_slots()) {
                        reify_non_rec(slot.get_value(), old_tps, new_tps);
                        if (const auto method = dynamic_cast<ObjMethod *>(slot.get_value()))
                            reify_method(method, old_tps, new_tps);
                    }
                }
            }
        };

        constexpr const static Reifier reify_fn;
        return reify_fn(obj, old_tps, new_tps);
    }

    Obj *Obj::create_copy_dynamic(const Obj *obj) {
        if (is<const Type>(obj) || is<const ObjCallable>(obj) || is<const ObjModule>(obj))
            // Unique state
            return (Obj *) obj;
        else
            return obj->copy();
    }

    Obj *Obj::get_member(const string &name) const {
        try {
            return get_member_slots().at(name).get_value();
        } catch (std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
        }
    }

    void Obj::set_member(const string &name, Obj *value) {
        try {
            get_member_slots().at(name).set_value(value);
        } catch (std::out_of_range &) {
            get_member_slots()[name] = MemberSlot{value, Flags().set_public()};
        }
    }

    const Table<string> &Obj::get_meta() const {
        const static Table<string> no_meta = {};
        if (sign.empty())
            return no_meta;
        try {
            return info.manager->get_vm()->get_metadata(sign.to_string());
        } catch (const IllegalAccessError &) {
            return no_meta;
        }
    }

    Obj *Obj::copy() const {
        const auto obj = halloc_mgr<Obj>(info.manager, sign, type, module);
        for (auto [name, slot]: member_slots) {
            obj->set_member(name, create_copy(slot.get_value()));
        }
        return obj;
    }

    string Obj::to_string() const {
        return std::format("<object {} : '{}'>", type->get_sign().to_string(), sign.to_string());
    }

    ObjMethod *Obj::get_super_class_method(const string &m_sign) {
        if (const auto it = super_class_methods.find(m_sign); it != super_class_methods.end())
            return it->second;
        throw IllegalAccessError(std::format("cannot find superclass method: {} in {}", m_sign, to_string()));
    }

    ObjBool *ComparableObj::operator<(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) < 0);
    }

    ObjBool *ComparableObj::operator>(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) > 0);
    }

    ObjBool *ComparableObj::operator<=(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) <= 0);
    }

    ObjBool *ComparableObj::operator>=(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) >= 0);
    }

    ObjBool *ComparableObj::operator==(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) == 0);
    }

    ObjBool *ComparableObj::operator!=(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) != 0);
    }
}    // namespace spade
