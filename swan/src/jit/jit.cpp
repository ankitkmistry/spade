#include "jit.hpp"
#include "asmjit/core/globals.h"
#include "asmjit/core/operand.h"
#include "asmjit/x86/x86operand.h"
#include "callable/foreign.hpp"
#include "ee/value.hpp"
#include "memory/memory.hpp"
#include "callable/method.hpp"

#include <asmjit/x86.h>
#include <spdlog/spdlog.h>

/// # x86_64 JIT Function Calling Convention
///
/// The supported types for function arguments are:
/// +----------+----------+---------+-----------------------------------------------+
/// | C type   | VM type  | Size    | Remarks                                       |
/// +----------+----------+---------+-----------------------------------------------+
/// | uint8_t  | bool     | 1 byte  |                                               |
/// | char     | char     | 1 byte  |                                               |
/// | int64_t  | int      | 8 bytes |                                               |
/// | double   | float    | 8 bytes | IEEE-754 double floating-point representation |
/// | Obj*     | pointer  | 8 bytes | 64-bit platforms -> 8 bytes                   |
/// +----------+----------+---------+-----------------------------------------------+
///
/// * Function return values are passed in registers
///     - Integer and pointer return value is passed in rax
///     - Floating point return value is passed in xmm0
/// * Integer and pointer arguments are passed in the following order:
///     - Arguments 1-6 -> rdi, rsi, rdx, rcx, r8, r9
///     - Arguments 7 onwards are pushed on the stack by the caller.
///         The caller is responsible for cleaning up the stack after the call.
///         The caller will push those arguments in reverse order.
///         The caller must align the stack on a 16-byte boundary even if there
///             are no arguments
///         The callee can access those arguments by:
///                 push rbp
///                 mov rbp, rsp
///
///                 [rbp   ] -> saved rbp
///                 [rbp+8 ] -> saved rip
///                 [rbp+16] -> argument 7
///                 [rbp+24] -> argument 8
///                 [rbp+32] -> argument 9
///
///                  0           8           16     24     32
///                 +-----------+-----------+------+------+------+
///                 | saved rbp | saved rip | arg7 | arg8 | arg9 |
///                 +-----------+-----------+------+------+------+
///                  ^
///                  |
///                  rbp
///
///                 mov rsp, rbp
///                 pop rbp
///                 ret
///
/// * Floating point arguments are passed in the following order:
///     - Arguments 1-8 -> xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7
///     - Arguments 9 onwards are pushed on the stack by the caller in the same way
///         as integer and pointer arguments.

namespace spade
{
    void JitCompiler::compile_symbol(const ObjMethod *method) {
        spdlog::info("JitCompiler: Starting compilation of symbol: {}", method->get_sign().to_string());
        void *fn = assemble_symbol(method, false);
        spdlog::info("JitCompiler: Completed assembling of symbol: {}", method->get_sign().to_string());
        const auto ffi_fn = halloc<ObjForeign>(method->get_sign(), fn, false);
        vm->set_symbol(method->get_sign().to_string(), ffi_fn);
        spdlog::info("JitCompiler: Inserted the symbol to vm globals: {}", method->get_sign().to_string());
    }

    void JitCompiler::compile_symbol(Obj *obj, const ObjMethod *method) {
        spdlog::info("JitCompiler: Starting compilation of symbol: {}", method->get_sign().to_string());
        void *fn = assemble_symbol(method, true);
        spdlog::info("JitCompiler: Completed assembling of symbol: {}", method->get_sign().to_string());
        const auto ffi_fn = halloc<ObjForeign>(method->get_sign(), fn, true);
        obj->set_member(method->get_sign().get_name(), ffi_fn);
        spdlog::info("JitCompiler: Inserted the symbol to the object: {}", method->get_sign().to_string());
    }

    void *JitCompiler::assemble_symbol(const ObjMethod *method, bool has_self) {
        // const auto module = cast<ObjModule>(vm->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
        // const auto conpool = module->get_constant_pool();

        asmjit::CodeHolder code;
        code.init(runtime.environment(), runtime.cpu_features());
        code.set_logger(&logger);
        asmjit::x86::Assembler a(&code);

        auto exit = a.new_label();

        // thread, self, ret, args...
        uint64_t locals_size = 0;
        locals_size += sizeof(Thread *);    // Thread *thread
        locals_size += sizeof(Obj *);       // Obj *self
        locals_size += sizeof(Value *);     // Value *ret
        // TODO: other arguments from args

        {
            using namespace asmjit;
            using namespace asmjit::x86;

            // Function prologue
            a.push(rbp);
            a.mov(rbp, rsp);
            // Allocate local variables
            a.sub(rsp, imm(locals_size));
            // arg: Thread *thread
            a.mov(qword_ptr(rbp, -8), rdi);
            // arg: Obj *obj
            a.mov(qword_ptr(rbp, -16), rsi);
            // arg: Value *ret
            a.mov(qword_ptr(rbp, -24), rdx);

            // Function epilogue
            a.bind(exit);
            a.mov(rsp, rbp);
            a.pop(rbp);
            a.ret();
        }

        void *handle = null;
        if (asmjit::Error err = runtime.add(&handle, &code); err != asmjit::Error::kOk) {
            throw FatalError(std::format("JIT compilation failed: {}", asmjit::DebugUtils::error_as_string(err)));
        }
        return handle;
    }

    void generate_body(asmjit::x86::Assembler &a, bool has_self, const asmjit::Label &exit) {
        // TODO: to be implemented
        using namespace asmjit;
        using namespace asmjit::x86;

        a.mov(rax, qword_ptr(rbp, -24));
        a.mov(qword_ptr(rax), imm(VALUE_NULL));
        a.mov(qword_ptr(rax, 8), imm(0));
        a.jmp(exit);
    }
}    // namespace spade
