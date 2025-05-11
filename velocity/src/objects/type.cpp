#include "type.hpp"
#include "../callable/table.hpp"
#include "typeparam.hpp"

static string kind_names[] = {"class", "interface", "enum", "annotation", "type_parameter", "unresolved"};

namespace spade
{
    Table<std::unordered_map<Table<Type *>, Type *>> Type::reification_table = {};

    Obj *Type::copy() {
        // Copy type params
        Table<NamedRef *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = halloc<NamedRef>(info.manager, type_param->get_name(), type_param->get_value()->copy(), type_param->get_meta());
        }
        // Create new type object
        Obj *new_type = halloc<Type>(info.manager, sign, kind, new_type_params, supers, member_slots, module);
        // Reify the type params
        reify(&new_type, type_params, new_type_params);
        return new_type;
    }

    bool Type::truth() const {
        return true;
    }

    string Type::to_string() const {
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    Obj *Type::get_static_member(const string &name) const {
        try {
            auto slot = get_member_slots().at(name);
            if (slot.get_flags().is_static()) {
                return slot.get_value();
            }
        } catch (const std::out_of_range &) {}

        Obj *obj = null;
        // Check for the members in the super classes
        for (auto super: get_supers() | std::views::values) {
            try {
                obj = super->get_static_member(name);
            } catch (const IllegalAccessError &) {
                obj = null;
            }
        }
        if (obj == null) {
            throw IllegalAccessError(std::format("cannot find static member: {} in {}", name, to_string()));
        }
        return obj;
    }

    void Type::set_static_member(const string &name, Obj *value) {
        try {
            auto &slot = get_member_slots().at(name);
            if (slot.get_flags().is_static())
                slot.set_value(value);
            return;
        } catch (const std::out_of_range &) {}

        // Check for the members in the super classes
        for (auto super: get_supers() | std::views::values) {
            try {
                super->set_static_member(name, value);
                return;
            } catch (const IllegalAccessError &) {
                continue;
            }
        }
        throw IllegalAccessError(std::format("cannot find static member: {} in {}", name, to_string()));
    }

    Type *Type::SENTINEL_(const string &sign, MemoryManager *manager) {
        return halloc<Type>(manager, Sign(sign), Kind::UNRESOLVED, Table<NamedRef *>{}, Table<Type *>{}, Table<MemberSlot>{}, null);
    }

    Type *Type::return_reified(const Table<Type *> &type_params) {
        auto type = cast<Type>(copy());
        auto params = type->get_type_params();
        for (const auto &[name, type_param]: params) {
            cast<TypeParam>(type_param->get_value())->set_placeholder(type_params.at(name));
        }
        return type;
    }

    Type::Type(const Type &type) : Obj(type.sign, null, type.module) {
        kind = type.kind;
        type_params = type.type_params;
        supers = type.supers;
        member_slots = type.member_slots;
    }

    Type *Type::get_reified(Obj **args, uint8 count) {
        return null;
        // TODO: implement this

        // if (type_params.size() != count) {
        //     throw ArgumentError(sign.to_string(), std::format("expected {} type arguments, but got {}", type_params.size(), count));
        // }

        // Table<Type *> type_args;
        // for (int i = 0; i < count; ++i) {
        //     type_args[get_sign().get_type_params()[i]] = cast<Type>(args[i]);
        // }

        // std::unordered_map<Table<Type *>, Type *> table;
        // Type *reified_type;

        // try {
        //     table = reification_table.at(get_sign().to_string());
        // } catch (const std::out_of_range &) {
        //     reified_type = return_reified(type_args);
        //     table[type_args] = reified_type;
        //     reification_table[get_sign().to_string()] = table;
        //     return reified_type;
        // }

        // try {
        //     return table.at(type_args);
        // } catch (const std::out_of_range &) {
        //     reified_type = return_reified(type_args);
        //     reification_table[get_sign().to_string()][type_args] = reified_type;
        //     return reified_type;
        // }
    }

    Type *Type::get_reified(const vector<Type *> &args) {
        if (args.size() >= UINT8_MAX) {
            throw ArgumentError(to_string(), "number of type arguments cannot be greater than 256");
        }
        auto data = const_cast<Type **>(args.data());
        return get_reified(reinterpret_cast<Obj **>(&data), static_cast<uint8>(args.size()));
    }

    TypeParam *Type::get_type_param(const string &name) const {
        try {
            return cast<TypeParam>(type_params.at(name)->get_value());
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find type param {} in {}", name, to_string()));
        }
    }

    NamedRef *Type::capture_type_param(const string &name) {
        try {
            return type_params.at(name);
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find type param {} in {}", name, to_string()));
        }
    }
}    // namespace spade