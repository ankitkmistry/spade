#include "thread.hpp"
#include "vm.hpp"

namespace spade
{
    Thread::Thread(SpadeVM *vm, const std::function<void(Thread *)> &fun) : state(vm), thread(fun, this) {
        threads[thread.get_id()] = this;
    }

    Thread *Thread::current() {
        if (const auto it = threads.find(std::this_thread::get_id()); it != threads.end())
            return it->second;
        return null;
    }
}    // namespace spade
