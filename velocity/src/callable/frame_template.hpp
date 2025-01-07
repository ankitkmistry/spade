#ifndef VELOCITY_FRAME_TEMPLATE_HPP
#define VELOCITY_FRAME_TEMPLATE_HPP

#include "../objects/obj.hpp"
#include "../objects/typeparam.hpp"
#include "../utils/common.hpp"
#include "frame.hpp"
#include "table.hpp"

namespace spade
{
    class ObjMethod;

    class FrameTemplate {
        uint32 codeCount;
        uint8 *code;
        uint32 maxStack;
        ArgsTable args;
        LocalsTable locals;
        ExceptionTable exceptions;
        LineNumberTable lines;
        vector<ObjMethod *> lambdas;
        vector<MatchTable> matches;
        ObjMethod *method;

      public:
        FrameTemplate(uint32 codeCount, uint8 *code, uint32 maxStack, ArgsTable args, LocalsTable locals,
                      ExceptionTable exceptions, LineNumberTable lines, vector<ObjMethod *> lambdas, vector<MatchTable> matches,
                      ObjMethod *method = null)
            : codeCount(codeCount),
              code(code),
              maxStack(maxStack),
              args(args),
              locals(locals),
              exceptions(exceptions),
              lines(lines),
              lambdas(lambdas),
              matches(matches),
              method(method) {}

        Frame initialize();
        FrameTemplate *copy();

        uint32 getCodeCount() const { return codeCount; }

        uint8 *getCode() const { return code; }

        uint32 getMaxStack() const { return maxStack; }

        const ArgsTable &getArgs() const { return args; }

        const LocalsTable &getLocals() const { return locals; }

        const ExceptionTable &getExceptions() const { return exceptions; }

        const LineNumberTable &getLines() const { return lines; }

        const vector<ObjMethod *> &getLambdas() const { return lambdas; }

        const vector<MatchTable> &getMatches() const { return matches; }

        ArgsTable &getArgs() { return args; }

        LocalsTable &getLocals() { return locals; }

        ExceptionTable &getExceptions() { return exceptions; }

        LineNumberTable &getLines() { return lines; }

        vector<ObjMethod *> &getLambdas() { return lambdas; }

        vector<MatchTable> &getMatches() { return matches; }

        ObjMethod *getMethod() const { return method; }

        void setMethod(ObjMethod *method_) { method = method_; }
    };
} // namespace spade

#endif // VELOCITY_FRAME_TEMPLATE_HPP
