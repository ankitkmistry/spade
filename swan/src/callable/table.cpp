#include "table.hpp"
#include "memory/memory.hpp"
#include "utils/errors.hpp"

namespace spade
{
    VariableTable::VariableTable(const VariableTable &other) {
        values = vector<Obj *>(other.values.size());
        for (size_t i = 0; i < other.count(); i++) {
            values[i] = other.values[i]->copy();
        }
        metas = other.metas;
    }

    VariableTable::VariableTable(VariableTable &&other) noexcept {
        values = std::move(other.values);
        metas = std::move(other.metas);
    }

    VariableTable &VariableTable::operator=(const VariableTable &other) {
        values = vector<Obj *>(other.values.size());
        for (size_t i = 0; i < other.count(); i++) {
            values[i] = other.values[i]->copy();
        }
        metas = other.metas;
        return *this;
    }

    VariableTable &VariableTable::operator=(VariableTable &&other) noexcept {
        values = std::move(other.values);
        metas = std::move(other.metas);
        return *this;
    }

    ObjCapture *VariableTable::ramp_up(uint8_t i) {
        if (i >= values.size())
            throw IndexError("variable", i);
        auto &value = values[i];
        if (is<ObjCapture>(value))
            return cast<ObjCapture>(value);
        const auto pointer = halloc_mgr<ObjCapture>(value->get_info().manager, value);
        value = pointer;
        return pointer;
    }

    Obj *VariableTable::get(uint8_t i) const {
        if (i >= values.size())
            throw IndexError("variable", i);

        const auto value = values[i];
        // Don't return the pointer, instead get the dereferenced value
        return is<ObjCapture>(value) ? cast<ObjCapture>(value)->get() : value;
    }

    void VariableTable::set(uint8_t i, Obj *val) {
        if (i >= values.size())
            throw IndexError("variable", i);

        auto &value = values[i];
        // Don't set the value if it is a pointer, instead change the value it is pointing at
        if (is<ObjCapture>(value))
            cast<ObjCapture>(value)->set(val);
        else
            value = val;
    }

    const Table<string> &VariableTable::get_meta(uint8_t i) const {
        if (i >= metas.size())
            throw IndexError("variable", i);
        return metas[i];
    }

    Table<string> &VariableTable::get_meta(uint8_t i) {
        if (i >= metas.size())
            throw IndexError("variable", i);
        return metas[i];
    }

    void VariableTable::set_meta(uint8_t i, const Table<string> &meta) {
        if (i >= metas.size())
            throw IndexError("variable", i);
        metas[i] = meta;
    }

    Exception ExceptionTable::get_target(uint32_t pc, const Type *type) const {
        for (const auto &exception: exceptions)
            if (exception.get_from() <= pc && pc < exception.get_to() && exception.get_type() == type)
                return exception;
        return Exception::NO_EXCEPTION();
    }

    void LineNumberTable::add_line(uint8_t times, uint32_t source_line) {
        if (!line_infos.empty() && line_infos.back().sourceLine == source_line) {
            line_infos.back().byteEnd += times;
        } else {
            uint16_t end = line_infos.empty() ? 0 : line_infos.back().byteEnd;
            line_infos.push_back(LineInfo{.sourceLine = source_line, .byteStart = end, .byteEnd = (uint16_t) (end + times)});
        }
    }

    uint64_t LineNumberTable::get_source_line(uint32_t byte_line) const {
        for (const auto line_info: line_infos)
            if (line_info.byteStart <= byte_line && byte_line < line_info.byteEnd)
                return line_info.sourceLine;
        throw IllegalAccessError(std::format("no source line mapping is present for byte line {}", byte_line));
    }

    bool MatchTable::ObjEqual::operator()(Obj *lhs, Obj *rhs) const {
        if (lhs->get_tag() != rhs->get_tag())
            return false;
        switch (lhs->get_tag()) {
        case ObjTag::NULL_OBJ:
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
        case ObjTag::FOREIGN:
        case ObjTag::METHOD:
        case ObjTag::TYPE:
        case ObjTag::CAPTURE:
            return lhs == rhs;
        }
        throw Unreachable();
    }

    void MatchTable::ObjHash::hash(size_t &seed, Obj *obj) const {
        boost::hash_combine(seed, obj->get_tag());
        switch (obj->get_tag()) {
        case ObjTag::NULL_OBJ:
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
        case ObjTag::ARRAY: {
            const auto arr = cast<ObjArray>(obj);
            for (size_t i = 0; i < arr->count(); i++) {
                const auto obj = arr->get(i);
                hash(seed, obj);
            }
            break;
        }
        case ObjTag::OBJECT:
        case ObjTag::MODULE:
        case ObjTag::METHOD:
        case ObjTag::FOREIGN:
        case ObjTag::TYPE:
        case ObjTag::CAPTURE:
            boost::hash_combine(seed, obj);
            break;
        }
    }

    size_t MatchTable::ObjHash::operator()(Obj *obj) const {
        size_t seed = 0;
        hash(seed, obj);
        return seed;
    }

    MatchTable::MatchTable(const vector<Case> &cases, uint32_t default_location) : default_location(default_location) {
        for (const auto &kase: cases) {
            table[kase.get_value()] = kase.get_location();
        }
    }

    // #define NAIVE_MATCH_PERFORM

    uint32_t MatchTable::perform(Obj *value) const {
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
