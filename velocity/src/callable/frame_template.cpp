#include "frame_template.hpp"

namespace spade
{
    Frame FrameTemplate::initialize() {
        Frame frame{};
        frame.code_count = code_count;
        frame.ip = frame.code = code;
        frame.stack = new Obj *[max_stack];
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
        return new FrameTemplate(code_count, code, max_stack, args, locals, exceptions, lines, lambdas, matches, method);
    }
}    // namespace spade
