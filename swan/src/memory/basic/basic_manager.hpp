#pragma once

#include "memory/manager.hpp"
#include "utils/common.hpp"

namespace spade::basic
{
    class BasicMemoryManager final : public MemoryManager {
      public:
        BasicMemoryManager(SpadeVM *vm = null) : MemoryManager(vm) {}

        void *allocate(size_t size);
        void post_allocation(Obj *obj);
        void deallocate(void *pointer);
        void collect_garbage();
    };
}    // namespace spade::basic
