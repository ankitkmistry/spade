#include "obj.hpp"
#include "inbuilt_types.hpp"
#include "memory/memory.hpp"
#include "module.hpp"
#include "type.hpp"
#include "typeparam.hpp"
#include "ee/vm.hpp"

namespace spade
{
    static Table<MemberSlot> type_get_all_non_static_members(Type *type, Table<ObjMethod *> &super_methods) {
        Table<MemberSlot> result;
        for (auto super: type->get_supers() | std::views::values) {
            for (auto [name, member]: type_get_all_non_static_members(super, super_methods)) {
                if (!member.get_flags().is_static()) {
                    result[name] = MemberSlot{Obj::create_copy(member.get_value()), member.get_flags()};
                }
            }
        }
        for (auto [name, member]: type->get_member_slots()) {
            if (!member.get_flags().is_static()) {
                // Save methods that are being overrode
                if (is<ObjMethod>(member.get_value()) && result.contains(name)) {
                    auto method = cast<ObjMethod>(member.get_value());
                    super_methods[method->get_sign().to_string()] = method;
                }
                result[name] = MemberSlot{Obj::create_copy(member.get_value()), member.get_flags()};
            }
        }
        return result;
    }

    Obj::Obj(const Sign &sign, Type *type, ObjModule *module) : module(module), sign(sign), type(type) {
        if (this->module == null) {
            this->module = ObjModule::current();
        }
        if (type != null) {
            member_slots = type_get_all_non_static_members(type, super_class_methods);
        }
    }

    void Obj::reify(Obj **p_obj, const Table<TypeParam *> &old_, const Table<TypeParam *> &new_) {
        // #define REIFY(p_obj) reify(p_obj, old_, new_)
        //         if (*p_obj == null)
        //             return;
        //         if (old_.empty() || new_.empty())
        //             return;

        //         if (auto array = dynamic_cast<ObjArray *>(*p_obj); array != null) {
        //             // Reify array items
        //             for (int i = 0; i < array->count(); ++i) {
        //                 auto item = array->get(i);
        //                 REIFY(&item);
        //                 array->set(i, item);
        //             }
        //         } else if (auto method = dynamic_cast<ObjMethod *>(*p_obj); method != null) {
        //             auto frame_template = method->get_frame_template();
        //             // Reify args
        //             auto &args = frame_template->get_args();
        //             for (int i = 0; i < args.count(); ++i) {
        //                 auto arg = args.get(i);
        //                 REIFY(&arg);
        //             }
        //             // Reify locals
        //             auto &locals = frame_template->get_locals();
        //             for (int i = 0; i < locals.count(); ++i) {
        //                 auto local = locals.get(i);
        //                 REIFY(&local);
        //             }
        //             // Reify lambdas
        //             auto &lambdas = frame_template->get_lambdas();
        //             for (auto lambda: lambdas) {
        //                 auto lambda_obj = cast<Obj>(lambda);
        //                 REIFY(&lambda_obj);
        //             }
        //             // Reify matches
        //             auto &matches = frame_template->get_matches();
        //             for (const auto &match: matches) {
        //                 auto cases = match.get_cases();
        //                 for (const auto &kase: cases) {
        //                     auto case_value = kase.get_value();
        //                     REIFY(&case_value);
        //                 }
        //             }
        //             // Reify exceptions
        //             auto &exceptions = frame_template->get_exceptions();
        //             for (int i = 0; i < exceptions.count(); ++i) {
        //                 auto exc_type = cast<Obj>(exceptions.get(i).get_type());
        //                 REIFY(&exc_type);
        //             }
        //         } else if (auto tp = dynamic_cast<TypeParam *>(*p_obj); tp != null) {
        //             // Change type params accordingly
        //             for (const auto &[name, param]: old_) {
        //                 if (*p_obj == param->get_value()) {
        //                     *p_obj = new_.at(name)->get_value();
        //                     break;
        //                 }
        //             }
        //             return;
        //         }

        //         auto object = *p_obj;
        //         // Reify object type
        //         Obj *type = object->type;
        //         if (type != null) {
        //             REIFY(&type);
        //             object->type = cast<Type>(type);
        //         }
        //         // Reify members
        //         for (auto member: object->get_member_slots() | std::views::values) {
        //             REIFY(&member.get_value());
        //         }
        // #undef REIFY
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
            get_member_slots()[name] = MemberSlot{value, 0b0001000000000000};
        }
    }

    const Table<string> &Obj::get_meta() const {
        static Table<string> no_meta = {};
        if (sign.empty())
            return no_meta;
        try {
            return info.manager->get_vm()->get_metadata(sign.to_string());
        } catch (const IllegalAccessError &) {
            return no_meta;
        }
    }

    Obj *Obj::copy() {
        auto copy_obj = halloc_mgr<Obj>(info.manager, sign, type, module);
        for (auto [name, slot]: member_slots) {
            copy_obj->set_member(name, create_copy(slot.get_value()));
        }
        return copy_obj;
    }

    string Obj::to_string() const {
        return std::format("<object {} : '{}'>", type->get_sign().to_string(), sign.to_string());
    }

    ObjMethod *Obj::get_super_class_method(const string &m_sign) {
        try {
            return super_class_methods[m_sign];
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find superclass method: {} in {}", m_sign, to_string()));
        }
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
