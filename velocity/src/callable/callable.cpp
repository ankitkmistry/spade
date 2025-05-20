#include "callable.hpp"
#include "ee/vm.hpp"

namespace spade
{
    Obj *ObjCallable::invoke(const vector<Obj *> &args) {
        Thread *thread = Thread::current();
        call(args);
        return thread->get_state()->get_vm()->run(thread);
    }

    void ObjCallable::validate_call_site() {
        // if (const auto mgr = MemoryManager::current(); !mgr || mgr != info.manager)
        //     throw IllegalAccessError(std::format("invalid call site, cannot call {}", to_string()));
    }
}    // namespace spade
