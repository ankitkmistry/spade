#pragma once

#include "../ee/obj.hpp"

namespace spade
{
    /**
     * Allocates a `Obj` object of type @p T and constructs an object
     * specified with @p args . If the current manager is null, throws ArgumentError.
     * Sets the manager of the object and calls spade::MemoryManager::post_allocation
     * on the object and returns the final object thus created.
     * @throws ArgumentError if manager is null whatsoever
     * @throws MemoryError if allocation fails
     * @tparam T type of the object
     * @tparam Args argument types of the object
     * @param args arguments for object constructor
     * @return the allocated object
     */
    template<typename T, typename... Args>
    inline T *halloc(Args... args)
        requires std::derived_from<T, Obj>
    {
        // auto manager = MemoryManager::current();
        // if (manager == null)
        //     throw ArgumentError("halloc()", "manager is null");
        // void *memory = manager->allocate(sizeof(T));
        // if (memory == null)
        //     throw MemoryError(sizeof(T));
        // Obj *obj = new (memory) T(args...);
        // obj->get_info().manager = manager;
        // manager->post_allocation(obj);
        // return (T *) obj;
        return new T(args...);
    }

    /**
     * Allocates a `Obj` object of type @p T and constructs an object
     * specified with @p args . If manager is null, it sets manager as the current
     * memory manager of the thread. Still if the manager is null, throws ArgumentError.
     * Sets the manager of the object and calls spade::MemoryManager::post_allocation
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
    inline T *halloc_mgr(MemoryManager *manager, Args... args)
        requires std::derived_from<T, Obj>
    {
        // if (manager == null)
        //     manager = MemoryManager::current();
        // if (manager == null)
        //     throw ArgumentError("halloc()", "manager is null");
        // void *memory = manager->allocate(sizeof(T));
        // if (memory == null)
        //     throw MemoryError(sizeof(T));
        // Obj *obj = new (memory) T(std::forward<Args>(args)...);
        // obj->get_info().manager = manager;
        // manager->post_allocation(obj);
        // return (T *) obj;
        (void) manager;
        return new T(args...);
    }

    /**
     * Frees an `Obj` object allocated by spade::halloc
     * @param obj the object to be freed
     */
    inline static void hfree(Obj *obj) {
        // auto manager = obj->get_info().manager;
        // obj->~Obj();
        // manager->deallocate(obj);
        delete obj;
    }
}    // namespace spade
