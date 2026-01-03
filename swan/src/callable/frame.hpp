#pragma once

#include "table.hpp"
#include <memory>

namespace spade
{
    class ObjMethod;

    class Frame {
        friend class VMState;

        friend class FrameTemplate;

      private:
        uint32_t stack_max = 0;
        uint32_t code_count = 0;

      public:
        std::unique_ptr<uint8_t[]> code = null;
        uint8_t *ip = null;
        std::unique_ptr<Obj *[]> stack = null;
        Obj **sp = null;

      private:
        VariableTable args;
        VariableTable locals;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<MatchTable> matches;
        ObjMethod *method = null;
        ObjModule *module = null;

        Frame(uint32_t stack_max);

      public:
        Frame();
        Frame(const Frame &frame);
        Frame(Frame &&frame) = default;
        Frame &operator=(const Frame &frame);
        Frame &operator=(Frame &&frame) = default;
        ~Frame() = default;

        /**
         * Pushes onto the stack
         * @param val the value to be pushed
         */
        void push(Obj *val);

        /**
         * Pops the stack
         * @return the popped value
         */
        Obj *pop();

        /**
         * @return The value of the last item of the stack
         */
        Obj *peek();

        /**
         * @return The constant pool
         */
        const vector<Obj *> &get_const_pool() const;

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
         * @return The exceptions table
         */
        ExceptionTable &get_exceptions() {
            return exceptions;
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
         * @return The exceptions table
         */
        const ExceptionTable &get_exceptions() const {
            return exceptions;
        }

        /**
         * @return The line number table
         */
        const LineNumberTable &get_lines() const {
            return lines;
        }

        /**
         * @return The array of match statements
         */
        const vector<MatchTable> &get_matches() const {
            return matches;
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
        uint32_t get_stack_count() const;

        /**
         * @return The maximum capacity of the stack
         */
        uint32_t get_max_stack_count() const;

        /**
         * @return The size of the bytecode
         */
        uint32_t get_code_count() const;
    };

    class FrameTemplate {
        uint32_t code_count;
        std::unique_ptr<uint8_t[]> code;
        uint32_t stack_max;
        VariableTable args;
        VariableTable locals;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<MatchTable> matches;
        ObjMethod *method;

      public:
        FrameTemplate(const vector<uint8_t> &code, uint32_t stack_max, const VariableTable &args, const VariableTable &locals,
                      const ExceptionTable &exceptions, const LineNumberTable &lines, const vector<MatchTable> &matches, ObjMethod *method = null)
            : code_count(static_cast<uint32_t>(code.size())),
              code(null),
              stack_max(stack_max),
              args(args),
              locals(locals),
              exceptions(exceptions),
              lines(lines),
              matches(matches),
              method(method) {
            this->code = std::make_unique<uint8_t[]>(code_count);
            std::memcpy(&this->code[0], code.data(), code_count);
        }

        // Copy constructor
        FrameTemplate(const FrameTemplate &other)
            : code_count(other.code_count),
              code(null),
              stack_max(other.stack_max),
              args(other.args),
              locals(other.locals),
              exceptions(other.exceptions),
              lines(other.lines),
              matches(other.matches),
              method(other.method) {
            code = std::make_unique<uint8_t[]>(code_count);
            std::memcpy(&code[0], &other.code[0], code_count);
        }

        // Move constructor
        FrameTemplate(FrameTemplate &&other) noexcept = default;

        // Copy assignment
        FrameTemplate &operator=(const FrameTemplate &other) {
            if (this != &other) {
                code_count = other.code_count;
                code = std::make_unique<uint8_t[]>(code_count);
                std::memcpy(&code[0], &other.code[0], code_count);

                stack_max = other.stack_max;
                args = other.args;
                locals = other.locals;
                exceptions = other.exceptions;
                lines = other.lines;
                matches = other.matches;
                method = other.method;
            }
            return *this;
        }

        // Move assignment
        FrameTemplate &operator=(FrameTemplate &&other) noexcept = default;

        // Destructor
        ~FrameTemplate() = default;

        Frame initialize();
        FrameTemplate *copy();

        uint32_t get_code_count() const {
            return code_count;
        }

        uint8_t *get_code() const {
            return &code[0];
        }

        uint32_t get_stack_max() const {
            return stack_max;
        }

        const VariableTable &get_args() const {
            return args;
        }

        const VariableTable &get_locals() const {
            return locals;
        }

        const ExceptionTable &get_exceptions() const {
            return exceptions;
        }

        const LineNumberTable &get_lines() const {
            return lines;
        }

        const vector<MatchTable> &get_matches() const {
            return matches;
        }

        VariableTable &get_args() {
            return args;
        }

        VariableTable &get_locals() {
            return locals;
        }

        ExceptionTable &get_exceptions() {
            return exceptions;
        }

        LineNumberTable &get_lines() {
            return lines;
        }

        vector<MatchTable> &get_matches() {
            return matches;
        }

        ObjMethod *get_method() const {
            return method;
        }

        void set_method(ObjMethod *method_) {
            method = method_;
        }
    };
}    // namespace spade
