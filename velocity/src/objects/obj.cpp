#include "obj.hpp"
#include "inbuilt_types.hpp"
#include "memory/memory.hpp"
#include "module.hpp"
#include "type.hpp"
#include "typeparam.hpp"
#include "ee/vm.hpp"

namespace spade
{
    static Table<MemberSlot> type_get_all_members(Type *type) {
        if (is<TypeParam>(type))
            if (!cast<TypeParam>(type)->get_placeholder())
                return {};

        Table<MemberSlot> result;

        for (const auto super: type->get_supers() | std::views::values)
            for (const auto &[name, member]: type_get_all_members(super))
                result[name] = MemberSlot{Obj::create_copy(member.get_value()), member.get_flags()};

        for (const auto &[name, member]: type->get_member_slots()) {
            // Save methods that are being overrode
            if (is<ObjMethod>(member.get_value()) && result.contains(name)) {
                const auto method = cast<ObjMethod>(member.get_value());
            }
            result[name] = MemberSlot{Obj::create_copy(member.get_value()), member.get_flags()};
        }
        return result;
    }

    Obj::Obj(Type *type) : type(type) {
        // this->tag = ObjTag::OBJECT;
        if (type) {
            if (is<TypeParam>(type))
                // Claim this object by the type param
                cast<TypeParam>(type)->claim(this);
            member_slots = type_get_all_members(type);
        }
    }

    Obj::Obj() : type(null) {
        set_type(SpadeVM::current()->get_vm_type(tag));
        member_slots = type_get_all_members(type);
    }

    void Obj::set_type(Type *destType) {
        if (type == destType) {
            member_slots = type_get_all_members(type);
            return;
        }
        // Unclaim the object from the previous type if it was a type param
        if (is<TypeParam>(type))
            cast<TypeParam>(type)->unclaim(this);
        type = destType;
        if (type) {
            if (is<TypeParam>(type))
                // Claim this object by the type param
                cast<TypeParam>(type)->claim(this);
            member_slots = type_get_all_members(type);
        } else
            member_slots.clear();
    }

    void Obj::reify(Obj *obj, const Table<TypeParam *> &old_tps, const Table<TypeParam *> &new_tps) {
        struct Reifier {
            void reify_non_rec(Obj *obj, const Table<TypeParam *> &old_tps, const Table<TypeParam *> &new_tps) const {
                // Change the type if it is a type parameter
                if (is<TypeParam>(obj->get_type())) {
                    const auto tp = cast<TypeParam>(obj->get_type());
                    if (const auto it = old_tps.find(tp->get_tp_sign()); it != old_tps.end())
                        obj->set_type(it->second);
                }
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
                    for (const auto &[value, _]: match.get_table()) {
                        reify_non_rec(value, old_tps, new_tps);
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
                if (is<ObjMethod>(obj))
                    reify_method(cast<ObjMethod>(obj), old_tps, new_tps);
                else {
                    // Reify the members if it was not a method
                    for (const auto &[name, slot]: obj->get_member_slots()) {
                        reify_non_rec(slot.get_value(), old_tps, new_tps);
                        if (is<ObjMethod>(slot.get_value()))
                            reify_method(cast<ObjMethod>(slot.get_value()), old_tps, new_tps);
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
        if (const auto it = get_member_slots().find(name); it != get_member_slots().end())
            return it->second.get_value();
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
    }

    void Obj::set_member(const string &name, Obj *value) {
        if (const auto it = get_member_slots().find(name); it != get_member_slots().end())
            it->second.set_value(value);
        get_member_slots()[name] = MemberSlot{value, Flags().set_public()};
    }

    Obj *Obj::copy() const {
        const auto obj = halloc_mgr<Obj>(info.manager, type);
        for (auto [name, slot]: member_slots) {
            obj->set_member(name, create_copy(slot.get_value()));
        }
        return obj;
    }

    string Obj::to_string() const {
        return std::format("<object of type {}>", type->get_sign().to_string());
    }

    ObjBool *ObjComparable::operator<(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) < 0);
    }

    ObjBool *ObjComparable::operator>(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) > 0);
    }

    ObjBool *ObjComparable::operator<=(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) <= 0);
    }

    ObjBool *ObjComparable::operator>=(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) >= 0);
    }

    ObjBool *ObjComparable::operator==(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) == 0);
    }

    ObjBool *ObjComparable::operator!=(const Obj *rhs) const {
        return halloc_mgr<ObjBool>(info.manager, compare(rhs) != 0);
    }
}    // namespace spade
