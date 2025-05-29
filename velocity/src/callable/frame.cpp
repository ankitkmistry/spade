#include <cstring>

#include "frame.hpp"
#include "callable/table.hpp"
#include "ee/vm.hpp"
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
          args(frame.args),
          locals(frame.locals),
          exceptions(frame.exceptions),
          lines(frame.lines),
          matches(frame.matches),
          method(frame.method),
          module(frame.module) {
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
          method(frame.method),
          module(frame.module) {
        module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()));
        frame.stack = null;
    }

    Frame &Frame::operator=(const Frame &frame) {
        stack_max = frame.stack_max;
        code_count = frame.code_count;
        code = frame.code;
        ip = frame.ip;
        args = frame.args;
        locals = frame.locals;
        exceptions = frame.exceptions;
        lines = frame.lines;
        matches = frame.matches;
        method = frame.method;
        module = frame.module;

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
        module = frame.module;

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

    void Frame::set_method(ObjMethod *met) {
        method = met;
        module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()));
    }

    const vector<Obj *> &Frame::get_const_pool() const {
        return module->get_constant_pool();
    }
}    // namespace spade
