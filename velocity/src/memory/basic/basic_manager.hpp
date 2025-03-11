#pragma once

#include "../manager.hpp"
#include "../memory.hpp"

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

        void postAllocation(Collectible *obj) override;

        void deallocate(void *pointer) override;

        void collectGarbage() override;
    };
}    // namespace spade::basic
