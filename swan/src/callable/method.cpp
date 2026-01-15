#include "method.hpp"
#include "callable.hpp"
#include "ee/thread.hpp"
#include "frame.hpp"
#include "ee/obj.hpp"
#include "utils/errors.hpp"

namespace spade
{
    ObjMethod::ObjMethod(Kind kind, const Sign &sign, const FrameTemplate &frame) : ObjCallable(OBJ_METHOD, kind, sign), frame_template(frame) {}

    void ObjMethod::call(Obj *self, const vector<Value > &args) {
        validate_call_site();

        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize(this);

        const auto arg_count = new_frame.get_args().count();
        if (arg_count < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", arg_count, args.size()));
        if (arg_count > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", arg_count, args.size()));

        for (int i = 0; i < arg_count; i++) {
            new_frame.get_args().set(i, args[i]);
        }
        thread->get_state().push_frame(std::move(new_frame));
        if (self) {
            thread->get_state().get_frame()->get_locals().set(0, self);
        }
    }

    void ObjMethod::call(Obj *self, Value *args) {
        validate_call_site();
        Thread *thread = Thread::current();
        auto new_frame = frame_template.initialize(this);
        for (int i = 0; i < new_frame.get_args().count(); i++) {
            new_frame.get_args().set(i, args[i]);
        }
        thread->get_state().push_frame(std::move(new_frame));
        if (self) {
            thread->get_state().get_frame()->get_locals().set(0, self);
        }
    }

    string ObjMethod::to_string() const {
        const static string kind_names[] = {"function", "method", "constructor"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }
}    // namespace spade
