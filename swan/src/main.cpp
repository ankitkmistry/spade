#include "ee/vm.hpp"
#include "jit/jit.hpp"
#include "memory/basic/basic_manager.hpp"
#include "spimp/utils.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

using namespace spade;

int main() {
    spdlog::set_level(spdlog::level::trace);

    basic::BasicMemoryManager mgr;
    SpadeVM vm(&mgr);
    vm.start("../swan/res/hello.elp", {}, true);
    std::cout << "Output:\n";
    std::cout << vm.get_output();

    JitCompiler compiler(&vm);
    compiler.compile_symbol(cast<ObjMethod>(vm.get_symbol("hello.greet()").as_obj()));
    return 0;
}
