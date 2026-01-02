#include "obj.hpp"
#include "memory/memory.hpp"
#include "spimp/utils.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace spade
{
    Obj::Obj(ObjTag tag) : tag(tag), monitor(), type(null), member_slots() {}

    Obj::Obj(Type *type) : tag(ObjTag::OBJECT), monitor(), type(type), member_slots() {
        // TODO: Compute member slots from type
    }

    void Obj::set_type(Type *new_type) {
        // TODO: Also change member_slots
        type = new_type;
    }

    Obj *Obj::copy() const {
        // TODO: Implement this after memory allocation
        return null;
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
        } else if (const auto it = type->member_slots.find(name); it != type->member_slots.end()) {
            if (it->second.get_flags().is_static())
                return it->second.get_value();
        }
        // TODO: propagate the assert as an error
        assert(false && "invalid member access");
        return null;
    }

    void Obj::set_member(const string &name, Obj *value) {
        if (const auto it = member_slots.find(name); it != member_slots.end()) {
            it->second.set_value(value);
        } else if (const auto it = type->member_slots.find(name); it != type->member_slots.end()) {
            if (it->second.get_flags().is_static())
                it->second.set_value(value);
        }
        // TODO: propagate the assert as an error
        assert(false && "invalid member access");
        return;
    }

    Ordering ObjNull::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::NULL_OBJ)
            return Ordering::EQUAL;
        return Ordering::UNDEFINED;
    }

    Ordering ObjBool::compare(const Obj *other) const {
        if (other->get_tag() == ObjTag::BOOL) {
            const auto other_bool = cast<ObjBool>(other);
            if (b == other_bool->b)
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    ObjBool *ObjBool::operator!() const {
        return halloc_mgr<ObjBool>(info.manager, !b);
    }

    Ordering ObjChar::compare(const Obj *other) const {
        if (is<ObjChar>(other)) {
            auto other_char = cast<ObjChar>(other);
            if (c < other_char->c)
                return Ordering::LESS;
            else if (c > other_char->c)
                return Ordering::GREATER;
            else
                return Ordering::EQUAL;
        }
        return Ordering::UNDEFINED;
    }

    Ordering ObjString::compare(const Obj *other) const {
        if (is<ObjString>(other)) {
            auto other_str = cast<ObjString>(other);
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
        // TODO: propagate the assert as an error
        assert(0 <= i && i < length && "index out of range");
        return array[i];
    }

    Obj *ObjArray::get(size_t i) const {
        // TODO: propagate the assert as an error
        assert(i < length && "index out of range");
        return array[i];
    }

    void ObjArray::set(int64_t i, Obj *value) {
        if (i < 0)
            i += length;
        // TODO: propagate the assert as an error
        assert(0 <= i && i < length && "index out of range");
        array[i] = value;
    }

    void ObjArray::set(size_t i, Obj *value) {
        // TODO: propagate the assert as an error
        assert(i < length && "index out of range");
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
        if (is<ObjArray>(other)) {
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

    Ordering ObjInt::compare(const Obj *other) const {
        if (is<ObjInt>(other)) {
            const auto other_int = cast<ObjInt>(other);
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

    Ordering ObjFloat::compare(const Obj *other) const {
        if (is<ObjInt>(other)) {
            const auto other_float = cast<ObjFloat>(other);
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
        // TODO: implement this
        // if (const auto thread = Thread::current()) {
        //     const auto state = thread->get_state();
        //     if (const auto frame = state->get_frame(); frame > state->get_call_stack())
        //         return frame->get_module();
        // }
        return null;
    }
}    // namespace spade
