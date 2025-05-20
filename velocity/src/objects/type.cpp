#include "type.hpp"
#include "memory/memory.hpp"
#include "typeparam.hpp"

static string kind_names[] = {"class", "interface", "enum", "annotation", "type_parameter", "unresolved"};

namespace spade
{
    Table<std::unordered_map<Table<Type *>, Type *>> Type::reification_table = {};

    Obj *Type::copy() const {
        // Copy type params
        Table<TypeParam *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = Obj::create_copy(type_param);
        }
        // Create new type object
        Obj *new_type = halloc_mgr<Type>(info.manager, sign, kind, new_type_params, supers, member_slots, module);
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

    Type *Type::UNRESOLVED(const Sign &sign, ObjModule *module, MemoryManager *manager) {
        return halloc_mgr<Type>(manager, sign, Kind::UNRESOLVED, Table<TypeParam *>{}, Table<Type *>{}, Table<MemberSlot>{}, module);
    }

    Type *Type::return_reified(const Table<Type *> &type_params) {
        auto type = cast<Type>(copy());
        auto params = type->get_type_params();
        for (const auto &[name, type_param]: params) {
            type_param->set_placeholder(type_params.at(name));
        }
        return type;
    }

    Type::Type(const Type &type) : Obj(type.sign, null, type.module) {
        kind = type.kind;
        type_params = type.type_params;
        supers = type.supers;
        member_slots = type.member_slots;
    }

    Type *Type::get_reified(Type *const *args, uint8 count) const {
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

    Type *Type::get_reified(const vector<Type *> &args) const {
        if (args.size() >= UINT8_MAX)
            throw ArgumentError(to_string(), "number of type arguments cannot be greater than 256");
        return get_reified(args.data(), static_cast<uint8>(args.size()));
    }

    TypeParam *Type::get_type_param(const string &name) const {
        if (const auto it = type_params.find(name); it != type_params.end())
            return it->second;
        throw IllegalAccessError(std::format("cannot find type param {} in {}", name, to_string()));
    }
}    // namespace spade