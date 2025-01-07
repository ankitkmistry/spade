#include "module.hpp"
#include "../ee/thread.hpp"

namespace spade
{
    string ObjModule::getAbsolutePath() {
        if (!path.is_absolute()) path = std::filesystem::current_path() / path;
        return path.string();
    }

    string ObjModule::getModuleName() const { return path.filename().string(); }

    ObjModule::ObjModule(const Sign &sign, const fs::path &path, const vector<Obj *> &constantPool, const ElpInfo &elp)
        : Obj(sign, null, null), path(path), constantPool(constantPool), elp(elp) {}

    Obj *ObjModule::copy() { return this; }

    bool ObjModule::truth() const { return true; }

    string ObjModule::toString() const { return std::format("<module {}>", sign.toString()); }

    ObjModule *ObjModule::current() {
        if (auto thread = Thread::current(); thread != null) {
            auto state = thread->getState();
            if (auto frame = state->getFrame(); frame > state->getCallStack()) { return frame->getMethod()->getModule(); }
        }
        return null;
    }
} // namespace spade
