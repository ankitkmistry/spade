#include <boost/functional/hash.hpp>
#include <cstddef>

#include "table.hpp"
#include "boost/container_hash/hash_fwd.hpp"
#include "objects/inbuilt_types.hpp"
#include "objects/int.hpp"
#include "spimp/error.hpp"
#include "spimp/utils.hpp"
#include "utils/exceptions.hpp"

namespace spade
{
    NamedRef::NamedRef(const NamedRef &other) : name(other.name), value(Obj::create_copy(other.value)), meta(other.meta) {}

    NamedRef::NamedRef(NamedRef &&other) noexcept : name(std::move(other.name)), value(other.value), meta(std::move(other.meta)) {}

    NamedRef &NamedRef::operator=(const NamedRef &other) {
        name = other.name;
        value = Obj::create_copy(other.value);
        meta = other.meta;
        return *this;
    }

    NamedRef &NamedRef::operator=(NamedRef &&other) noexcept {
        name = std::move(other.name);
        value = other.value;
        meta = std::move(other.meta);
        return *this;
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
        new_args.args.reserve(args.size());
        for (const auto arg: args) {
            new_args.add_arg(arg);
        }
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
        new_locals.locals.reserve(locals.size());
        for (const auto local: locals) {
            new_locals.add_local(local);
        }
        new_locals.closures.reserve(closures.size());
        for (const auto closure: closures) {
            new_locals.add_closure(closure);
        }
        return new_locals;
    }

    Exception ExceptionTable::get_target(uint32 pc, const Type *type) const {
        for (const auto &exception: exceptions)
            if (exception.get_from() <= pc && pc < exception.get_to() && exception.get_type() == type)
                return exception;
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
        for (const auto line_info: line_infos)
            if (line_info.byteStart <= byte_line && byte_line < line_info.byteEnd)
                return line_info.sourceLine;
        throw IllegalAccessError(std::format("no source line mapping is present for byte line {}", byte_line));
    }

    bool MatchTable::ObjEqual::operator()(Obj *lhs, Obj *rhs) const {
        if (lhs->get_tag() != rhs->get_tag())
            return false;
        switch (lhs->get_tag()) {
            case ObjTag::NULL_:
                return true;
            case ObjTag::BOOL:
                return lhs->truth() == rhs->truth();
            case ObjTag::CHAR:
            case ObjTag::STRING:
            case ObjTag::INT:
            case ObjTag::FLOAT:
                return lhs->to_string() == rhs->to_string();
            case ObjTag::ARRAY: {
                const auto lhs_arr = cast<ObjArray>(lhs);
                const auto rhs_arr = cast<ObjArray>(rhs);
                if (lhs_arr->count() != rhs_arr->count())
                    return false;
                for (size_t i = 0; i < lhs_arr->count(); i++) {
                    if (!MatchTable::ObjEqual()(lhs_arr->get(i), rhs_arr->get(i)))
                        return false;
                }
                return true;
            }
            case ObjTag::OBJECT:
            case ObjTag::MODULE:
            case ObjTag::METHOD:
            case ObjTag::TYPE:
            case ObjTag::TYPE_PARAM:
                return lhs == rhs;
        }
        throw Unreachable();
    }

    void MatchTable::ObjHash::hash_combine(size_t &seed, ObjArray *arr) const {
        for (size_t i = 0; i < arr->count(); i++) {
            const auto obj = arr->get(i);
            switch (obj->get_tag()) {
                case ObjTag::NULL_:
                    break;
                case ObjTag::BOOL:
                    boost::hash_combine(seed, obj->truth());
                    break;
                case ObjTag::CHAR:
                case ObjTag::STRING:
                case ObjTag::INT:
                case ObjTag::FLOAT:
                    boost::hash_combine(seed, obj->to_string());
                    break;
                case ObjTag::ARRAY:
                    hash_combine(seed, cast<ObjArray>(obj));
                    break;
                case ObjTag::OBJECT:
                case ObjTag::MODULE:
                case ObjTag::METHOD:
                case ObjTag::TYPE:
                case ObjTag::TYPE_PARAM:
                    boost::hash_combine(seed, obj);
                    break;
            }
        }
    }

    size_t MatchTable::ObjHash::operator()(Obj *obj) const {
        size_t seed = 0;
        boost::hash_combine(seed, obj->get_tag());
        switch (obj->get_tag()) {
            case ObjTag::NULL_:
                break;
            case ObjTag::BOOL:
                boost::hash_combine(seed, obj->truth());
                break;
            case ObjTag::CHAR:
            case ObjTag::STRING:
            case ObjTag::INT:
            case ObjTag::FLOAT:
                boost::hash_combine(seed, obj->to_string());
                break;
            case ObjTag::ARRAY:
                hash_combine(seed, cast<ObjArray>(obj));
                break;
            case ObjTag::OBJECT:
            case ObjTag::MODULE:
            case ObjTag::METHOD:
            case ObjTag::TYPE:
            case ObjTag::TYPE_PARAM:
                boost::hash_combine(seed, obj);
                break;
        }
        return seed;
    }

    MatchTable::MatchTable(const vector<Case> &cases, uint32 default_location) : default_location(default_location) {
        for (const auto &kase: cases) {
            table[kase.get_value()] = kase.get_location();
        }
    }

    // #define NAIVE_MATCH_PERFORM

    uint32 MatchTable::perform(Obj *value) const {
#ifdef NAIVE_MATCH_PERFORM
        // Info: improve this to perform fast matching in case of integer values
        for (const auto &[case_value, location]: table) {
            if (MatchTable::ObjEqual()(value, case_value))
                return location;
        }
        return default_location;
#else
        if (const auto it = table.find(value); it != table.end())
            return it->second;
        return default_location;
#endif
    }
}    // namespace spade
