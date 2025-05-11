#include "manager.hpp"
#include "ee/vm.hpp"

namespace spade
{
    MemoryManager *MemoryManager::current() {
        if (auto vm = SpadeVM::current(); vm != null)
            return vm->get_memory_manager();
        return null;
    }
}    // namespace spade
