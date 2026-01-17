#include "method.hpp"
#include "callable.hpp"
#include "callable/table.hpp"
#include "ee/thread.hpp"
#include "ee/vm.hpp"
#include "frame.hpp"
#include "ee/obj.hpp"
#include "memory/memory.hpp"
#include "spimp/utils.hpp"
#include "utils/errors.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>

namespace spade
{
    ObjMethod::ObjMethod(Kind kind, const Sign &sign, const vector<uint8_t> &code, uint32_t stack_max, uint8_t args_count, uint16_t locals_count,
                         const ExceptionTable &exceptions, const LineNumberTable &lines, const vector<MatchTable> &matches)
        : ObjCallable(OBJ_METHOD, kind, sign),
          code_count(code.size()),
          code(std::make_unique<uint8_t[]>(code_count)),
          stack_max(stack_max),
          args_count(args_count),
          locals_count(locals_count),
          exceptions(exceptions),
          lines(lines),
          matches(matches) {
        std::copy(code.begin(), code.end(), &this->code[0]);
    }

    void ObjMethod::call(Obj *self, vector<Value> args) {
        validate_call_site();
        if (args_count < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", args_count, args.size()));
        if (args_count > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", args_count, args.size()));
        // Call the function
        call_impl(self, args.data());
    }

    void ObjMethod::call(Obj *self, Value *args) {
        validate_call_site();
        call_impl(self, args);
    }

    void ObjMethod::set_capture(uint16_t local_idx, ObjCapture *capture) {
        if (local_idx >= locals_count)
            throw IndexError("local", local_idx);
        captures.emplace_back(local_idx, capture);
    }

    ObjMethod *ObjMethod::force_copy() const {
        const auto method = halloc_mgr<ObjMethod>(info.manager, kind, sign, vector(&code[0], &code[code_count]), stack_max, args_count, locals_count,
                                                  exceptions, lines, matches);
        for (const auto &[name, slot]: member_slots) {
            method->set_member(name, slot.get_value().copy());
            method->set_flags(name, slot.get_flags());
        }
        for (const auto &info: captures) {
            method->set_capture(info.local_index, info.capture);
        }
        return method;
    }

    string ObjMethod::to_string() const {
        const static string kind_names[] = {"function", "method", "constructor"};
        return std::format("<{} '{}'>", kind_names[static_cast<int>(kind)], sign.to_string());
    }

    void ObjMethod::call_impl(Obj *self, Value *args) {
        Frame frame;

        frame.code_count = code_count;
        frame.stack_max = stack_max;

        frame.code = &code[0];
        frame.pc = 0;
        frame.stack = new Value[args_count + locals_count + stack_max]();
        frame.sc = args_count + locals_count;

        frame.args_count = args_count;
        frame.locals_count = locals_count;
        frame.method = this;
        frame.module = cast<ObjModule>(info.manager->get_vm()->get_symbol(sign.get_parent_module().to_string()).as_obj());

        // Set the arguments
        for (size_t i = 0; i < args_count; i++) frame.set_arg(i, args[i]);
        // Set the captures
        for (const auto &info: captures) frame.set_local(info.local_index, info.capture);
        // Set the self reference
        if (self)
            frame.set_local(0, self);
        // Push the frame
        Thread::current()->get_state().push_frame(std::move(frame));
    }
}    // namespace spade
