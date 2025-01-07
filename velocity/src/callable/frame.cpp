#include "frame.hpp"
#include "../objects/module.hpp"
#include "method.hpp"

namespace spade
{
    void Frame::push(Obj *val) { *sp++ = val; }

    Obj *Frame::pop() { return *--sp; }

    Obj *Frame::peek() { return sp[-1]; }

    uint32 Frame::getStackCount() { return sp - stack; }

    uint32 Frame::getCodeCount() const { return codeCount; }

    const vector<Obj *> &Frame::getConstPool() const { return method->getModule()->getConstantPool(); }
}    // namespace spade
