#include "frame_template.hpp"

namespace spade
{
    Frame FrameTemplate::initialize() {
        Frame frame{};
        frame.code_count = code_count;
        frame.ip = frame.code = code;
        frame.stack = new Obj *[stack_max];
        frame.sp = frame.stack;
        frame.args = args.copy();
        frame.locals = locals.copy();
        frame.exceptions = exceptions;
        frame.lines = lines;
        frame.matches = matches;
        frame.method = method;
        return frame;
    }
}    // namespace spade
