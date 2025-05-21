#include <cstring>

#include "state.hpp"
#include "callable/frame.hpp"
#include "utils/exceptions.hpp"

namespace spade
{
    VMState::VMState(SpadeVM *vm, size_t stack_depth) : stack_depth(stack_depth), vm(vm) {
        call_stack = new Frame[stack_depth];
        fp = call_stack;
    }

    VMState::VMState(const VMState &other) : stack_depth(other.stack_depth), vm(other.vm) {
        call_stack = new Frame[stack_depth];
        std::memcpy(call_stack, other.call_stack, stack_depth * sizeof(Frame));
        fp = call_stack + (other.fp - other.call_stack);
    }

    VMState::VMState(VMState &&other) : stack_depth(other.stack_depth), vm(other.vm) {
        call_stack = other.call_stack;
        fp = other.fp;

        other.call_stack = null;
    }

    VMState &VMState::operator=(const VMState &other) {
        stack_depth = other.stack_depth;

        delete[] call_stack;
        call_stack = new Frame[stack_depth];
        std::memcpy(call_stack, other.call_stack, stack_depth * sizeof(Frame));
        fp = call_stack + (other.fp - other.call_stack);
        return *this;
    }

    VMState &VMState::operator=(VMState &&other) {
        stack_depth = other.stack_depth;

        delete[] call_stack;
        call_stack = other.call_stack;
        fp = other.fp;

        other.call_stack = null;
        return *this;
    }

    VMState::~VMState() {
        delete[] call_stack;    // This automatically calls the destructor of all elements in the array
    }

    void VMState::push_frame(Frame frame) {
        if (fp - call_stack >= stack_depth)
            throw StackOverflowError();
        *fp++ = std::move(frame);
    }

    bool VMState::pop_frame() {
        if (fp > call_stack) {
            // no need because delete[] and std::move handles this
            // get_frame()->~Frame();    // destroy the frame
            fp--;
            return true;
        }
        return false;
    }
}    // namespace spade
