#pragma once

#include <cstddef>

#include "utils/common.hpp"
#include "memory/manager.hpp"

namespace spade::basic
{
    struct LNode {
        size_t size = 0;
        Obj *data = null;
        LNode *next = null;
    };

    class BasicMemoryManager : public MemoryManager {
        size_t cur_alloc_size = 0;
        size_t allocation_size = 0;
        size_t free_size = 0;

      public:
        LNode *head = null;
        LNode *last = null;

        BasicMemoryManager(SpadeVM *vm = null) : MemoryManager(vm) {}

        ~BasicMemoryManager() override;

        void *allocate(size_t size) override;
        void post_allocation(Obj *obj) override;
        void deallocate(void *pointer) override;
        void collect_garbage() override;

        size_t get_allocation_size() const {
            return allocation_size;
        }

        size_t get_free_size() const {
            return free_size;
        }

        size_t get_used_size() const {
            return allocation_size - free_size;
        }
    };
}    // namespace spade::basic
