#include "table.hpp"
#include "ee/obj.hpp"
#include "spimp/utils.hpp"
#include "utils/errors.hpp"
#include <boost/container_hash/hash_fwd.hpp>

namespace spade
{
    Exception ExceptionTable::get_target(uint32_t pc, const Type *type) const {
        for (const auto &exception: exceptions)
            if (exception.get_from() <= pc && pc < exception.get_to() && exception.get_type() == type)
                return exception;
        return Exception::NO_EXCEPTION();
    }

    void LineNumberTable::add_line(uint8_t times, uint32_t source_line) {
        if (!line_infos.empty() && line_infos.back().source_line == source_line) {
            line_infos.back().byte_end += times;
        } else {
            uint16_t end = line_infos.empty() ? 0 : line_infos.back().byte_end;
            line_infos.push_back(LineInfo{.source_line = source_line, .byte_start = end, .byte_end = (uint16_t) (end + times)});
        }
    }

    uint64_t LineNumberTable::get_source_line(uint32_t byte_line) const {
        for (const auto line_info: line_infos)
            if (line_info.byte_start <= byte_line && byte_line < line_info.byte_end)
                return line_info.source_line;
        throw IllegalAccessError(std::format("no source line mapping is present for byte line {}", byte_line));
    }

    bool MatchTable::ValueEqual::operator()(Value lhs, Value rhs) const {
        if (lhs.get_tag() != rhs.get_tag())
            return false;
        switch (lhs.get_tag()) {
        case VALUE_NULL:
            return true;
        case VALUE_BOOL:
            return lhs.as_bool() == rhs.as_bool();
        case VALUE_CHAR:
            return lhs.as_char() == rhs.as_char();
        case VALUE_INT:
            return lhs.as_int() == rhs.as_int();
        case VALUE_FLOAT:
            return lhs.as_float() == rhs.as_float();
        case VALUE_OBJ: {
            const auto lhs_obj = lhs.as_obj();
            const auto rhs_obj = rhs.as_obj();

            if (lhs_obj->get_tag() != rhs_obj->get_tag())
                return false;
            switch (lhs_obj->get_tag()) {
            case OBJ_STRING:
                return lhs_obj->to_string() == rhs_obj->to_string();
            case OBJ_ARRAY: {
                const auto lhs_arr = cast<ObjArray>(lhs_obj);
                const auto rhs_arr = cast<ObjArray>(rhs_obj);
                if (lhs_arr->count() != rhs_arr->count())
                    return false;
                for (size_t i = 0; i < lhs_arr->count(); i++) {
                    if (!MatchTable::ValueEqual()(lhs_arr->get(i), rhs_arr->get(i)))
                        return false;
                }
                return true;
            }
            case OBJ_OBJECT:
            case OBJ_MODULE:
            case OBJ_FOREIGN:
            case OBJ_METHOD:
            case OBJ_TYPE:
            case OBJ_CAPTURE:
                return lhs_obj == rhs_obj;
            }
        }
        }
        throw Unreachable();
    }

    void MatchTable::ValueHash::hash(size_t &seed, Value value) const {
        boost::hash_combine(seed, value.get_tag());
        switch (value.get_tag()) {
        case VALUE_NULL:
            break;
        case VALUE_BOOL:
            boost::hash_combine(seed, value.as_bool());
            break;
        case VALUE_CHAR:
            boost::hash_combine(seed, value.as_char());
            break;
        case VALUE_INT:
            boost::hash_combine(seed, value.as_int());
            break;
        case VALUE_FLOAT:
            boost::hash_combine(seed, value.as_float());
            break;
        case VALUE_OBJ: {
            const auto obj = value.as_obj();
            switch (obj->get_tag()) {
            case OBJ_STRING:
                boost::hash_combine(seed, obj->to_string());
                break;
            case OBJ_ARRAY: {
                const auto arr = cast<ObjArray>(obj);
                for (size_t i = 0; i < arr->count(); i++) {
                    const auto obj = arr->get(i);
                    hash(seed, obj);
                }
                break;
            }
            case OBJ_OBJECT:
            case OBJ_MODULE:
            case OBJ_METHOD:
            case OBJ_FOREIGN:
            case OBJ_TYPE:
            case OBJ_CAPTURE:
                boost::hash_combine(seed, obj);
                break;
            }
            break;
        }
        }
    }

    size_t MatchTable::ValueHash::operator()(Value value) const {
        size_t seed = 0;
        hash(seed, value);
        return seed;
    }

    MatchTable::MatchTable(const vector<Case> &cases, uint32_t default_location) : default_location(default_location) {
        for (const auto &kase: cases) {
            table.emplace(kase.get_value(), kase.get_location());
        }
    }

    // #define NAIVE_MATCH_PERFORM

    uint32_t MatchTable::perform(Value value) const {
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
