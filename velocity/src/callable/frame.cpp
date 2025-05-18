#include "frame.hpp"
#include "../objects/module.hpp"
#include "method.hpp"

namespace spade
{
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
