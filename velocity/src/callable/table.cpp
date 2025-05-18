#include "table.hpp"
#include "objects/int.hpp"
#include "utils/exceptions.hpp"

namespace spade
{
    NamedRef NamedRef::copy() const {
        return NamedRef(name, no_copy ? value : Obj::create_copy(value), meta);
    }

    Obj *ArgsTable::get(uint8 i) const {
        if (i >= args.size())
            throw IndexError("argument", i);
        return args[i].get_value();
    }

    void ArgsTable::set(uint8 i, Obj *val) {
        if (i >= args.size())
            throw IndexError("argument", i);
        args[i].set_value(val);
    }

    ArgsTable ArgsTable::copy() const {
        ArgsTable new_args;
        for (auto arg: args) {
            new_args.add_arg(arg.copy());
        }
        new_args.args.shrink_to_fit();
        return new_args;
    }

    Obj *LocalsTable::get(uint16 i) const {
        if (i >= closureStart)
            return get_closure(i)->get_value();
        return get_local(i).get_value();
    }

    void LocalsTable::set(uint16 i, Obj *val) {
        if (i >= closureStart)
            get_closure(i)->set_value(val);
        else
            get_local(i).set_value(val);
    }

    const NamedRef &LocalsTable::get_local(uint16 i) const {
        if (i >= locals.size())
            throw IndexError("local", i);
        return locals[i];
    }

    NamedRef &LocalsTable::get_local(uint16 i) {
        if (i >= locals.size())
            throw IndexError("local", i);
        return locals[i];
    }

    NamedRef *LocalsTable::get_closure(uint16 i) const {
        if (i - closureStart >= closures.size())
            throw IndexError("closure", i - closureStart);
        return closures[i - closureStart];
    }

    LocalsTable LocalsTable::copy() const {
        LocalsTable new_locals{closureStart};
        for (auto local: locals) {
            new_locals.add_local(local.copy());
        }
        new_locals.locals.shrink_to_fit();
        for (auto closure: closures) {
            new_locals.add_closure(closure);
        }
        new_locals.closures.shrink_to_fit();
        return new_locals;
    }

    Exception ExceptionTable::get_target(uint32 pc, const Type *type) const {
        for (auto &exception: exceptions) {
            if (exception.get_from() <= pc && pc < exception.get_to() && exception.get_type() == type) {
                return exception;
            }
        }
        return Exception::NO_EXCEPTION();
    }

    void LineNumberTable::add_line(uint8 times, uint32 source_line) {
        if (!line_infos.empty() && line_infos.back().sourceLine == source_line) {
            line_infos.back().byteEnd += times;
        } else {
            uint16 end = line_infos.empty() ? 0 : line_infos.back().byteEnd;
            line_infos.push_back(LineInfo{.sourceLine = source_line, .byteStart = end, .byteEnd = (uint16) (end + times)});
        }
    }

    uint64 LineNumberTable::get_source_line(uint32 byte_line) const {
        for (auto line_info: line_infos) {
            if (line_info.byteStart <= byte_line && byte_line < line_info.byteEnd) {
                return line_info.sourceLine;
            }
        }
        throw IllegalAccessError(std::format("no source line mapping is present for byte line {}", byte_line));
    }

    uint32 MatchTable::perform(Obj *value) const {
        // Info: improve this to perform fast matching in case of integer values
        if (is<ObjInt>(value)) {
            auto val = cast<ObjInt>(value)->value();
            return cases[val].get_location();
        }
        for (auto kase: cases) {
            if (kase.get_value() == value)
                return kase.get_location();
        }
        return default_location;
    }
}    // namespace spade
