#pragma once

#include "utils/common.hpp"

namespace spade
{
    class SpadeVM;
    class Obj;

    class MemoryManager {
      protected:
        SpadeVM *vm;

        MemoryManager(SpadeVM *vm) : vm(vm) {}

      public:
        SWAN_EXPORT virtual ~MemoryManager() = default;

        /**
         * Allocates a block of memory
         * @param size size in bytes
         * @return the pointer to the memory block
         */
        SWAN_EXPORT virtual void *allocate(size_t size) = 0;

        /**
         * This function performs post allocation tasks on the object.
         * This function is automatically just after allocation and initialization
         * @param obj
         */
        SWAN_EXPORT virtual void post_allocation(Obj *obj) = 0;

        /**
         * Frees the pointer and returns it to the operating system for further use
         * @param pointer pointer to the memory block
         */
        SWAN_EXPORT virtual void deallocate(void *pointer) = 0;

        /**
         * Initiates garbage collection. Frees up unnecessary space.
         */
        SWAN_EXPORT virtual void collect_garbage() = 0;

        SWAN_EXPORT void set_vm(SpadeVM *vm_) {
            vm = vm_;
        }

        SWAN_EXPORT const SpadeVM *get_vm() const {
            return vm;
        }

        SWAN_EXPORT SpadeVM *get_vm() {
            return vm;
        }

        /**
         * @return the current memory manager respective to the current vm
         */
        SWAN_EXPORT static MemoryManager *current();
    };
}    // namespace spade
