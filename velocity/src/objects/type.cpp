#include "type.hpp"
#include "../callable/table.hpp"
#include "typeparam.hpp"

static string kindNames[] = {"class", "interface", "enum", "annotation", "type_parameter", "unresolved"};

namespace spade
{
    Table<map<Table<Type *>, Type *>> Type::reificationTable = {};

    Obj *Type::copy() {
        // Copy type params
        Table<NamedRef *> newTypeParams;
        for (const auto &[name, typeParam]: typeParams) {
            newTypeParams[name] =
                    halloc<NamedRef>(info.manager, typeParam->getName(), typeParam->getValue()->copy(), typeParam->getMeta());
        }
        // Create new type object
        Obj *newType = halloc<Type>(info.manager, sign, kind, newTypeParams, supers, memberSlots, module);
        // Reify the type params
        reify(&newType, typeParams, newTypeParams);
        return newType;
    }

    bool Type::truth() const { return true; }

    string Type::toString() const { return std::format("<{} '{}'>", kindNames[static_cast<int>(kind)], sign.toString()); }

    Obj *Type::getStaticMember(const string &name) const {
        try {
            auto slot = getMemberSlots().at(name);
            if (slot.getFlags().isStatic()) { return slot.getValue(); }
        } catch (const std::out_of_range &) {}

        Obj *obj = null;
        // Check for the members in the super classes
        for (auto super: getSupers() | std::views::values) {
            try {
                obj = super->getStaticMember(name);
            } catch (const IllegalAccessError &) { obj = null; }
        }
        if (obj == null) { throw IllegalAccessError(std::format("cannot find static member: {} in {}", name, toString())); }
        return obj;
    }

    void Type::setStaticMember(const string &name, Obj *value) {
        try {
            auto &slot = getMemberSlots().at(name);
            if (slot.getFlags().isStatic()) slot.setValue(value);
            return;
        } catch (const std::out_of_range &) {}

        // Check for the members in the super classes
        for (auto super: getSupers() | std::views::values) {
            try {
                super->setStaticMember(name, value);
                return;
            } catch (const IllegalAccessError &) { continue; }
        }
        throw IllegalAccessError(std::format("cannot find static member: {} in {}", name, toString()));
    }

    Type *Type::SENTINEL_(const string &sign, MemoryManager *manager) {
        return halloc<Type>(manager, Sign(sign), Kind::UNRESOLVED, Table<NamedRef *>{}, Table<Type *>{}, Table<MemberSlot>{},
                            null);
    }

    Type *Type::returnReified(const Table<Type *> &typeParams) {
        auto type = cast<Type *>(copy());
        auto params = type->getTypeParams();
        for (const auto &[name, typeParam]: params) {
            cast<TypeParam *>(typeParam->getValue())->setPlaceholder(typeParams.at(name));
        }
        return type;
    }

    Type::Type(const Type &type) : Obj(type.sign, null, type.module) {
        kind = type.kind;
        typeParams = type.typeParams;
        supers = type.supers;
        memberSlots = type.memberSlots;
    }

    Type *Type::getReified(Obj **args, uint8 count) {
        if (typeParams.size() != count) {
            throw ArgumentError(sign.toString(),
                                std::format("expected {} type arguments, but got {}", typeParams.size(), count));
        }

        Table<Type *> typeArgs;
        for (int i = 0; i < count; ++i) { typeArgs[getSign().getTypeParams()[i]] = cast<Type *>(args[i]); }

        map<Table<Type *>, Type *> table;
        Type *reifiedType;

        try {
            table = reificationTable.at(getSign().toString());
        } catch (const std::out_of_range &) {
            reifiedType = returnReified(typeArgs);
            table[typeArgs] = reifiedType;
            reificationTable[getSign().toString()] = table;
            return reifiedType;
        }

        try {
            return table.at(typeArgs);
        } catch (const std::out_of_range &) {
            reifiedType = returnReified(typeArgs);
            reificationTable[getSign().toString()][typeArgs] = reifiedType;
            return reifiedType;
        }
    }

    Type *Type::getReified(const vector<Type *> &args) {
        if (args.size() >= UINT8_MAX) {
            throw ArgumentError(toString(), "number of type arguments cannot be greater than 256");
        }
        auto data = const_cast<Type **>(args.data());
        return getReified(reinterpret_cast<Obj **>(&data), static_cast<uint8>(args.size()));
    }

    TypeParam *Type::getTypeParam(const string &name) const {
        try {
            return cast<TypeParam *>(typeParams.at(name)->getValue());
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find type param {} in {}", name, toString()));
        }
    }

    NamedRef *Type::captureTypeParam(const string &name) {
        try {
            return typeParams.at(name);
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find type param {} in {}", name, toString()));
        }
    }
} // namespace spade