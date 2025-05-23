#include <cstring>

#include "frame.hpp"
#include "callable/table.hpp"
#include "objects/module.hpp"
#include "method.hpp"

namespace spade
{
    Frame::Frame(uint32 stack_max) : stack_max(stack_max) {
        stack = new Obj *[stack_max];
        sp = stack;
    }

    Frame::Frame() : stack_max(0), stack(null) {}

    Frame::Frame(const Frame &frame)
        : stack_max(frame.stack_max),
          code_count(frame.code_count),
          code(frame.code),
          ip(frame.ip),
          args(frame.args.copy()),
          locals(frame.locals.copy()),
          exceptions(frame.exceptions),
          lines(frame.lines),
          matches(frame.matches),
          method(frame.method) {
        stack = new Obj *[stack_max];
        std::memcpy(stack, frame.stack, stack_max * sizeof(Obj *));
        sp = stack + (frame.sp - frame.stack);
    }

    Frame::Frame(Frame &&frame)
        : stack_max(frame.stack_max),
          code_count(frame.code_count),
          code(frame.code),
          ip(frame.ip),
          stack(frame.stack),
          sp(frame.sp),
          args(std::move(frame.args)),
          locals(std::move(frame.locals)),
          exceptions(frame.exceptions),
          lines(frame.lines),
          matches(frame.matches),
          method(frame.method) {
        frame.stack = null;
    }

    Frame &Frame::operator=(const Frame &frame) {
        stack_max = frame.stack_max;
        code_count = frame.code_count;
        code = frame.code;
        ip = frame.ip;
        args = frame.args.copy();
        locals = frame.locals.copy();
        exceptions = frame.exceptions;
        lines = frame.lines;
        matches = frame.matches;
        method = frame.method;

        delete[] stack;
        stack = new Obj *[stack_max];
        std::memcpy(stack, frame.stack, stack_max * sizeof(Obj *));
        sp = stack + (frame.sp - frame.stack);
        return *this;
    }

    Frame &Frame::operator=(Frame &&frame) {
        stack_max = frame.stack_max;
        code_count = frame.code_count;
        code = frame.code;
        ip = frame.ip;
        args = std::move(frame.args);
        locals = std::move(frame.locals);
        exceptions = frame.exceptions;
        lines = frame.lines;
        matches = frame.matches;
        method = frame.method;

        delete[] stack;
        stack = frame.stack;
        sp = frame.sp;
        frame.stack = null;
        return *this;
    }

    Frame::~Frame() {
        delete[] stack;
    }

    void Frame::push(Obj *val) {
        *sp++ = val;
    }

    Obj *Frame::pop() {
        return *--sp;
    }

    Obj *Frame::peek() {
        return sp[-1];
    }

    uint32 Frame::get_stack_count() const {
        return sp - stack;
    }

    uint32 Frame::get_code_count() const {
        return code_count;
    }

    const vector<Obj *> &Frame::get_const_pool() const {
        return method->get_module()->get_constant_pool();
    }
}    // namespace spade
