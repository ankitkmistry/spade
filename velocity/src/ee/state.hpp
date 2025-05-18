#pragma once

#include "../utils/common.hpp"
#include "../callable/frame.hpp"

namespace spade
{
    class SpadeVM;

    class VMState {
      private:
        SpadeVM *vm;
        Frame *call_stack = null, *fp = null;

      public:
        VMState(SpadeVM *vm);

        VMState(const VMState &state) : vm(state.vm), call_stack(state.call_stack), fp(state.fp) {}

        // Frame operations
        /**
         * Pushes a call frame on top of the call stack
         * @param frame the frame to be pushed
         */
        void push_frame(Frame frame);

        /**
         * Pops the active call frame and reloads the state
         * @return the popped frame
         */
        Frame *pop_frame();

        // Stack operations
        /**
         * Pushes val on top of the operand stack
         * @param val value to be pushed
         */
        void push(Obj *val) const {
            get_frame()->push(val);
        }

        /**
         * Pops the operand stack
         * @return the popped value
         */
        Obj *pop() const {
            return get_frame()->pop();
        }

        /**
         * @return the value on top of the operand stack
         */
        Obj *peek() const {
            return get_frame()->peek();
        }

        // Constant pool operations
        /**
         * Loads the constant from the constant pool at index
         * @param index
         * @return the loaded value
         */
        Obj *load_const(uint16 index) const {
            return Obj::create_copy(get_frame()->get_const_pool()[index]);
        }

        // Code operations
        /**
         * Advances ip by 1 byte and returns the byte read
         * @return the byte
         */
        uint8 read_byte() {
            return *get_frame()->ip++;
        }

        /**
         * Advances ip by 2 bytes and returns the bytes read
         * @return the short
         */
        uint16 read_short() {
            auto frame = get_frame();
            frame->ip += 2;
            return (frame->ip[-2] << 8) | frame->ip[-1];
        }

        /**
         * Adjusts the ip by offset
         * @param offset offset to be adjusted
         */
        void adjust(ptrdiff_t offset) {
            get_frame()->ip += offset;
        }

        // Getters
        /**
         * @return The vm pointer associated with this state
         */
        SpadeVM *get_vm() const {
            return vm;
        }

        /**
         * @return The call stack
         */
        Frame *get_call_stack() const {
            return call_stack;
        }

        /**
         * @return The active frame
         */
        Frame *get_frame() const {
            return fp - 1;
        }

        /**
         * @return The size of the call stack
         */
        uint16 get_call_stack_size() const {
            return fp - call_stack;
        }

        /**
         * @return The program counter
         */
        uint32 get_pc() const {
            return get_frame()->ip - get_frame()->code;
        }

        /**
         * Sets the program counter
         * @param pc the program counter value
         */
        void set_pc(uint32 pc) {
            get_frame()->ip = get_frame()->code + pc;
        }
    };
}    // namespace spade
