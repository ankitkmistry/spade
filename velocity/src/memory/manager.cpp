#include "manager.hpp"
#include "ee/thread.hpp"
#include "ee/vm.hpp"

namespace spade
{
    MemoryManager *MemoryManager::current() {
        if (const auto thread = Thread::current())
            return thread->get_state()->get_vm()->get_memory_manager();
        return null;
    }
}    // namespace spade
