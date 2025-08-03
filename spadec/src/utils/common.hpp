#pragma once

#include <string>
#include <filesystem>

#ifdef SPDLOG_ACTIVE_LEVEL
#    undef SPDLOG_ACTIVE_LEVEL
#endif
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>

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
