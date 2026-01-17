#pragma once

#include "table.hpp"
#include <cstdint>

namespace spade
{
    class ObjMethod;

    class SWAN_EXPORT Frame {
        friend class ObjMethod;

      private:
        uint32_t stack_max;
        uint32_t code_count;

      public:
        const uint8_t *code;
        uint32_t pc;
        Value *stack;
        uint32_t sc;

      private:
        uint8_t args_count;
        uint16_t locals_count;
        ObjMethod *method;
        ObjModule *module;

      public:
        Frame();
        Frame(const Frame &frame) = delete;
        Frame(Frame &&frame);
        Frame &operator=(const Frame &frame) = delete;
        Frame &operator=(Frame &&frame);
        ~Frame();

        /**
         * Pushes onto the stack
         * @param val the value to be pushed
         */
        void push(Value val) {
            assert(sc < stack_max && "stack counter is out of bounds");
            stack[sc] = val;
            sc++;
        }

        /**
         * Pops the stack
         * @return the popped value
         */
        Value pop() {
            assert(sc > 0 && "stack counter is out of bounds");
            return stack[--sc];
        }

        /**
         * @return The value of the last item of the stack
         */
        Value peek() const {
            assert(sc > 0 && "stack counter is out of bounds");
            return stack[sc - 1];
        }

        /**
         * @return The constant pool
         */
        const vector<Value> &get_const_pool() const {
            return module->get_constant_pool();
        }

        uint8_t get_args_count() const {
            return args_count;
        }

        Value get_arg(uint8_t i) const;
        void set_arg(uint8_t i, Value value) const;
        ObjCapture *ramp_up_arg(uint8_t i) const;

        uint16_t get_locals_count() const {
            return locals_count;
        }

        Value get_local(uint16_t i) const;
        void set_local(uint16_t i, Value value) const;
        ObjCapture *ramp_up_local(uint16_t i) const;

        /**
         * @return The method associated with the this frame
         */
        ObjMethod *get_method() const {
            return method;
        }

        /**
         * @return The module associated with the this frame
         */
        ObjModule *get_module() const {
            return module;
        }

        /**
         * Sets the method associated with this frame
         * @param met the method value
         */
        void set_method(ObjMethod *met);

        /**
         * @return The number of items on the stack
         */
        uint32_t get_stack_count() const {
            return sc;
        }

        /**
         * @return The maximum capacity of the stack
         */
        uint32_t get_max_stack_count() const {
            return stack_max;
        }

        /**
         * @return The size of the bytecode
         */
        uint32_t get_code_count() const {
            return code_count;
        }
    };
}    // namespace spade
