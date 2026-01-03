#include "method.hpp"
#include "callable.hpp"
#include "frame.hpp"
#include "../ee/obj.hpp"
#include "../memory/memory.hpp"

namespace spade
{
    ObjMethod::ObjMethod(Kind kind, const Sign &sign, const FrameTemplate &frame, const Table<Type *> &type_params)
        : ObjCallable(ObjTag::METHOD, kind, sign), frame_template(frame), type_params(type_params) {}

    // TODO: implement these after VM threads

    void ObjMethod::call(const vector<Obj *> &args) {}

    void ObjMethod::call(Obj **args) {}

    string ObjMethod::to_string() const {
        const static string kind_names[] = {"function", "method", "constructor"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    Obj *ObjMethod::force_copy() const {
        const auto obj = halloc_mgr<ObjMethod>(info.manager, kind, sign, frame_template, type_params);
        // Copy members
        for (const auto &[name, slot]: member_slots) {
            obj->set_member(name, slot.get_value()->copy());
        }
        // Create new type params
        Table<Type *> new_type_params;
        for (const auto &[name, type_param]: type_params) {
            new_type_params[name] = cast<Type>(type_param->copy());
        }
        // TODO: implement reification
        // Obj::reify(obj, type_params, new_type_params);
        return obj;
    }
}    // namespace spade
