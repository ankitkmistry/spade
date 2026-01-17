#include "frame.hpp"
#include "method.hpp"
#include "ee/vm.hpp"

namespace spade
{
    Frame::Frame()
        : stack_max(0),
          code_count(0),
          code(null),
          pc(0),
          stack(null),
          sc(0),
          args(),
          locals(),
          method(null),
          module(null) {}

    Frame::Frame(Frame &&frame)
        : stack_max(frame.stack_max),
          code_count(frame.code_count),
          code(frame.code),
          pc(frame.pc),
          stack(frame.stack),
          sc(frame.sc),
          args(std::move(frame.args)),
          locals(std::move(frame.locals)),
          method(frame.method),
          module(frame.module) {
        frame.code_count = frame.stack_max = 0;
        frame.code = null;
        frame.pc = 0;
        frame.stack = null;
        frame.sc = 0;
        frame.method = null;
        frame.module = null;
    }

    Frame &Frame::operator=(Frame &&frame) {
        stack_max = frame.stack_max;
        code_count = frame.code_count;
        code = frame.code;
        pc = frame.pc;
        stack = frame.stack;
        sc = frame.sc;
        args = std::move(frame.args);
        locals = std::move(frame.locals);
        method = frame.method;
        module = frame.module;

        frame.code_count = frame.stack_max = 0;
        frame.code = null;
        frame.pc = 0;
        frame.stack = null;
        frame.sc = 0;
        frame.method = null;
        frame.module = null;

        return *this;
    }

    Frame::~Frame() {
        if (stack)
            delete[] stack;

        code_count = stack_max = 0;
        code = null;
        pc = 0;
        stack = null;
        sc = 0;
        method = null;
        module = null;
    }

    void Frame::set_method(ObjMethod *met) {
        method = met;
        module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
    }
}    // namespace spade
