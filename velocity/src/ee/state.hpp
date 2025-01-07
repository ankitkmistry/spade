#ifndef VELOCITY_STATE_HPP
#define VELOCITY_STATE_HPP

#include "../utils/common.hpp"
#include "../utils/utils.hpp"
#include "../callable/frame.hpp"

namespace spade {
    class SpadeVM;

    class VMState {
    private:
        SpadeVM *vm;
        Frame *callStack = null, *fp = null;
    public:
        VMState(SpadeVM *vm);

        VMState(const VMState &state)
                : vm(state.vm),
                  callStack(state.callStack), fp(state.fp) {}

        // Frame operations
        /**
         * Pushes a call frame on top of the call stack
         * @param frame the frame to be pushed
         */
        void pushFrame(Frame frame);

        /**
         * Pops the active call frame and reloads the state
         * @return the popped frame
         */
        Frame *popFrame();

        // Stack operations
        /**
         * Pushes val on top of the operand stack
         * @param val value to be pushed
         */
        void push(Obj *val) const { getFrame()->push(val); }

        /**
         * Pops the operand stack
         * @return the popped value
         */
        Obj *pop() const { return getFrame()->pop(); }

        /**
         * @return the value on top of the operand stack
         */
        Obj *peek() const { return getFrame()->peek(); }

        // Constant pool operations
        /**
         * Loads the constant from the constant pool at index
         * @param index
         * @return the loaded value
         */
        Obj *loadConst(uint16 index) const { return Obj::createCopy(getFrame()->getConstPool()[index]); }

        // Code operations
        /**
         * Advances ip by 1 byte and returns the byte read
         * @return the byte
         */
        uint8 readByte() { return *getFrame()->ip++; }

        /**
         * Advances ip by 2 bytes and returns the bytes read
         * @return the short
         */
        uint16 readShort() {
            auto frame = getFrame();
            frame->ip += 2;
            return (frame->ip[-2] << 8) | frame->ip[-1];
        }

        /**
         * Adjusts the ip by offset
         * @param offset offset to be adjusted
         */
        void adjust(ptrdiff_t offset) { getFrame()->ip += offset; }

        // Getters
        /**
         * @return The vm pointer associated with this state
         */
        SpadeVM *getVM() const { return vm; }

        /**
         * @return The call stack
         */
        Frame *getCallStack() const { return callStack; }

        /**
         * @return The active frame
         */
        Frame *getFrame() const { return fp - 1; }

        /**
         * @return The size of the call stack
         */
        uint16 getCallStackSize() { return fp - callStack; }

        /**
         * @return The program counter
         */
        uint32 getPc() { return getFrame()->ip - getFrame()->code; }

        /**
         * Sets the program counter
         * @param pc the program counter value
         */
        void setPc(uint32 pc) { getFrame()->ip = getFrame()->code + pc; }
    };
}

#endif //VELOCITY_STATE_HPP
