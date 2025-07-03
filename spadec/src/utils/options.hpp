#pragma once

#include "common.hpp"

namespace spadec
{
    struct CompilerOptions {
        fs::path basic_module_path;
        std::vector<fs::path> import_search_dirs;

        // Warning options
        bool w_error = false;
    };
}    // namespace spadec