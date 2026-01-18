#include "jit.hpp"
#include "asmjit/core/codeholder.h"
#include "asmjit/core/func.h"
#include "asmjit/core/operand.h"
#include "asmjit/x86/x86assembler.h"
#include "asmjit/x86/x86compiler.h"
#include "asmjit/x86/x86operand.h"
#include "callable/foreign.hpp"
#include "ee/value.hpp"
#include "ee/vm.hpp"
#include "memory/memory.hpp"
#include "callable/method.hpp"
#include "spimp/utils.hpp"
#include "spinfo/opcode.hpp"

#include <asmjit/x86.h>
#include <cassert>
#include <exception>
#include <iostream>
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
///                 leave
///                 ret
///
/// * Floating point arguments are passed in the following order:
///     - Arguments 1-8 -> xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7
///     - Arguments 9 onwards are pushed on the stack by the caller in the same way
///         as integer and pointer arguments.
///
/// * JIT Function Signature
///     A compiled JIT function will have a signature like this:
///         void handle(Obj *self, Value *ret);
///     or
///         void handle(Obj *self, Value *ret, const Value *arg0);
///     or
///         void handle(Obj *self, Value *ret, const Value *arg0, const Value *arg1);
///     and so on ...

namespace spade
{
    static void print_bytecode(const SpadeVM *vm, const ObjMethod *method) {
        const auto module = cast<ObjModule>(vm->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());

        const auto code = method->get_code();
        const auto ip = code;
        const auto code_count = method->get_code_count();
        const auto &pool = module->get_constant_pool();
        const auto &line_table = method->get_lines();

        if (code_count == 0)
            return;

        const auto byte_line_max_len = std::to_string(code_count - 1).length();
        const auto source_line_max_len = std::to_string(line_table.get_line_infos().back().source_line).length() + 2;
        uint64_t source_line = 0;
        uint32_t i = 0;
        const auto read_byte = [&i, code]() -> uint8_t { return code[i++]; };
        const auto read_short = [&i, code]() -> uint16_t {
            i += 2;
            return code[i - 2] << 8 | code[i - 1];
        };

        while (i < code_count) {
            // Compute source line
            uint64_t source_line_tmp = line_table.get_source_line(i);
            string source_line_str;
            if (source_line != source_line_tmp) {
                // If the current source line is different from the prev one then show the line number
                source_line = source_line_tmp;
                source_line_str = pad_right(std::to_string(source_line) + " |", source_line_max_len);
            } else {
                // If the current source line is same as the prev source line then do not show the line number
                source_line_str = pad_right(" |", source_line_max_len);
            }

            // Get the start of the line
            const auto start = i;
            // Get the opcode
            const auto opcode = static_cast<Opcode>(read_byte());
            // Evaluate parameters of the opcode
            string param;
            switch (OpcodeInfo::params_count(opcode)) {
            case 1: {
                const auto num = read_byte();
                string val_str = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num].to_string()) : "";
                param = std::format("{} {}", num, val_str);
                break;
            }
            case 2: {
                uint16_t num = read_short();
                switch (opcode) {
                case Opcode::JMP:
                case Opcode::JT:
                case Opcode::JF:
                case Opcode::JLT:
                case Opcode::JLE:
                case Opcode::JEQ:
                case Opcode::JNE:
                case Opcode::JGE:
                case Opcode::JGT: {
                    const auto offset = static_cast<int16_t>(num);
                    param = std::to_string(offset);
                    break;
                }
                default:
                    string val_str = OpcodeInfo::take_from_const_pool(opcode) ? std::format("({})", pool[num].to_string()) : "";
                    param = std::format("{} {}", num, val_str);
                    break;
                }
                break;
            }
            default:
                param = "";
                if (opcode == Opcode::CLOSURELOAD) {
                    const auto count = read_byte();
                    param.append("[");
                    for (uint8_t i = 0; i < count; i++) {
                        const auto local_idx = read_short();
                        string kind;
                        size_t to_idx;
                        switch (read_byte()) {
                        case 0:
                            kind = "arg";
                            to_idx = read_byte();
                            break;
                        case 1:
                            kind = "local";
                            to_idx = read_short();
                            break;
                        default:
                            throw Unreachable();
                        }
                        param += std::format("{}->{}({}), ", local_idx, kind, to_idx);
                    }
                    if (param.back() != '[') {
                        param.pop_back();
                        param.pop_back();
                    }
                    param.append("]");
                }
                break;
            }

            std::cout << std::format("  {: >{}}: {} {} {}\n", start, byte_line_max_len, source_line_str, OpcodeInfo::to_string(opcode), param);
        }
    }

    void JitCompiler::compile_symbol(const ObjMethod *method) {
        spdlog::info("JitCompiler: Starting compilation of symbol: {}", method->get_sign().to_string());
        spdlog::info("JitCompiler: BYTECODE START ===================");
        print_bytecode(vm, method);
        spdlog::info("JitCompiler: BYTECODE END =====================");
        void *fn = assemble_symbol(method, false);
        spdlog::info("JitCompiler: Completed assembling of symbol: {}", method->get_sign().to_string());
        // const auto ffi_fn = halloc<ObjForeign>(method->get_sign(), fn, false);
        // vm->set_symbol(method->get_sign().to_string(), ffi_fn);
        // spdlog::info("JitCompiler: Inserted the symbol to vm globals: {}", method->get_sign().to_string());
    }

    void JitCompiler::compile_symbol(Obj *obj, const ObjMethod *method) {
        spdlog::info("JitCompiler: Starting compilation of symbol: {}", method->get_sign().to_string());
        spdlog::info("JitCompiler: BYTECODE START ===================");
        print_bytecode(vm, method);
        spdlog::info("JitCompiler: BYTECODE END =====================");
        void *fn = assemble_symbol(method, true);
        spdlog::info("JitCompiler: Completed assembling of symbol: {}", method->get_sign().to_string());
        // const auto ffi_fn = halloc<ObjForeign>(method->get_sign(), fn, true);
        // obj->set_member(method->get_sign().get_name(), ffi_fn);
        // spdlog::info("JitCompiler: Inserted the symbol to the object: {}", method->get_sign().to_string());
    }

    static Obj *jit_concat(Obj *a, Obj *b) {
        return cast<ObjString>(a)->concat(cast<ObjString>(b));
    }

    static void jit_println(SpadeVM *vm, const Value *val) {
        spdlog::trace("jit_println: {}", val->to_string());
        vm->write(val->to_string() + "\n");
    }

    class FunctionBodyGen {
        const ObjMethod *method;
        asmjit::x86::Assembler &a;
        bool has_self;
        asmjit::Label exit;

        SpadeVM *vm;
        uint32_t pc = 0;
        int64_t sc;
        vector<Value> conpool;
        vector<int64_t> local_positions;

      public:
        FunctionBodyGen(const ObjMethod *method, asmjit::x86::Assembler &a, bool has_self, const asmjit::Label &exit, int64_t stack_counter)
            : method(method),
              a(a),
              has_self(has_self),
              exit(exit),
              vm(method->get_info().manager->get_vm()),
              sc(stack_counter),
              conpool(cast<ObjModule>(                                                                    //
                              method->get_info()                                                          //
                                      .manager                                                            //
                                      ->get_vm()                                                          //
                                      ->get_symbol(method->get_sign().get_parent_module().to_string())    //
                                      .as_obj()                                                           //
                              )
                              ->get_constant_pool()),
              local_positions(method->get_locals_count()) {}

        void generate() {
            // TODO: to be implemented
            using namespace asmjit;
            using namespace asmjit::x86;


            for (size_t i = 0; i < method->get_locals_count(); i++) {
                spdlog::trace("FunctionBodyGen: allocate local index {}", i);
                push_null();
                local_positions[i] = sc;
            }

            while (pc < method->get_code_count()) {
                Opcode opcode = static_cast<Opcode>(read_byte());
                switch (opcode) {
                case Opcode::CONST: {
                    const auto index = read_byte();
                    spdlog::trace("FunctionBodyGen: const ({})", conpool[index].to_string());
                    push(conpool[index]);
                    break;
                }
                case Opcode::LFLOAD: {
                    const auto index = read_byte();
                    spdlog::trace("FunctionBodyGen: lfload {}", index);
                    load_local(index);
                    break;
                }
                case Opcode::PLFSTORE: {
                    const auto index = read_byte();
                    spdlog::trace("FunctionBodyGen: plfstore {}", index);
                    a.mov(rax, qword_ptr(rbp, sc + 8));
                    a.mov(qword_ptr(rbp, local_positions[index] + 8), rax);
                    a.mov(rax, qword_ptr(rbp, sc));
                    a.mov(qword_ptr(rbp, local_positions[index]), rax);
                    sc += 16;    // Pop one value
                    break;
                }
                case Opcode::CONCAT: {
                    // FIXME: concat does not work, I don't know why
                    spdlog::trace("FunctionBodyGen: concat");
                    a.mov(reg_arg0(), qword_ptr(rbp, sc + 24));
                    a.mov(reg_arg1(), qword_ptr(rbp, sc + 8));
                    a.call(imm((void *) jit_concat));
                    sc += 32;    // Pop two values
                    sc -= 16;    // Push the concat value
                    a.mov(qword_ptr(rbp, sc + 8), rax);
                    a.mov(qword_ptr(rbp, sc), imm(VALUE_OBJ));
                    break;
                }
                case Opcode::PRINTLN: {
                    spdlog::trace("FunctionBodyGen: println");
                    a.mov(reg_arg0(), imm(vm));
                    a.lea(reg_arg1(), qword_ptr(rbp, sc));
                    a.call(imm((void *) jit_println));
                    sc += 16;    // Pop one values
                    break;
                }
                case Opcode::VRET: {
                    spdlog::trace("FunctionBodyGen: vret");
                    // if (pc < method->get_code_count() - 1)
                    a.jmp(exit);
                    break;
                }
                default:
                    spdlog::error("FunctionBodyGen: opcode '{}' not implemented", OpcodeInfo::to_string(opcode));
                    std::exit(1);
                }
            }
        }

      private:
        void load_local(size_t index) {
            using namespace asmjit;
            using namespace asmjit::x86;

            sc -= 16;

            a.mov(rax, qword_ptr(rbp, local_positions[index] + 8));
            a.mov(qword_ptr(rbp, sc + 8), rax);
            a.mov(rax, qword_ptr(rbp, local_positions[index]));
            a.mov(qword_ptr(rbp, sc), rax);
        }

        void push_null() {
            using namespace asmjit;
            using namespace asmjit::x86;

            sc -= 16;

            // Push the value
            a.mov(qword_ptr(rbp, sc + 8), imm(0));
            a.mov(qword_ptr(rbp, sc), imm(0));
        }

        void push(Value value) {
            using namespace asmjit;
            using namespace asmjit::x86;

            sc -= 16;

            // Push the value
            switch (value.get_tag()) {
            case VALUE_NULL:
                a.mov(qword_ptr(rbp, sc + 8), imm(0));
                break;
            case VALUE_BOOL:
                if (value.as_bool())
                    a.mov(qword_ptr(rbp, sc + 8), imm(1));
                else
                    a.mov(qword_ptr(rbp, sc + 8), imm(0));
                break;
            case VALUE_CHAR:
                a.mov(qword_ptr(rbp, sc + 8), imm(value.as_char()));
                break;
            case VALUE_INT:
                a.mov(qword_ptr(rbp, sc + 8), imm(value.as_int()));
                break;
            case VALUE_FLOAT:
                a.mov(qword_ptr(rbp, sc + 8), imm(value.as_float()));
                break;
            case VALUE_OBJ:
                a.mov(qword_ptr(rbp, sc + 8), imm(value.as_obj()));
                break;
            }
            // Push the value tag
            a.mov(qword_ptr(rbp, sc), imm(value.get_tag()));
        }

        uint8_t read_byte() {
            return method->get_code()[pc++];
        }

        uint16_t read_short() {
            auto b1 = read_byte();
            auto b2 = read_byte();
            return (b1 << 8) | b2;
        }

        asmjit::x86::Gp reg_arg0() {
#if defined OS_WINDOWS
            return asmjit::x86::rcx;
#elif OS_LINUX
            return asmjit::x86::rdi;
#else
#    error Implement this
#endif
        }

        asmjit::x86::Gp reg_arg1() {
#if defined OS_WINDOWS
            return asmjit::x86::rdx;
#elif OS_LINUX
            return asmjit::x86::rsi;
#else
#    error Implement this
#endif
        }
    };

    void *JitCompiler::assemble_symbol(const ObjMethod *method, bool has_self) {
        // const auto module = cast<ObjModule>(vm->get_symbol(method->get_sign().get_parent_module().to_string()).as_obj());
        // const auto conpool = module->get_constant_pool();

        asmjit::CodeHolder code;
        code.init(runtime.environment(), runtime.cpu_features());
        code.set_logger(&logger);
        asmjit::x86::Assembler a(&code);

        auto exit = a.new_label();

        // self, ret, args...
        uint64_t stack_size = 0;
        stack_size += sizeof(Obj *);                                       // Obj *self
        stack_size += sizeof(Value *);                                     // Value *ret
        stack_size += sizeof(const Value *) * method->get_args_count();    // args...
        stack_size += sizeof(Value) * method->get_locals_count();          // locals
        stack_size += sizeof(Value) * method->get_stack_max();             // stack

        int64_t stack_counter = 0;

        {
            using namespace asmjit;
            using namespace asmjit::x86;

            // Function prologue
            a.push(rbp);
            a.mov(rbp, rsp);
            // Allocate local variables
            a.sub(rsp, imm(stack_size));
            // arg: Obj *self
            spdlog::trace("JitCompiler: allocate 'Obj *self'");
            stack_counter -= sizeof(Obj *);
            a.mov(qword_ptr(rbp, stack_counter), rdi);
            // arg: Value *ret
            spdlog::trace("JitCompiler: allocate 'Value *ret'");
            stack_counter -= sizeof(Value *);
            a.mov(qword_ptr(rbp, stack_counter), rsi);
            if (method->get_args_count() >= 1) {
                spdlog::trace("JitCompiler: allocate 'const Value *arg0'");
                stack_counter -= sizeof(const Value *);
                a.mov(qword_ptr(rbp, stack_counter), rdx);
            }
            if (method->get_args_count() >= 2) {
                spdlog::trace("JitCompiler: allocate 'const Value *arg1'");
                stack_counter -= sizeof(const Value *);
                a.mov(qword_ptr(rbp, stack_counter), rcx);
            }
            if (method->get_args_count() >= 3) {
                spdlog::trace("JitCompiler: allocate 'const Value *arg2'");
                stack_counter -= sizeof(const Value *);
                a.mov(qword_ptr(rbp, stack_counter), r8);
            }
            if (method->get_args_count() >= 4) {
                spdlog::trace("JitCompiler: allocate 'const Value *arg3'");
                stack_counter -= sizeof(const Value *);
                a.mov(qword_ptr(rbp, stack_counter), r9);
            }

            // TODO: arg count more than 4 is not supported
            if (method->get_args_count() > 4) {
                spdlog::error("JITCompiler: JIT compiled functions cannot have more than 4 arguments for now");
                std::exit(1);
            }

            // Generate function body
            FunctionBodyGen(method, a, has_self, exit, stack_counter).generate();

            // Function epilogue
            a.bind(exit);
            a.leave();
            a.ret();
        }

        void *handle = null;
        if (asmjit::Error err = runtime.add(&handle, &code); err != asmjit::Error::kOk) {
            throw FatalError(std::format("JIT compilation failed: {}", asmjit::DebugUtils::error_as_string(err)));
        }
        test_symbol(handle);
        return handle;
    }

    void JitCompiler::test_symbol(void *handle) {
        spdlog::trace("JitCompiler: Compiling helper test function");

        asmjit::CodeHolder code;
        code.init(runtime.environment(), runtime.cpu_features());
        code.set_logger(&logger);
        asmjit::x86::Assembler a(&code);
        {
            using namespace asmjit;
            using namespace asmjit::x86;
            /// sub rsp, 16                 ; Allocate return value on the stack
            /// mov qword ptr [rsp], 0      ; Set return value.tag=0
            /// mov qword ptr [rsp+8], 0    ; Set return value.as=0
            /// xor rdi, rdi                ; Set rdi=0 (Obj *self = null)
            /// mov rsi, rsp                ; Set rsp to return value
            /// call handle                 ; Call the jit function
            /// add rsp 16                  ; Deallocate the return value
            /// ret
            a.sub(rsp, imm(16));
            a.mov(qword_ptr(rsp), 0);
            a.mov(qword_ptr(rsp, 8), imm(0));
            a.xor_(rdi, rdi);
            a.mov(rsi, rsp);
            a.call(imm(handle));
            a.add(rsp, imm(16));
            a.ret();
        }
        void (*fn)();
        runtime.add(&fn, &code);
        fn();
    }
}    // namespace spade
