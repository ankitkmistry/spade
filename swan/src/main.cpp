#include <iostream>
#include "ee/vm.hpp"

using namespace spade;

int main() {
    try {
        SpadeVM vm(null);
        vm.start("../velocity/res/hello.elp", {}, true);
        return vm.get_exit_code();
    } catch (const SpadeError &error) {
        std::cout << "VM Error: " << error.what() << std::endl;
        return 1;
    }
}
