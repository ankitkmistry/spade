#include "frame_template.hpp"
#include "ee/vm.hpp"

namespace spade
{
    Frame FrameTemplate::initialize() {
        Frame frame(stack_max);
        frame.code_count = code_count;
        frame.ip = frame.code = code;
        frame.args = args.copy();
        frame.locals = locals.copy();
        frame.exceptions = exceptions;
        frame.lines = lines;
        frame.matches = matches;
        frame.method = method;
        frame.module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()));
        return frame;
    }
}    // namespace spade
