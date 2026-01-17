#include "frame.hpp"
#include "ee/obj.hpp"
#include "memory/memory.hpp"
#include "method.hpp"
#include "ee/vm.hpp"
#include "spimp/utils.hpp"

namespace spade
{
    Frame::Frame() : stack_max(0), code_count(0), code(null), pc(0), stack(null), sc(0), args_count(0), locals_count(0), method(null), module(null) {}

    Frame::Frame(Frame &&frame)
        : stack_max(frame.stack_max),
          code_count(frame.code_count),
          code(frame.code),
          pc(frame.pc),
          stack(frame.stack),
          sc(frame.sc),
          args_count(frame.args_count),
          locals_count(frame.locals_count),
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
        args_count = frame.args_count;
        locals_count = frame.locals_count;
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

    Value Frame::get_arg(uint8_t i) const {
        // Check bounds
        if (i >= args_count)
            throw IndexError("arg", i);

        auto value = stack[i];
        if (value.is_obj() && value.as_obj()->get_tag() == OBJ_CAPTURE) {
            // Return the captured value if the arg is captured
            return cast<ObjCapture>(value.as_obj())->get();
        }
        // Otherwise return the normal value
        return value;
    }

    void Frame::set_arg(uint8_t i, Value value) const {
        // Check bounds
        if (i >= args_count)
            throw IndexError("arg", i);

        auto &arg = stack[i];
        // Don't set the value if it is a pointer, instead change the value it is pointing at
        if (arg.is_obj() && arg.as_obj()->get_tag() == OBJ_CAPTURE)
            cast<ObjCapture>(arg.as_obj())->set(value);
        else
            arg = value;
    }

    ObjCapture *Frame::ramp_up_arg(uint8_t i) const {
        // Check bounds
        if (i >= args_count)
            throw IndexError("arg", i);

        auto &value = stack[i];
        if (value.is_obj() && value.as_obj()->get_tag() == OBJ_CAPTURE) {
            // Return the captured value if the arg is captured
            return cast<ObjCapture>(value.as_obj());
        }

        const auto pointer = halloc<ObjCapture>(value);
        value.set(pointer);
        return pointer;
    }

    Value Frame::get_local(uint16_t i) const {
        // Check bounds
        if (i >= locals_count)
            throw IndexError("local", i);

        auto value = stack[args_count + i];
        if (value.is_obj() && value.as_obj()->get_tag() == OBJ_CAPTURE) {
            // Return the captured value if the local is captured
            return cast<ObjCapture>(value.as_obj())->get();
        }
        // Otherwise return the normal value
        return value;
    }

    void Frame::set_local(uint16_t i, Value value) const {
        // Check bounds
        if (i >= locals_count)
            throw IndexError("local", i);

        auto &local = stack[args_count + i];
        // Don't set the value if it is a pointer, instead change the value it is pointing at
        if (local.is_obj() && local.as_obj()->get_tag() == OBJ_CAPTURE)
            cast<ObjCapture>(local.as_obj())->set(value);
        else
            local = value;
    }

    ObjCapture *Frame::ramp_up_local(uint16_t i) const {
        // Check bounds
        if (i >= locals_count)
            throw IndexError("local", i);

        auto &value = stack[args_count + i];
        if (value.is_obj() && value.as_obj()->get_tag() == OBJ_CAPTURE) {
            // Return the captured value if the local is captured
            return cast<ObjCapture>(value.as_obj());
        }

        const auto pointer = halloc<ObjCapture>(value);
        value.set(pointer);
        return pointer;
    }

    void Frame::set_method(ObjMethod *met) {
        method = met;
        module = cast<ObjModule>(SpadeVM::current()->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
    }
}    // namespace spade
