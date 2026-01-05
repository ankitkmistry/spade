#include "basic_manager.hpp"
#include <cstdlib>

namespace spade::basic
{
    void *BasicMemoryManager::allocate(size_t size) {
        return std::malloc(size);
    }

    void BasicMemoryManager::post_allocation(Obj *) {}

    void BasicMemoryManager::deallocate(void *pointer) {
        std::free(pointer);
    }

    void BasicMemoryManager::collect_garbage() {
        // TODO: implement this
    }
}    // namespace spade::basic
