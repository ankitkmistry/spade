#include "module.hpp"
#include "ee/thread.hpp"

namespace spade
{
    ObjModule::ObjModule(const Sign &sign, const fs::path &path, const vector<Obj *> &constant_pool, const Table<MemberSlot> &member_slots)
        : Obj(sign, null, null), path(path), constant_pool(constant_pool) {
        this->member_slots = member_slots;
    }

    Obj *ObjModule::copy() {
        return this;
    }

    bool ObjModule::truth() const {
        return true;
    }

    string ObjModule::to_string() const {
        return std::format("<module {}>", sign.to_string());
    }

    ObjModule *ObjModule::current() {
        if (auto thread = Thread::current(); thread != null) {
            auto state = thread->get_state();
            if (auto frame = state->get_frame(); frame > state->get_call_stack()) {
                return frame->get_method()->get_module();
            }
        }
        return null;
    }
}    // namespace spade
