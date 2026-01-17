#include "method.hpp"
#include "callable.hpp"
#include "callable/table.hpp"
#include "ee/thread.hpp"
#include "ee/vm.hpp"
#include "frame.hpp"
#include "ee/obj.hpp"
#include "utils/errors.hpp"
#include <algorithm>
#include <cstdint>
#include <memory>

namespace spade
{
    ObjMethod::ObjMethod(Kind kind, const Sign &sign, const vector<uint8_t> &code, uint32_t stack_max, const VariableTable &args,
                         const VariableTable &locals, const ExceptionTable &exceptions, const LineNumberTable &lines,
                         const vector<MatchTable> &matches)
        : ObjCallable(OBJ_METHOD, kind, sign),
          code_count(code.size()),
          code(std::make_unique<uint8_t[]>(code_count)),
          stack_max(stack_max),
          arg_tbl(args),
          loc_tbl(locals),
          exceptions(exceptions),
          lines(lines),
          matches(matches) {
        std::copy(code.begin(), code.end(), &this->code[0]);
    }

    void ObjMethod::call(Obj *self, vector<Value> args) {
        validate_call_site();
        // Check arg count
        const auto arg_count = arg_tbl.count();
        if (arg_count < args.size())
            throw ArgumentError(sign.to_string(), std::format("too less arguments, expected {} got {}", arg_count, args.size()));
        if (arg_count > args.size())
            throw ArgumentError(sign.to_string(), std::format("too many arguments, expected {} got {}", arg_count, args.size()));
        // Call the function
        call_impl(self, args.data());
    }

    void ObjMethod::call(Obj *self, Value *args) {
        validate_call_site();
        call_impl(self, args);
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
        frame.stack = new Value[stack_max]();
        frame.sc = 0;

        frame.args = arg_tbl;
        frame.locals = loc_tbl;
        frame.method = this;
        frame.module = cast<ObjModule>(info.manager->get_vm()->get_symbol(sign.get_parent_module().to_string()).as_obj());

        // Set the arguments
        for (size_t i = 0; i < arg_tbl.count(); i++) frame.args.set(i, args[i]);
        // Set the self reference
        if (self)
            frame.locals.set(0, self);
        // Push the frame
        Thread::current()->get_state().push_frame(std::move(frame));
    }
}    // namespace spade
