#include "state.hpp"
#include "../callable/method.hpp"
#include "../objects/module.hpp"

namespace spade {
    VMState::VMState(SpadeVM *vm) : vm(vm) {
        callStack = new Frame[FRAMES_MAX];
        fp = callStack;
    }

    void VMState::pushFrame(Frame frame) {
        if (fp - callStack >= FRAMES_MAX) {
            throw StackOverflowError();
        }
        *fp++ = frame;
    }

    Frame *VMState::popFrame() {
        if (fp > callStack) {
            return fp--;
        }
        return fp = null;
    }
}
