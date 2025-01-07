#include "frame_template.hpp"

namespace spade
{
    Frame FrameTemplate::initialize() {
        Frame frame{};
        frame.codeCount = codeCount;
        frame.ip = frame.code = code;
        frame.stack = new Obj *[maxStack];
        frame.sp = frame.stack;
        frame.args = args.copy();
        frame.locals = locals.copy();
        frame.exceptions = exceptions;
        frame.lines = lines;
        frame.lambdas = lambdas;
        frame.matches = matches;
        frame.method = method;
        return frame;
    }

    FrameTemplate *FrameTemplate::copy() {
        return new FrameTemplate(codeCount, code, maxStack, args, locals, exceptions, lines, lambdas,
                                 matches, method);
    }
}    // namespace spade
