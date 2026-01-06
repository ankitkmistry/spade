#include "obj.hpp"
#include "thread.hpp"
#include "memory/memory.hpp"
#include "spimp/utils.hpp"
#include "utils/errors.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace spade
{
    Obj::Obj(ObjTag tag) : tag(tag), monitor(), type(null), member_slots() {}

    Obj::Obj(Type *type) : tag(ObjTag::OBJECT), monitor(), type(type), member_slots() {
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
            obj->set_member(name, slot.get_value()->copy());
        }
        return obj;
    }

    Ordering Obj::compare(const Obj *other) const {
        if (this == other)
            return Ordering::EQUAL;
        return Ordering::UNDEFINED;
    }

    ObjBool *Obj::operator<(const Obj *other) const {
        return halloc_mgr<ObjBool>(info.manager, compare(other) == Ordering::LESS);
    }

    ObjBool *Obj::operator>(const Obj *other) const {
        return halloc_mgr<ObjBool>(info.manager, compare(other) == Ordering::GREATER);
    }

    ObjBool *Obj::operator<=(const Obj *other) const {
        switch (compare(other)) {
        case Ordering::LESS:
        case Ordering::EQUAL:
            return halloc_mgr<ObjBool>(info.manager, true);
        default:
            return halloc_mgr<ObjBool>(info.manager, false);
        }
    }

    ObjBool *Obj::operator>=(const Obj *other) const {
        switch (compare(other)) {
        case Ordering::EQUAL:
        case Ordering::GREATER:
            return halloc_mgr<ObjBool>(info.manager, true);
        default:
            return halloc_mgr<ObjBool>(info.manager, false);
        }
    }

    ObjBool *Obj::operator==(const Obj *other) const {
        return halloc_mgr<ObjBool>(info.manager, compare(other) == Ordering::EQUAL);
    }

    ObjBool *Obj::operator!=(const Obj *other) const {
        switch (compare(other)) {
        case Ordering::LESS:
        case Ordering::GREATER:
            return halloc_mgr<ObjBool>(info.manager, true);
        default:
            return halloc_mgr<ObjBool>(info.manager, false);
        }
    }

    string Obj::to_string() const {
        return std::format("<object of type {}>", type->get_sign().to_string());
    }

    Obj *Obj::get_member(const string &name) const {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            return it->second.get_value();
        } else if (type) {
            if (const auto it = type->member_slots.find(name); it != type->member_slots.end()) {
                if (it->second.get_flags().is_static())
                    return it->second.get_value();
            }
        }
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
    }

    void Obj::set_member(const string &name, Obj *value) {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            it->second.set_value(value);
            return;
        } else if (type) {
            if (const auto it = type->member_slots.find(name); it != type->member_slots.end()) {
                if (it->second.get_flags().is_static()) {
                    it->second.set_value(value);
                    return;
                }
            }
        }
        // Create a new member slot if there is no such member
        member_slots[name] = MemberSlot(value);
    }

    Flags Obj::get_flags(const string &name) const {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            return it->second.get_flags();
        } else if (type) {
            if (const auto it = type->member_slots.find(name); it != type->member_slots.end()) {
                if (it->second.get_flags().is_static())
                    return it->second.get_flags();
            }
        }
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
    }

    void Obj::set_flags(const string &name, Flags flags) {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            it->second.set_flags(flags);
            return;
        } else if (type) {
            if (const auto it = type->member_slots.find(name); it != type->member_slots.end()) {
                if (it->second.get_flags().is_static()) {
                    it->second.set_flags(flags);
                    return;
                }
            }
        }
        throw IllegalAccessError(std::format("cannot find member: {} in {}", name, to_string()));
}

    ObjNull::ObjNull() : Obj(ObjTag::NULL_OBJ) {}

    Ordering ObjNull::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::NULL_OBJ)
            return Ordering::EQUAL;
        return Ordering::UNDEFINED;
    }

    ObjBool::ObjBool(bool value) : Obj(ObjTag::BOOL), b(value) {}

    Ordering ObjBool::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::BOOL) {
            const auto other_bool = cast<const ObjBool>(other);
            if (b == other_bool->b)
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    ObjBool *ObjBool::operator!() const {
        return halloc_mgr<ObjBool>(info.manager, !b);
    }

    ObjChar::ObjChar(const char c) : Obj(ObjTag::CHAR), c(c) {}

    Ordering ObjChar::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::CHAR) {
            const auto other_char = cast<const ObjChar>(other);
            if (c < other_char->c)
                return Ordering::LESS;
            else if (c > other_char->c)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    ObjString::ObjString(const string &str) : Obj(ObjTag::STRING), str(str) {}

    ObjString::ObjString(const uint8_t *bytes, uint16_t len) : Obj(ObjTag::STRING), str(bytes, bytes + len) {}

    Ordering ObjString::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::STRING) {
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

    ObjArray::ObjArray(size_t length) : Obj(ObjTag::ARRAY), array(null), length(length) {
        if (length > 0) {
            array = std::make_unique<Obj *[]>(length);
        }
    }

    void ObjArray::for_each(const std::function<void(Obj *)> &func) const {
        for (size_t i = 0; i < length; i++) {
            func(array[i]);
        }
    }

    Obj *ObjArray::get(int64_t i) const {
        if (i < 0)
            i += length;
        if (i < 0 || i >= length)
            throw IndexError("array", i);
        return array[i];
    }

    Obj *ObjArray::get(size_t i) const {
        if (i >= length)
            throw IndexError("array", i);
        return array[i];
    }

    void ObjArray::set(int64_t i, Obj *value) {
        if (i < 0)
            i += length;
        if (i < 0 || i >= length)
            throw IndexError("array", i);
        array[i] = value;
    }

    void ObjArray::set(size_t i, Obj *value) {
        if (i >= length)
            throw IndexError("array", i);
        array[i] = value;
    }

    string ObjArray::to_string() const {
        string str;
        for (size_t i = 0; i < length; ++i) {
            str += array[i]->to_string() + (i < length - 1 ? ", " : "");
        }
        return "[" + str + "]";
    }

    Obj *ObjArray::copy() const {
        auto new_array = halloc_mgr<ObjArray>(info.manager, length);
        for (size_t i = 0; i < length; i++) {
            new_array->set(i, get(i)->copy());
        }
        return new_array;
    }

    Ordering ObjArray::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::ARRAY) {
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

    ObjInt::ObjInt(int64_t val) : ObjNumber(ObjTag::INT), val(val) {}

    Ordering ObjInt::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::INT) {
            const auto other_int = cast<const ObjInt>(other);
            if (val < other_int->val)
                return Ordering::LESS;
            else if (val > other_int->val)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    Obj *ObjInt::operator-() const {
        return halloc_mgr<ObjInt>(info.manager, -val);
    }

    Obj *ObjInt::power(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, std::pow(val, cast<const ObjInt>(n)->val));
    }

    Obj *ObjInt::operator+(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val + cast<const ObjInt>(n)->val);
    }

    Obj *ObjInt::operator-(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val - cast<const ObjInt>(n)->val);
    }

    Obj *ObjInt::operator*(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val * cast<const ObjInt>(n)->val);
    }

    Obj *ObjInt::operator/(const ObjNumber *n) const {
        return halloc_mgr<ObjInt>(info.manager, val / cast<const ObjInt>(n)->val);
    }

    ObjInt *ObjInt::operator~() const {
        return halloc_mgr<ObjInt>(info.manager, ~val);
    }

    ObjInt *ObjInt::operator%(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val % n.val);
    }

    ObjInt *ObjInt::operator<<(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val << n.val);
    }

    ObjInt *ObjInt::operator>>(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val >> n.val);
    }

    ObjInt *ObjInt::operator&(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val & n.val);
    }

    ObjInt *ObjInt::operator|(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val | n.val);
    }

    ObjInt *ObjInt::operator^(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val ^ n.val);
    }

    ObjInt *ObjInt::unsigned_right_shift(const ObjInt &n) const {
        return halloc_mgr<ObjInt>(info.manager, val & 0x7fffffff >> n.val);
    }

    ObjFloat::ObjFloat(double val) : ObjNumber(ObjTag::FLOAT), val(val) {}

    Ordering ObjFloat::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::INT) {
            const auto other_float = cast<const ObjFloat>(other);
            if (val < other_float->val)
                return Ordering::LESS;
            else if (val > other_float->val)
                return Ordering::GREATER;
            else if (val == other_float->val)
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    Obj *ObjFloat::operator-() const {
        return halloc_mgr<ObjFloat>(info.manager, -val);
    }

    Obj *ObjFloat::power(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, std::pow(val, cast<const ObjFloat>(n)->val));
    }

    Obj *ObjFloat::operator+(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val + cast<const ObjFloat>(n)->val);
    }

    Obj *ObjFloat::operator-(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val - cast<const ObjFloat>(n)->val);
    }

    Obj *ObjFloat::operator*(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val * cast<const ObjFloat>(n)->val);
    }

    Obj *ObjFloat::operator/(const ObjNumber *n) const {
        return halloc_mgr<ObjFloat>(info.manager, val / cast<const ObjFloat>(n)->val);
    }

    ObjModule::ObjModule(const Sign &sign) : Obj(ObjTag::MODULE), sign(sign) {}

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

    Type::Type(Kind kind, Sign sign, const Table<Type *> &type_params, const vector<Sign> &supers)
        : Obj(ObjTag::TYPE), kind(kind), sign(sign), type_params(type_params), supers(supers) {}

    string Type::to_string() const {
        static const string kind_names[] = {"class", "interface", "enum", "annotation", "type_parameter", "unresolved"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    Obj *Type::force_copy() const {
        const auto obj = halloc_mgr<Type>(info.manager, kind, sign, type_params, supers);
        // Copy members
        for (const auto &[name, slot]: member_slots) {
            obj->set_member(name, slot.get_value()->copy());
        }
        // Create new type params
        Table<Type *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = cast<Type>(type_param->force_copy());
        }
        // TODO: implement this
        // Obj::reify(obj, type_params, new_type_params);
        return obj;
    }

    ObjCapture::ObjCapture(Obj *value) : Obj(ObjTag::CAPTURE), value(value) {}

    bool ObjCapture::truth() const {
        return value && value->get_tag() != ObjTag::NULL_OBJ;
    }

    string ObjCapture::to_string() const {
        return value ? std::format("<pointer to {}>", value->to_string()) : "<pointer to null>";
    }
}    // namespace spade
