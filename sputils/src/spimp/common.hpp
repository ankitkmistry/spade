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
#    define COMPILER_MSVC
#else
#    define COMPILER_OTHER
#endif

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <format>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>

#define null (nullptr)

using std::string;
using std::vector;
