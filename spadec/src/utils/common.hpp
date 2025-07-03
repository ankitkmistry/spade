#pragma once

#include <string>

#include <sputils.hpp>

#define MAX_FUN_CHECK_SEQ (5)

namespace spadec
{
    using namespace spade;
    using std::string;

    namespace fs = std::filesystem;

    template<StringConvertible T>
    std::ostream &operator<<(std::ostream &stream, const T &obj) {
        stream << obj.to_string();
        return stream;
    }
}    // namespace spadec
