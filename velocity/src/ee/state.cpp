#include "state.hpp"
#include "utils/exceptions.hpp"

namespace spade {
    VMState::VMState(SpadeVM *vm) : vm(vm) {
        call_stack = new Frame[FRAMES_MAX];
        fp = call_stack;
    }

    void VMState::push_frame(Frame frame) {
        if (fp - call_stack >= FRAMES_MAX) {
            throw StackOverflowError();
        }
        *fp++ = frame;
    }

    Frame *VMState::pop_frame() {
        if (fp > call_stack) {
            return fp--;
        }
        return fp = null;
    }
}
