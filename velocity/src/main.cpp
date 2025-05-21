#include <iostream>

#include "ee/vm.hpp"
#include "memory/basic/basic_manager.hpp"

using namespace spade;

void sign_test() {
    Sign sign1{"A::B"};
    std::cout << sign1.to_string() << std::endl;
    Sign sign2{"A::B.C"};
    std::cout << sign2.to_string() << std::endl;
    Sign sign3{"A::B.C()"};
    std::cout << sign3.to_string() << std::endl;
    Sign sign4{"A::B.C[T,V]"};
    std::cout << sign4.to_string() << std::endl;
    Sign sign5{"A::B.C[T](A.int,A.str)"};
    std::cout << sign5.to_string() << std::endl;
    Sign sign6{"A.B"};
    std::cout << sign6.to_string() << std::endl;
    Sign sign7{".B"};
    std::cout << sign7.to_string() << std::endl;
    Sign sign8{".B(B.int)"};
    std::cout << sign8.to_string() << std::endl;
}

int run_vm() {
    try {
        basic::BasicMemoryManager manager;
        SpadeVM vm(&manager);
#if defined(OS_LINUX)
        return vm.start("../velocity/res/hello.elp", {});
#elif defined(OS_WINDOWS)
        return vm.start("..\\velocity\\res\\hello.elp", {});
#endif
    } catch (const SpadeError &error) {
        std::cout << "VM Error: " << error.what() << std::endl;
        return 1;
    }
}

int main() {
    std::ios_base::sync_with_stdio(true);
    std::cout << std::boolalpha;
    // sign_test();
    return run_vm();
}
