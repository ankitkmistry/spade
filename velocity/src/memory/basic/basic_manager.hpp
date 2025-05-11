#pragma once

#include "memory/memory.hpp"
#include "memory/manager.hpp"

namespace spade::basic
{
    struct LNode {
        LNode *prev = null;
        Collectible *data = null;
        LNode *next = null;
    };

    class BasicMemoryManager : public MemoryManager {
      public:
        LNode *head = null;
        LNode *last = null;

        BasicMemoryManager(SpadeVM *vm = null) : MemoryManager(vm) {}

        void *allocate(size_t size) override;

        void post_allocation(Collectible *obj) override;

        void deallocate(void *pointer) override;

        void collect_garbage() override;
    };
}    // namespace spade::basic
