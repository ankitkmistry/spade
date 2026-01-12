#pragma once

#include "memory/manager.hpp"
#include "utils/common.hpp"

namespace spade::basic
{
    class BasicMemoryManager final : public MemoryManager {
      public:
        SWAN_EXPORT BasicMemoryManager(SpadeVM *vm = null) : MemoryManager(vm) {}

        SWAN_EXPORT void *allocate(size_t size);
        SWAN_EXPORT void post_allocation(Obj *obj);
        SWAN_EXPORT void deallocate(void *pointer);
        SWAN_EXPORT void collect_garbage();
    };
}    // namespace spade::basic
