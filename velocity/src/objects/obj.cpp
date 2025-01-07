#include "obj.hpp"
#include "../ee/vm.hpp"
#include "inbuilt_types.hpp"
#include "module.hpp"
#include "type.hpp"
#include "typeparam.hpp"

namespace spade
{
    static Table<MemberSlot> Type_getAllNonStaticMembers(Type *type, Table<ObjMethod *> &superMethods) {
        Table<MemberSlot> result;
        for (auto super: type->getSupers() | std::views::values) {
            for (auto [name, member]: Type_getAllNonStaticMembers(super, superMethods)) {
                if (!member.getFlags().isStatic()) {
                    result[name] = MemberSlot{Obj::createCopy(member.getValue()), member.getFlags()};
                }
            }
        }
        for (auto [name, member]: type->getMemberSlots()) {
            if (!member.getFlags().isStatic()) {
                // Save methods that are being overrode
                if (is<ObjMethod *>(member.getValue()) && result.contains(name)) {
                    auto method = cast<ObjMethod *>(member.getValue());
                    superMethods[method->getSign().toString()] = method;
                }
                result[name] = MemberSlot{Obj::createCopy(member.getValue()), member.getFlags()};
            }
        }
        return result;
    }

    Obj::Obj(const Sign &sign, Type *type, ObjModule *module) : module(module), sign(sign), type(type) {
        if (this->module == null) { this->module = ObjModule::current(); }
        if (type != null) {
            memberSlots = Type_getAllNonStaticMembers(type, superClassMethods);
            for (auto slot: memberSlots | std::views::values) {
                Obj *value = slot.getValue();
                if (is<ObjCallable *>(value)) { cast<ObjCallable *>(value->copy())->setSelf(this); }
            }
        }
    }

    void Obj::reify(Obj **pObj, const Table<NamedRef *> &old_, const Table<NamedRef *> &new_) {
#define REIFY(pObj) reify(pObj, old_, new_)
        if (*pObj == null) return;
        if (old_.empty() || new_.empty()) return;

        if (auto array = dynamic_cast<ObjArray *>(*pObj); array != null) {
            // Reify array items
            for (int i = 0; i < array->count(); ++i) {
                auto item = array->get(i);
                REIFY(&item);
                array->set(i, item);
            }
        } else if (auto method = dynamic_cast<ObjMethod *>(*pObj); method != null) {
            auto frameTemplate = method->getFrameTemplate();
            // Reify args
            auto &args = frameTemplate->getArgs();
            for (int i = 0; i < args.count(); ++i) {
                auto arg = args.get(i);
                REIFY(&arg);
            }
            // Reify locals
            auto &locals = frameTemplate->getLocals();
            for (int i = 0; i < locals.count(); ++i) {
                auto local = locals.get(i);
                REIFY(&local);
            }
            // Reify lambdas
            auto &lambdas = frameTemplate->getLambdas();
            for (auto lambda: lambdas) {
                auto lambdaObj = cast<Obj *>(lambda);
                REIFY(&lambdaObj);
            }
            // Reify matches
            auto &matches = frameTemplate->getMatches();
            for (const auto &match: matches) {
                auto cases = match.getCases();
                for (const auto &kase: cases) {
                    auto caseValue = kase.getValue();
                    REIFY(&caseValue);
                }
            }
            // Reify exceptions
            auto &exceptions = frameTemplate->getExceptions();
            for (int i = 0; i < exceptions.count(); ++i) {
                auto excType = cast<Obj *>(exceptions.get(i).getType());
                REIFY(&excType);
            }
        } else if (auto tp = dynamic_cast<TypeParam *>(*pObj); tp != null) {
            // Change type params accordingly
            for (const auto &[name, param]: old_) {
                if (*pObj == param->getValue()) {
                    *pObj = new_.at(name)->getValue();
                    break;
                }
            }
            return;
        }

        auto object = *pObj;
        // Reify object type
        Obj *type = object->type;
        if (type != null) {
            REIFY(&type);
            object->type = cast<Type *>(type);
        }
        // Reify members
        for (auto member: object->getMemberSlots() | std::views::values) { REIFY(&member.getValue()); }
#undef REIFY
    }

    Obj *Obj::createCopy(Obj *obj) {
        return is<Type *>(obj) || is<ObjCallable *>(obj) || is<ObjModule *>(obj) ? obj : obj->copy();
    }

    Obj *Obj::getMember(const string &name) const {
        try {
            return getMemberSlots().at(name).getValue();
        } catch (std::out_of_range &) {
            if (type == null) {
                throw IllegalAccessError(std::format("cannot find member: {} in {}", name, toString()));
            } else {
                try {
                    return type->getStaticMember(name);
                } catch (const IllegalAccessError &) {
                    throw IllegalAccessError(std::format("cannot find member: {} in {}", name, toString()));
                }
            }
        }
    }

    void Obj::setMember(const string& name, Obj *value) {
        try {
            getMemberSlots().at(name).setValue(value);
        } catch (std::out_of_range &) {
            if (type == null) {
                getMemberSlots()[name] = MemberSlot{value, 0b0001000000000000};
            } else {
                try {
                    type->setStaticMember(name, value);
                } catch (const IllegalAccessError &) { getMemberSlots()[name] = MemberSlot{value, 0b0001000000000000}; }
            }
        }
        if (is<ObjCallable *>(value)) {
            cast<ObjCallable *>(value)->setSelf(this);
            if (is<ObjMethod *>(value)) { cast<ObjMethod *>(value)->setType(type); }
        }
    }

    const Table<string> &Obj::getMeta() const {
        static Table<string> noMeta = {};
        if (sign.empty()) return noMeta;
        try {
            return info.manager->getVM()->getMetadata(sign.toString());
        } catch (const IllegalAccessError &) { return noMeta; }
    }

    Obj *Obj::copy() {
        auto copyObj = halloc<Obj>(info.manager, sign, type, module);
        for (auto [name, slot]: memberSlots) { copyObj->setMember(name, createCopy(slot.getValue())); }
        return copyObj;
    }

    string Obj::toString() const { return std::format("<object {} : '{}'>", type->getSign().toString(), sign.toString()); }

    ObjMethod *Obj::getSuperClassMethod(const string& mSign) {
        try {
            return superClassMethods[mSign];
        } catch (const std::out_of_range &) {
            throw IllegalAccessError(std::format("cannot find superclass method: {} in {}", mSign, toString()));
        }
    }

    ObjBool *ComparableObj::operator<(const Obj *rhs) const { return halloc<ObjBool>(info.manager, compare(rhs) < 0); }

    ObjBool *ComparableObj::operator>(const Obj *rhs) const { return halloc<ObjBool>(info.manager, compare(rhs) > 0); }

    ObjBool *ComparableObj::operator<=(const Obj *rhs) const { return halloc<ObjBool>(info.manager, compare(rhs) <= 0); }

    ObjBool *ComparableObj::operator>=(const Obj *rhs) const { return halloc<ObjBool>(info.manager, compare(rhs) >= 0); }

    ObjBool *ComparableObj::operator==(const Obj *rhs) const { return halloc<ObjBool>(info.manager, compare(rhs) == 0); }

    ObjBool *ComparableObj::operator!=(const Obj *rhs) const { return halloc<ObjBool>(info.manager, compare(rhs) != 0); }
} // namespace spade
