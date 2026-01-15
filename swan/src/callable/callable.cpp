#include "callable.hpp"
#include "utils/errors.hpp"

namespace spade
{
    void ObjCallable::validate_call_site() {
        if (const auto mgr = MemoryManager::current(); !mgr || mgr != info.manager)
            throw IllegalAccessError(std::format("invalid call site, cannot call {}", to_string()));
    }
}    // namespace spade
