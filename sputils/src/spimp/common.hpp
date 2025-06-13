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
#    warning "Unknown platform"
#endif

#if defined __GNUC__
#    define COMPILER_GCC
#elif defined _MSC_VER
#    if defined __clang__
#        define COMPILER_CLANG
#    else
#        define COMPILER_MSVC
#    endif
#elif defined __clang__
#    define COMPILER_CLANG
#else
#    define COMPILER_OTHER
#endif

#include <format>
#include <string>
#include <vector>

#define null (nullptr)

using std::string;
using std::vector;
