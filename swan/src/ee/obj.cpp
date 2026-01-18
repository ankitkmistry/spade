#include "obj.hpp"
#include "thread.hpp"
#include "memory/memory.hpp"
#include "spimp/utils.hpp"
#include "utils/errors.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>

namespace spade
{
    Obj::Obj(ObjTag tag) : tag(tag), monitor(), type(null), member_slots() {}

    Obj::Obj(Type *type) : tag(OBJ_OBJECT), monitor(), type(type), member_slots() {
        set_type(type);
    }

    void Obj::set_type(Type *new_type) {
        if (type) {
            member_slots = new_type->get_member_slots();
        } else
            member_slots.clear();
        type = new_type;
    }

    Obj *Obj::copy() const {
        const auto obj = halloc_mgr<Obj>(info.manager, type);
        for (const auto &[name, slot]: member_slots) {
            obj->set_member(name, slot.get_value().copy());
            obj->set_flags(name, slot.get_flags());
        }
        return obj;
    }

    Ordering Obj::compare(const Obj *other) const {
        if (this == other)
            return Ordering::EQUAL;
        return Ordering::UNDEFINED;
    }

    Value Obj::operator<(const Obj *other) const {
        return Value(compare(other) == Ordering::LESS);
    }

    Value Obj::operator>(const Obj *other) const {
        return Value(compare(other) == Ordering::GREATER);
    }

    Value Obj::operator<=(const Obj *other) const {
        switch (compare(other)) {
        case Ordering::LESS:
        case Ordering::EQUAL:
            return Value(true);
        default:
            return Value(false);
        }
    }

    Value Obj::operator>=(const Obj *other) const {
        switch (compare(other)) {
        case Ordering::EQUAL:
        case Ordering::GREATER:
            return Value(true);
        default:
            return Value(false);
        }
    }

    Value Obj::operator==(const Obj *other) const {
        return Value(compare(other) == Ordering::EQUAL);
    }

    Value Obj::operator!=(const Obj *other) const {
        switch (compare(other)) {
        case Ordering::LESS:
        case Ordering::GREATER:
            return Value(true);
        default:
            return Value(false);
        }
    }

    string Obj::to_string() const {
        return std::format("<object of type {}>", type->get_sign().to_string());
    }

    Value Obj::get_member(const string &name) const {
        std::shared_lock member_slots_lk(member_slots_mtx);
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            return it->second.get_value();
        }
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
    }

    void Obj::set_member(const string &name, Value value) {
        std::unique_lock member_slots_lk(member_slots_mtx);
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            it->second.set_value(value);
            return;
        }
        // Create a new member slot if there is no such member
        member_slots.emplace(name, value);
    }

    Flags Obj::get_flags(const string &name) const {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            return it->second.get_flags();
        }
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
    }

    void Obj::set_flags(const string &name, Flags flags) {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            it->second.set_flags(flags);
            return;
        }
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
    }

    ObjString::ObjString(const string &str) : Obj(OBJ_STRING), str(str) {}

    ObjString::ObjString(const uint8_t *bytes, uint16_t len) : Obj(OBJ_STRING), str(bytes, bytes + len) {}

    ObjString *ObjString::concat(const ObjString *other) {
        return halloc_mgr<ObjString>(info.manager, str + other->str);
    }

    Ordering ObjString::compare(const Obj *other) const {
        if (other->get_tag() == OBJ_STRING) {
            const auto other_str = cast<const ObjString>(other);
            if (str < other_str->str)
                return Ordering::LESS;
            else if (str > other_str->str)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    ObjArray::ObjArray(size_t length) : Obj(OBJ_ARRAY), array(null), length(length) {
        if (length > 0) {
            array = std::make_unique<Value[]>(length);
        }
    }

    void ObjArray::for_each(const std::function<void(Value)> &func) const {
        for (size_t i = 0; i < length; i++) {
            func(array[i]);
        }
    }

    Value ObjArray::get(int64_t i) const {
        if (i < 0)
            i += length;
        if (i < 0 || i >= length)
            throw IndexError("array", i);
        return array[i];
    }

    Value ObjArray::get(size_t i) const {
        if (i >= length)
            throw IndexError("array", i);
        return array[i];
    }

    void ObjArray::set(int64_t i, Value value) {
        if (i < 0)
            i += length;
        if (i < 0 || i >= length)
            throw IndexError("array", i);
        array[i] = value;
    }

    void ObjArray::set(size_t i, Value value) {
        if (i >= length)
            throw IndexError("array", i);
        array[i] = value;
    }

    string ObjArray::to_string() const {
        string str;
        for (size_t i = 0; i < length; ++i) {
            str += array[i].to_string() + (i < length - 1 ? ", " : "");
        }
        return "[" + str + "]";
    }

    Obj *ObjArray::copy() const {
        auto new_array = halloc_mgr<ObjArray>(info.manager, length);
        for (size_t i = 0; i < length; i++) {
            new_array->set(i, get(i).copy());
        }
        return new_array;
    }

    Ordering ObjArray::compare(const Obj *other) const {
        if (other->get_tag() == OBJ_ARRAY) {
            const auto str = to_string();
            const auto other_str = other->to_string();
            if (str < other_str)
                return Ordering::LESS;
            else if (str > other_str)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    ObjModule::ObjModule(const Sign &sign) : Obj(OBJ_MODULE), sign(sign) {}

    string ObjModule::to_string() const {
        return std::format("<module {}>", sign.to_string());
    }

    ObjModule *ObjModule::current() {
        if (const auto thread = Thread::current()) {
            const auto &state = thread->get_state();
            if (const auto frame = state.get_frame(); frame > state.get_call_stack())
                return frame->get_module();
        }
        return null;
    }

    Type::Type(Kind kind, Sign sign, const vector<Sign> &supers) : Obj(OBJ_TYPE), kind(kind), sign(sign), supers(supers) {}

    string Type::to_string() const {
        static const string kind_names[] = {"class", "interface", "enum", "annotation", "type_parameter", "unresolved"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    ObjCapture::ObjCapture(Value value) : Obj(OBJ_CAPTURE), value(value) {}

    bool ObjCapture::truth() const {
        return !value.is_null();
    }

    string ObjCapture::to_string() const {
        return std::format("<pointer to {}>", value.to_string());
    }
}    // namespace spade
