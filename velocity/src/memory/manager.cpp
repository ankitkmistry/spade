#include "manager.hpp"
#include "../ee/vm.hpp"

namespace spade
{
    MemoryManager *MemoryManager::current() {
        if (auto vm = SpadeVM::current(); vm != null)
            return vm->getMemoryManager();
        return null;
    }
}    // namespace spade
