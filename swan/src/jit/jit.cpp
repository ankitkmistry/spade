#include "jit.hpp"
#include "asmjit/core/operand.h"
#include "asmjit/x86/x86operand.h"
#include "callable/foreign.hpp"
#include "ee/value.hpp"
#include "memory/memory.hpp"
#include "callable/method.hpp"

#include <asmjit/x86.h>

namespace spade
{
    void JitCompiler::compile_symbol(const ObjMethod *method) {
        void *fn = assemble_symbol(method, false);
        const auto ffi_fn = halloc<ObjForeign>(method->get_sign(), fn, false);
        vm->set_symbol(method->get_sign().to_string(), ffi_fn);
    }

    void JitCompiler::compile_symbol(Obj *obj, const ObjMethod *method) {
        void *fn = assemble_symbol(method, true);
        const auto ffi_fn = halloc<ObjForeign>(method->get_sign(), fn, true);
        obj->set_member(method->get_sign().get_name(), ffi_fn);
    }

    void *JitCompiler::assemble_symbol(const ObjMethod *method, bool has_self) {
        // const auto module = cast<ObjModule>(vm->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
        // const auto conpool = module->get_constant_pool();

        asmjit::CodeHolder code;
        code.init(runtime.environment(), runtime.cpu_features());
        asmjit::x86::Compiler c(&code);

        // Create the function signature
        auto func = asmjit::FuncSignature::build<void>();
        // Thread *thread;
        func.add_arg_t<Thread *>();
        if (has_self)
            // Obj *self;
            func.add_arg_t<Obj *>();
        // Value *ret
        func.add_arg_t<Value *>();
        // Value ... args
        const size_t args_count = method->get_frame_template().get_args().count();
        for (size_t i = 0; i < args_count; i++) {
            func.add_arg_t<uint64_t>();
            func.add_arg_t<uint64_t>();
        }
        // Now add the function
        auto func_node = c.add_func(func);

        // Set up function virtual registers
        auto thread = c.new_gp_ptr("thread");
        auto self = c.new_gp_ptr("self");
        auto ret = c.new_gp_ptr("ret");
        vector<asmjit::x86::Gp> tags;
        vector<asmjit::x86::Gp> args;
        for (size_t i = 0; i < args.size(); i++) {
            tags.push_back(c.new_gp64("tag%d", i));
            args.push_back(c.new_gp64("arg%d", i));
        }

        // Bind the registers with the argmunents
        size_t arg_idx = 0;
        func_node->set_arg(arg_idx++, thread);
        if (has_self)
            func_node->set_arg(arg_idx++, self);
        func_node->set_arg(arg_idx++, ret);
        for (const auto &arg: args) {
            func_node->set_arg(arg_idx++, arg);
        }

        generate_body(c, has_self, thread, self, ret, args);

        c.end_func();
        c.finalize();

        void *handle = null;
        runtime.add(&handle, &code);
        return handle;
    }

    void JitCompiler::generate_body(asmjit::x86::Compiler &c, bool has_self, const asmjit::x86::Gp &thread, const asmjit::x86::Gp &self,
                                    const asmjit::x86::Gp &ret, const vector<asmjit::x86::Gp> &args) {
        // TODO: to be implemented
        c.mov(asmjit::x86::qword_ptr(ret), asmjit::imm(VALUE_NULL));
        c.mov(asmjit::x86::qword_ptr(ret, 8), asmjit::imm(VALUE_NULL));
        c.ret();
    }
}    // namespace spade
