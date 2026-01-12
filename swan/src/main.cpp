#include <asmjit/x86.h>
#include <iostream>

using namespace ::asmjit;
using namespace ::asmjit::x86;

int main() {
    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment(), rt.cpu_features());

    Assembler a(&code);

    a.mov(rax, 69);
    a.lea(rax, ptr(0, rax, 2));
    a.ret();

    int64_t (*fn)(void);
    Error err = rt._add((void **) &fn, &code);
    if (err != Error::kOk) {
        std::cerr << "JIT compilation failed: " << DebugUtils::error_as_string(err);
        return 1;
    }

    auto result = fn();
    std::cout << "Result: " << result;

    return 0;
}
