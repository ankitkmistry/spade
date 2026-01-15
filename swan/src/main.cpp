#include <asmjit/x86.h>
#include <iostream>

using namespace ::asmjit;
using namespace ::asmjit::x86;

int main() {
    JitRuntime rt;
    FileLogger logger(stdout);

    CodeHolder code;
    code.init(rt.environment(), rt.cpu_features());
    code.set_logger(&logger);

    Assembler a(&code);
    auto start = a.new_label();
    auto end = a.new_label();

    ///     push rbp
    ///     mov rbp, rsp
    ///
    ///     xor rax, rax
    ///
    /// start:
    ///     test rcx, rcx
    ///     jz end
    ///
    ///     add rax, rcx
    ///     dec rcx
    ///     jmp start
    ///
    /// end:
    ///     mov rsp, rbp
    ///     pop rbp
    ///     ret

    // a.push(rbp);
    // a.mov(rbp, rsp);
    //
    // a.xor_(rax, rax);
    //
    // a.bind(start);
    // a.test(rcx, rcx);
    // a.jz(end);
    //
    // a.add(rax, rcx);
    // a.dec(rcx);
    // a.jmp(start);
    //
    // a.bind(end);
    // a.mov(rsp, rbp);
    // a.pop(rbp);
    // a.ret();

    ///     mov rax, rcx
    ///     dec rcx
    ///     mul rcx
    ///     shr rax, 1
    ///     ret

    ///     lfload 0
    ///     dup
    ///     const 1
    ///     add
    ///     mul
    ///     const 1
    ///     ushr
    ///     ret

    a.mov(rax, rcx);
    a.inc(rcx);
    a.mul(rcx);
    a.shr(rax, 1);
    a.ret();

    uint64_t (*fn)(uint64_t);
    Error err = rt._add((void **) &fn, &code);
    if (err != Error::kOk) {
        std::cerr << "JIT compilation failed: " << DebugUtils::error_as_string(err);
        return 1;
    }

    auto result = fn(126806);
    std::cout << "Result: fn() = " << result;

    return 0;
}
