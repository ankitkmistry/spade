#include "table.hpp"
#include "../objects/int.hpp"

namespace spade
{
    NamedRef *NamedRef::copy() const { return halloc<NamedRef>(null, name, noCopy ? value : Obj::createCopy(value), meta); }

    Obj *ArgsTable::get(uint8 i) const {
        if (i >= args.size()) throw IndexError("argument", i);
        return args[i]->getValue();
    }

    void ArgsTable::set(uint8 i, Obj *val) {
        if (i >= args.size()) throw IndexError("argument", i);
        args[i]->setValue(val);
    }

    ArgsTable ArgsTable::copy() const {
        ArgsTable newArgs;
        for (auto arg: args) { newArgs.addArg(arg->copy()); }
        newArgs.args.shrink_to_fit();
        return newArgs;
    }

    Obj *LocalsTable::get(uint16 i) const {
        if (i >= closureStart) return getClosure(i)->getValue();
        return getLocal(i)->getValue();
    }

    void LocalsTable::set(uint16 i, Obj *val) {
        if (i >= closureStart) getClosure(i)->setValue(val);
        else
            getLocal(i)->setValue(val);
    }

    NamedRef *LocalsTable::getLocal(uint16 i) const {
        if (i >= locals.size()) throw IndexError("local", i);
        return locals[i];
    }

    NamedRef *LocalsTable::getClosure(uint16 i) const {
        if (i - closureStart >= closures.size()) throw IndexError("closure", i - closureStart);
        return closures[i - closureStart];
    }

    LocalsTable LocalsTable::copy() const {
        LocalsTable newLocals{closureStart};
        for (auto local: locals) { newLocals.addLocal(local->copy()); }
        newLocals.locals.shrink_to_fit();
        for (auto closure: closures) { newLocals.addClosure(closure); }
        newLocals.closures.shrink_to_fit();
        return newLocals;
    }

    Exception ExceptionTable::getTarget(uint32 pc, const Type *type) const {
        for (auto &exception: exceptions) {
            if (exception.getFrom() <= pc && pc < exception.getTo() && exception.getType() == type) { return exception; }
        }
        return Exception::NO_EXCEPTION();
    }

    void LineNumberTable::addLine(uint8 times, uint32 sourceLine) {
        if (!lineInfos.empty() && lineInfos.back().sourceLine == sourceLine) {
            lineInfos.back().byteEnd += times;
        } else {
            uint16 end = lineInfos.empty() ? 0 : lineInfos.back().byteEnd;
            lineInfos.push_back(LineInfo{.sourceLine = sourceLine, .byteStart = end, .byteEnd = (uint16) (end + times)});
        }
    }

    uint64 LineNumberTable::getSourceLine(uint32 byteLine) const {
        for (auto lineInfo: lineInfos) {
            if (lineInfo.byteStart <= byteLine && byteLine < lineInfo.byteEnd) { return lineInfo.sourceLine; }
        }
        throw IllegalAccessError(std::format("no source line mapping is present for byte line {}", byteLine));
    }

    uint32 MatchTable::perform(Obj *value) const {
        // Info: improve this to perform fast matching in case of integer values
        if (is<ObjInt *>(value)) {
            auto val = cast<ObjInt *>(value)->value();
            return cases[val].getLocation();
        }
        for (auto kase: cases) {
            if (kase.getValue() == value) return kase.getLocation();
        }
        return defaultLocation;
    }
} // namespace spade
