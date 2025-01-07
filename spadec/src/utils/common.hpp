#pragma once

#if defined(_WIN32)
#    define OS_WINDOWS    /// Windows
#elif defined(_WIN64)
#    define OS_WINDOWS    /// Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
#    define OS_WINDOWS    /// Windows (Cygwin POSIX under Microsoft Windows)
#elif defined(__linux__) || defined(__ANDROID__)
#    define OS_LINUX    /// Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
#elif defined(__APPLE__) && defined(__MACH__)
#    define OS_MAC    /// Apple OSX and iOS (Darwin)
#else
#    error Unknown platform
#endif

#if defined __GNUC__
#    define COMPILER_GCC
#elif defined _MSC_VER
#    define COMPILER_MSVC
#else
#    define COMPILER_OTHER
#endif

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

#define null (nullptr)

namespace spade
{
    using std::string;
    namespace fs = std::filesystem;

    template<typename T>
    concept StringConvertible = requires(T t) {
        { t.to_string() } -> std::same_as<string>;
    };

    template<StringConvertible T>
    std::ostream &operator<<(std::ostream &stream, const T &obj) {
        stream << obj.to_string();
        return stream;
    }
}    // namespace spade
