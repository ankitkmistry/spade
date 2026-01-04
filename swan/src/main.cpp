#include "ee/vm.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace spade;

int main(int argc, char **argv) {
    vector<string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    auto logger = spdlog::stdout_color_mt("console");
    logger->set_pattern("[%Y-%m-%d %H:%M:%S %z] [thread %t] [%^%l%$] %v");
    spdlog::set_default_logger(logger);

    try {
        SpadeVM vm(null);
        vm.start("../swan/res/hello.elp", args, true);
        std::cout << vm.get_output();
        spdlog::info("VM exited with code {}", vm.get_exit_code());
        return vm.get_exit_code();
    } catch (const SpadeError &error) {
        std::cerr << "VM Error: " << error.what() << std::endl;
        return 1;
    }
}
