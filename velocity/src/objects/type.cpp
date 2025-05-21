#include "type.hpp"
#include "memory/memory.hpp"
#include "typeparam.hpp"
#include "utils/common.hpp"

static const string kind_names[] = {"class", "interface", "enum", "annotation", "type_parameter", "unresolved"};

namespace spade
{
    std::unordered_map<vector<Type *>, Type *> Type::reification_table = {};

    Obj *Type::copy() const {
        const auto obj = halloc_mgr<Type>(info.manager, sign, kind, type_params, supers, member_slots, module);
        // Copy members
        for (const auto &[name, slot]: member_slots) {
            obj->set_member(name, create_copy(slot.get_value()));
        }
        // Create new type params
        Table<TypeParam *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = cast<TypeParam>(type_param->copy());
        }
        Obj::reify(obj, type_params, new_type_params);
        return obj;
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

    Type::Type(const Type &type) : Obj(type.sign, null, type.module) {
        kind = type.kind;
        type_params = type.type_params;
        supers = type.supers;
        member_slots = type.member_slots;
    }

    Type *Type::get_reified(Type *const *args, uint8 count) const {
        // Check if the number of type args is correct
        if (type_params.size() < count)
            throw ArgumentError(sign.to_string(), std::format("too less type arguments, expected {} got {}", type_params.size(), count));
        if (type_params.size() > count)
            throw ArgumentError(sign.to_string(), std::format("too many type arguments, expected {} got {}", type_params.size(), count));

        Table<Type *> type_args;
        vector<Type *> ta_specifier(count);    // Type arg specifier
        for (int i = 0; i < count; ++i) {
            // Build the type arg specifier and the list of type args
            const auto type = cast<Type>(args[i]);
            type_args["[" + get_sign().get_type_params()[i] + "]"] = type;
            ta_specifier[i] = type;
        }

        if (const auto it = reification_table.find(ta_specifier); it != reification_table.end())
            // Return the type if it was already reified
            return it->second;
        // Or else, create a new type and reify it
        const auto reified_type = cast<Type>(copy());
        for (const auto &[name, tp]: reified_type->type_params) {
            tp->set_placeholder(type_args[name]);
        }
        // Return the reified type
        return reified_type;
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