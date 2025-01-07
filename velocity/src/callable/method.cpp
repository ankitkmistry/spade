#include "method.hpp"
#include "../ee/thread.hpp"

namespace spade
{
    Table<map<Table<Type *>, ObjMethod *>> ObjMethod::reificationTable = {};

    ObjMethod::ObjMethod(const Sign &sign, Kind kind, FrameTemplate *frame, Type *type, const Table<NamedRef *> &typeParams,
                         ObjModule *module)
        : ObjCallable(sign, kind, type, module), frameTemplate(frame), typeParams(typeParams) {
        frameTemplate->setMethod(this);
    }

    Obj *ObjMethod::copy() {
        Table<NamedRef *> newTypeParams;
        for (const auto &[name, typeParam]: typeParams) {
            newTypeParams[name] =
                    halloc<NamedRef>(info.manager, typeParam->getName(), typeParam->getValue()->copy(), typeParam->getMeta());
        }
        Obj *newMethod = halloc<ObjMethod>(info.manager, sign, kind, frameTemplate->copy(), type, newTypeParams, module);
        reify(&newMethod, typeParams, newTypeParams);
        return newMethod;
    }

    void ObjMethod::call(const vector<Obj *> &args) {
        validateCallSite();
        Thread *thread = Thread::current();
        auto newFrame = frameTemplate->initialize();
        if (newFrame.getArgs().count() < args.size()) {
            throw ArgumentError(sign.toString(),
                                std::format("too less arguments, expected {} got {}", newFrame.getArgs().count(), args.size()));
        }
        if (newFrame.getArgs().count() > args.size()) {
            throw ArgumentError(sign.toString(),
                                std::format("too many arguments, expected {} got {}", newFrame.getArgs().count(), args.size()));
        }
        for (int i = 0; i < newFrame.getArgs().count(); i++) { newFrame.getArgs().set(i, args[i]); }
        thread->getState()->pushFrame(newFrame);
    }

    void ObjMethod::call(Obj **args) {
        validateCallSite();
        Thread *thread = Thread::current();
        auto newFrame = frameTemplate->initialize();
        for (int i = 0; i < newFrame.getArgs().count(); i++) { newFrame.getArgs().set(i, args[i]); }
        thread->getState()->pushFrame(newFrame);
    }

    string ObjMethod::toString() const {
        static string kindNames[] = {"function", "method", "constructor"};
        return std::format("<{} '{}'>", kindNames[static_cast<int>(kind)], sign.toString());
    }

    ObjMethod *ObjMethod::returnReified(const Table<Type *> &tParams) const {
        auto method = cast<ObjMethod *>(copy());
        auto params = method->getTypeParams();
        for (auto [name, typeParam]: params) { cast<TypeParam *>(typeParam->getValue())->setPlaceholder(tParams.at(name)); }
        return method;
    }

    ObjMethod *ObjMethod::getReified(Obj **args, uint8 count) const {
        if (typeParams.size() != count) {
            throw ArgumentError(sign.toString(),
                                std::format("expected {} type arguments, but got {}", typeParams.size(), count));
        }

        Table<Type *> typeArgs;
        for (int i = 0; i < count; ++i) { typeArgs[getSign().getTypeParams()[i]] = (cast<Type *>(args[i])); }

        map<Table<Type *>, ObjMethod *> table;
        ObjMethod *reifiedMethod;

        try {
            table = reificationTable.at(getSign().toString());
        } catch (const std::out_of_range &) {
            reifiedMethod = returnReified(typeArgs);
            table[typeArgs] = reifiedMethod;
            reificationTable[getSign().toString()] = table;
            return reifiedMethod;
        }

        try {
            return table.at(typeArgs);
        } catch (const std::out_of_range &) {
            reifiedMethod = returnReified(typeArgs);
            reificationTable[getSign().toString()][typeArgs] = reifiedMethod;
            return reifiedMethod;
        }
    }

    ObjMethod *ObjMethod::getReified(const vector<Type *> &args) const {
        if (args.size() >= UINT8_MAX) {
            throw ArgumentError(toString(), "number of type arguments cannot be greater than 256");
        }
        auto data = const_cast<Type **>(args.data());
        return getReified(reinterpret_cast<Obj **>(data), static_cast<uint8>(args.size()));
    }

    void ObjMethod::setSelf(Obj *self) {
        if (frameTemplate->getLocals().count() == 0) return;
        auto local = frameTemplate->getLocals().getLocal(0);
        local->setValue(self);
        local->setNoCopy(true);
    }

    TypeParam *ObjMethod::getTypeParam(const string &name) const {
        try {
            return cast<TypeParam *>(typeParams.at(name)->getValue());
        } catch (const std::out_of_range &) {
            if (type != null) { return type->getTypeParam(name); }
            throw IllegalAccessError(std::format("cannot find type param {} in {}", name, toString()));
        }
    }

    NamedRef *ObjMethod::captureTypeParam(const string &name) const {
        try {
            return typeParams.at(name);
        } catch (const std::out_of_range &) {
            if (type != null) { return type->captureTypeParam(name); }
            throw IllegalAccessError(std::format("cannot find type param {} in {}", name, toString()));
        }
    }
} // namespace spade
