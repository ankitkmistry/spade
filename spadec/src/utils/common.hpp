#pragma once

#include <cstddef>
#include <cstdlib>

#include <stdexcept>
#include <concepts>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <format>
#include <filesystem>
#include <map>
#include <functional>

#include <sputils.hpp>

// #define ENABLE_MT

namespace spade
{
    using std::string;
    namespace fs = std::filesystem;

    template<StringConvertible T>
    std::ostream &operator<<(std::ostream &stream, const T &obj) {
        stream << obj.to_string();
        return stream;
    }
}    // namespace spade
