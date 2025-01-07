#ifndef VELOCITY_FRAME_HPP
#define VELOCITY_FRAME_HPP

#include "../objects/obj.hpp"
#include "../utils/common.hpp"
#include "table.hpp"

namespace spade
{
    class ObjMethod;

    class Frame {
        friend class VMState;

        friend class FrameTemplate;

      private:
        uint32 codeCount = 0;

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
        vector<ObjMethod *> lambdas;
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
        const vector<Obj *> &getConstPool() const;

        /**
         * @return The arguments table
         */
        ArgsTable &getArgs() { return args; }

        /**
         * @return The locals table
         */
        LocalsTable &getLocals() { return locals; }

        /**
         * @return The exceptions table
         */
        ExceptionTable &getExceptions() { return exceptions; }

        /**
         * @return The arguments table
         */
        const ArgsTable &getArgs() const { return args; }

        /**
         * @return The locals table
         */
        const LocalsTable &getLocals() const { return locals; }

        /**
         * @return The exceptions table
         */
        const ExceptionTable &getExceptions() const { return exceptions; }

        /**
         * @return The line number table
         */
        const LineNumberTable &getLines() const { return lines; }

        /**
         * @return The array of lambda functions
         */
        const vector<ObjMethod *> &getLambdas() const { return lambdas; }

        /**
         * @return The array of check statements
         */
        const vector<MatchTable> &getMatches() const { return matches; }

        /**
         * @return The method associated with the this frameTemplate
         */
        ObjMethod *getMethod() const { return method; }

        /**
         * Sets the ip of this frameTemplate to ip
         * @param newIp the new ip value
         */
        void setIp(uint8 *newIp) { ip = newIp; }

        /**
         * Sets the method associated with this frameTemplate
         * @param met the method value
         */
        void setMethod(ObjMethod *met) { method = met; }

        /**
         * @return The number of items on the stack
         */
        uint32 getStackCount();

        /**
         * @return The size of the bytecode
         */
        uint32 getCodeCount() const;
    };
}    // namespace spade

#endif    // VELOCITY_FRAME_HPP
