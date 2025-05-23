#pragma once

#include "common.hpp"

namespace spade
{
    struct CompilerOptions {
        fs::path basic_module_path;
        std::vector<fs::path> import_search_dirs;
    };
}    // namespace spade