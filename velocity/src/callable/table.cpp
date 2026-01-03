#include <cstddef>
#include <boost/functional/hash.hpp>

#include "table.hpp"
#include "utils/exceptions.hpp"
#include "objects/inbuilt_types.hpp"
#include "objects/pointer.hpp"
#include "memory/memory.hpp"

namespace spade
{
    VariableTable::VariableTable(const VariableTable &other) {
        values = vector<Obj *>(other.values.size());
        for (size_t i = 0; i < other.count(); i++) {
            values[i] = Obj::create_copy(other.values[i]);
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
            values[i] = Obj::create_copy(other.values[i]);
        }
        metas = other.metas;
        return *this;
    }

    VariableTable &VariableTable::operator=(VariableTable &&other) noexcept {
        values = std::move(other.values);
        metas = std::move(other.metas);
        return *this;
    }

    ObjPointer *VariableTable::ramp_up(uint8 i) {
        if (i >= values.size())
            throw IndexError("variable", i);
        auto &value = values[i];
        if (is<ObjPointer>(value))
            return cast<ObjPointer>(value);
        const auto pointer = halloc_mgr<ObjPointer>(value->get_info().manager, value);
        value = pointer;
        return pointer;
    }

    Obj *VariableTable::get(uint8 i) const {
        if (i >= values.size())
            throw IndexError("variable", i);

        const auto value = values[i];
        // Don't return the pointer, instead get the dereferenced value
        return is<ObjPointer>(value) ? cast<ObjPointer>(value)->get() : value;
    }

    void VariableTable::set(uint8 i, Obj *val) {
        if (i >= values.size())
            throw IndexError("variable", i);

        auto &value = values[i];
        // Don't set the value if it is a pointer, instead change the value it is pointing at
        if (is<ObjPointer>(value))
            cast<ObjPointer>(value)->set(val);
        else
            value = val;
    }

    const Table<string> &VariableTable::get_meta(uint8 i) const {
        if (i >= metas.size())
            throw IndexError("variable", i);
        return metas[i];
    }

    Table<string> &VariableTable::get_meta(uint8 i) {
        if (i >= metas.size())
            throw IndexError("variable", i);
        return metas[i];
    }

    void VariableTable::set_meta(uint8 i, const Table<string> &meta) {
        if (i >= metas.size())
            throw IndexError("variable", i);
        metas[i] = meta;
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
            case ObjTag::POINTER:
