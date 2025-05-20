#include "state.hpp"
#include "utils/exceptions.hpp"
#include <cstring>

namespace spade
{
    VMState::VMState(SpadeVM *vm) : vm(vm) {
        fp = &call_stack[0];
    }

    VMState::VMState(const VMState &other) : vm(other.vm), call_stack(), fp(other.fp) {
        std::memcpy(call_stack, other.call_stack, FRAMES_MAX * sizeof(Frame));
        fp = call_stack + (other.fp - other.call_stack);
    }

    VMState::VMState(VMState &&other) : vm(other.vm), call_stack(), fp(other.fp) {
        std::memcpy(call_stack, other.call_stack, FRAMES_MAX * sizeof(Frame));
        fp = call_stack + (other.fp - other.call_stack);
    }

    void VMState::push_frame(Frame frame) {
        if (fp - call_stack >= FRAMES_MAX)
            throw StackOverflowError();
        *fp++ = std::move(frame);
    }

    Frame *VMState::pop_frame() {
        if (fp > call_stack)
            return fp--;
        return fp = null;
    }
}    // namespace spade
