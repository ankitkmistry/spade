#pragma once

namespace spade
{
    class SpadeVM;

    class Obj;

    class Collectible;

    class MemoryManager {
      protected:
        SpadeVM *vm;

      public:
        MemoryManager(SpadeVM *vm) : vm(vm) {}

        /**
         * Allocates a block of memory
         * @param size size in bytes
         * @return the pointer to the memory block
         */
        virtual void *allocate(size_t size) = 0;

        /**
         * This function performs post allocation tasks on the object.
         * This function is automatically just after allocation and initialization
         * @param obj
         */
        virtual void post_allocation(Collectible *obj) = 0;

        /**
         * Frees the pointer and returns it to the operating system for further use
         * @param pointer pointer to the memory block
         */
        virtual void deallocate(void *pointer) = 0;

        /**
         * Initiates garbage collection. Frees up unnecessary space.
         */
        virtual void collect_garbage() = 0;

        void set_vm(SpadeVM *vm_) {
            vm = vm_;
        }

        const SpadeVM *get_vm() const {
            return vm;
        }

        SpadeVM *get_vm() {
            return vm;
        }

        /**
         * @return the current memory manager respective to the current vm
         */
        static MemoryManager *current();
    };
}    // namespace spade
