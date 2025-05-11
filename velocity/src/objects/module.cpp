#include "module.hpp"
#include "ee/thread.hpp"

namespace spade
{
    string ObjModule::get_absolute_path() {
        if (!path.is_absolute())
            path = std::filesystem::current_path() / path;
        return path.string();
    }

    string ObjModule::get_module_name() const {
        return path.filename().string();
    }

    ObjModule::ObjModule(const Sign &sign, const fs::path &path, const vector<Obj *> &constant_pool, const vector<string> &dependencies,
                         const ElpInfo &elp)
        : Obj(sign, null, null), path(path), constant_pool(constant_pool), dependencies(dependencies), elp(elp) {}

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
