#include "module.hpp"
#include "ee/thread.hpp"

namespace spade
{
    ObjModule::ObjModule(const Sign &sign) : Obj(null), sign(sign) {
        this->tag = ObjTag::MODULE;
        this->member_slots = member_slots;
    }

    Obj *ObjModule::copy() const {
        return (Obj *) this;
    }

    bool ObjModule::truth() const {
        return true;
    }

    string ObjModule::to_string() const {
        return std::format("<module {}>", sign.to_string());
    }

    ObjModule *ObjModule::current() {
        if (const auto thread = Thread::current()) {
            const auto state = thread->get_state();
            if (const auto frame = state->get_frame(); frame > state->get_call_stack())
                return frame->get_module();
        }
        return null;
    }
}    // namespace spade
