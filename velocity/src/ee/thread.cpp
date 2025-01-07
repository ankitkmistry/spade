#include "thread.hpp"

namespace spade {
    Thread::Thread(VMState *state, function<void(Thread *)> fun)
            : state(state), thread(fun, this) {
        threads[thread.get_id()] = this;
    }

    Thread *Thread::current() {
        try {
            return threads.at(std::this_thread::get_id());
        } catch (const std::out_of_range &) {
            return null;
        }
    }
}
