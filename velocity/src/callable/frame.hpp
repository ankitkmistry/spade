#pragma once

#include "utils/common.hpp"
#include "objects/obj.hpp"
#include "table.hpp"

namespace spade
{
    class ObjMethod;

    class Frame {
        friend class VMState;

        friend class FrameTemplate;

      private:
        uint32 code_count = 0;

      public:
        uint8 *code = null;
        uint8 *ip = null;
        Obj **stack = null;
        Obj **sp = null;

      private:
        ArgsTable args;
        LocalsTable locals{0};
        ExceptionTable exceptions;
        LineNumberTable lines{};
        vector<MatchTable> matches;
        ObjMethod *method = null;

        Frame() = default;

      public:
        Frame(const Frame &frame) = default;
        Frame &operator=(const Frame &frame) = default;

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
        ArgsTable &get_args() {
            return args;
        }

        /**
         * @return The locals table
         */
        LocalsTable &get_locals() {
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
        const ArgsTable &get_args() const {
            return args;
        }

        /**
         * @return The locals table
         */
        const LocalsTable &get_locals() const {
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
         * @return The method associated with the this frameTemplate
         */
        ObjMethod *get_method() const {
            return method;
        }

        /**
         * Sets the ip of this frameTemplate to ip
         * @param newIp the new ip value
         */
        void set_ip(uint8 *new_ip) {
            ip = new_ip;
        }

        /**
         * Sets the method associated with this frameTemplate
         * @param met the method value
         */
        void set_method(ObjMethod *met) {
            method = met;
        }

        /**
         * @return The number of items on the stack
         */
        uint32 get_stack_count() const;

        /**
         * @return The size of the bytecode
         */
        uint32 get_code_count() const;
    };
}    // namespace spade
