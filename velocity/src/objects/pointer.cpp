#include "pointer.hpp"
#include "obj.hpp"

namespace spade
{
    ObjPointer::ObjPointer(Obj *value) : Obj(null), value(value) {
        this->tag = ObjTag::CAPTURE;
    }

    bool ObjPointer::truth() const {
        return value && !is<ObjNull>(value);
    }

    string ObjPointer::to_string() const {
        return value ? std::format("<pointer to {}>", value->to_string()) : "<pointer to null>";
    }
}    // namespace spade
