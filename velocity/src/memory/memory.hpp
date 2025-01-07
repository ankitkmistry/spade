#ifndef VELOCITY_MEMORY_HPP
#define VELOCITY_MEMORY_HPP

#include "../utils/exceptions.hpp"
#include "manager.hpp"

namespace spade
{
    struct MemoryInfo {
        bool marked = false;
        uint32 life = 0;
        MemoryManager *manager = null;
    };

    class Collectible {
      protected:
        MemoryInfo info;

      public:
        explicit Collectible(MemoryInfo info = {}) : info(info) {}

        virtual ~Collectible() = default;

        const MemoryInfo &getInfo() const { return info; }

        MemoryInfo &getInfo() { return info; }
    };

    /**
     * Allocates a Collectible object of type \p T and constructs an object
     * specified with \p args . If manager is null, it sets manager as the current
     * memory manager of the thread. Still if the manager is null, throws ArgumentError.
     * Sets the manager of the object and calls spade::MemoryManager::postAllocation
     * on the object and returns the final object thus created.
     * @throws ArgumentError if manager is null whatsoever
     * @throws MemoryError if allocation fails
     * @tparam T type of the object
     * @tparam Args argument types of the object
     * @param manager the memory manager which will allocate the object
     * @param args arguments for object constructor
     * @return the allocated object
     */
    template<typename T, typename... Args>
    inline T *halloc(MemoryManager *manager, Args... args) {
        if (manager == null) {
            manager = MemoryManager::current();
        }
        if (manager == null) {
            throw ArgumentError("halloc()", "manager is null");
        }
        void *memory = manager->allocate(sizeof(T));
        if (memory == null) {
            throw MemoryError(sizeof(T));
        }
        Collectible *obj = new (memory) T(args...);
        obj->getInfo().manager = manager;
        manager->postAllocation(obj);
        return (T *) obj;
    }

    /**
     * Frees an Collectible object allocated by spade::halloc
     * @param obj the object to be freed
     */
    inline static void hfree(Collectible *obj) {
        auto manager = obj->getInfo().manager;
        obj->~Collectible();
        manager->deallocate(obj);
    }
}    // namespace spade

#endif    // VELOCITY_MEMORY_HPP
