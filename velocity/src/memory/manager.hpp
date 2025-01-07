#ifndef VELOCITY_MANAGER_HPP
#define VELOCITY_MANAGER_HPP

#include "../utils/common.hpp"

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
        virtual void postAllocation(Collectible *obj) = 0;

        /**
         * Frees the pointer and returns it to the operating system for further use
         * @param pointer pointer to the memory block
         */
        virtual void deallocate(void *pointer) = 0;

        /**
         * Initiates garbage collection. Frees up unnecessary space.
         */
        virtual void collectGarbage() = 0;

        void setVM(SpadeVM *vm_) { vm = vm_; }

        const SpadeVM *getVM() const { return vm; }

        SpadeVM *getVM() { return vm; }

        /**
         * @return the current memory manager respective to the current vm
         */
        static MemoryManager *current();
    };
} // namespace spade

#endif // VELOCITY_MANAGER_HPP
