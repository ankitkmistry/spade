#pragma once

#include "table.hpp"

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
        size_t args_count;
        size_t locals_count;

        VariableTable args;
        VariableTable locals;
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

        /**
         * @return The arguments table
         */
        VariableTable &get_args() {
            return args;
        }

        /**
         * @return The locals table
         */
        VariableTable &get_locals() {
            return locals;
        }

        /**
         * @return The arguments table
         */
        const VariableTable &get_args() const {
            return args;
        }

        /**
         * @return The locals table
         */
        const VariableTable &get_locals() const {
            return locals;
        }

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
