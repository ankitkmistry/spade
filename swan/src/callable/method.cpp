#include "method.hpp"
#include "callable.hpp"
#include "ee/thread.hpp"
#include "frame.hpp"
#include "../ee/obj.hpp"
#include "../memory/memory.hpp"
#include "utils/errors.hpp"

namespace spade
{
    ObjMethod::ObjMethod(Kind kind, const Sign &sign, const FrameTemplate &frame, const Table<Type *> &type_params)
        : ObjCallable(ObjTag::METHOD, kind, sign), frame_template(frame), type_params(type_params) {}

    void ObjMethod::call(const vector<Obj *> &args) {
        validate_call_site();
        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize(this);
        if (new_frame.get_args().count() < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", new_frame.get_args().count(), args.size()));
        if (new_frame.get_args().count() > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", new_frame.get_args().count(), args.size()));
        for (int i = 0; i < new_frame.get_args().count(); i++) {
            new_frame.get_args().set(i, args[i]);
        }
        thread->get_state().push_frame(std::move(new_frame));
    }

    void ObjMethod::call(Obj **args) {
        validate_call_site();
        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize(this);
        for (int i = 0; i < new_frame.get_args().count(); i++) {
            new_frame.get_args().set(i, args[i]);
        }
        thread->get_state().push_frame(std::move(new_frame));
    }

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
            new_type_params[name] = cast<Type>(type_param->force_copy());
        }
        // TODO: implement reification
        // Obj::reify(obj, type_params, new_type_params);
        return obj;
    }
}    // namespace spade
