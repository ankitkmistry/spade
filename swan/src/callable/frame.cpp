#include "frame.hpp"
#include "method.hpp"
#include "ee/vm.hpp"
#include <cstring>
#include <memory>

namespace spade
{
    Frame::Frame(uint32_t stack_max) : stack_max(stack_max) {
        stack = std::make_unique<Value[]>(stack_max);
        sp = &stack[0];
    }

    Frame::Frame() : stack_max(0), stack(null) {}

    Frame::Frame(const Frame &frame)
        : stack_max(frame.stack_max),
          code_count(frame.code_count),
          code(null),
          ip(null),
          stack(null),
          sp(null),
          args(frame.args),
          locals(frame.locals),
          exceptions(frame.exceptions),
          lines(frame.lines),
          matches(frame.matches),
          method(frame.method),
          module(frame.module) {
        code = std::make_unique<uint8_t[]>(code_count);
        std::memcpy(&code[0], &frame.code[0], code_count);
        ip = &code[0] + (frame.ip - &frame.code[0]);

        stack = std::make_unique<Value[]>(stack_max);
        std::memcpy(&stack[0], &frame.stack[0], stack_max * sizeof(Obj *));
        sp = &stack[0] + (frame.sp - &frame.stack[0]);
    }

    Frame &Frame::operator=(const Frame &frame) {
        stack_max = frame.stack_max;
        code_count = frame.code_count;
        args = frame.args;
        locals = frame.locals;
        exceptions = frame.exceptions;
        lines = frame.lines;
        matches = frame.matches;
        method = frame.method;
        module = frame.module;

        code = std::make_unique<uint8_t[]>(code_count);
        std::memcpy(&code[0], &frame.code[0], code_count);
        ip = &code[0] + (frame.ip - &frame.code[0]);

        stack = std::make_unique<Value[]>(stack_max);
        std::memcpy(&stack[0], &frame.stack[0], stack_max * sizeof(Obj *));
        sp = &stack[0] + (frame.sp - &frame.stack[0]);

        return *this;
    }

    void Frame::set_method(ObjMethod *met) {
        method = met;
        module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
    }

    Frame FrameTemplate::initialize(ObjMethod *method) {
        Frame frame(stack_max);

        frame.code_count = code_count;
        frame.code = std::make_unique<uint8_t[]>(code_count);
        std::memcpy(&frame.code[0], &code[0], code_count);
        frame.ip = &frame.code[0];

        frame.args = args;
        frame.locals = locals;
        frame.exceptions = exceptions;
        frame.lines = lines;
        frame.matches = matches;
        frame.method = method;
        frame.module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
        return frame;
    }
}    // namespace spade
