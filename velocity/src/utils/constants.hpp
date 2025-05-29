#pragma once

#define FRAMES_MAX 1024

#if defined _WIN32
#define OS_WINDOWS
#pragma warning (disable : 4996)
#pragma warning (disable : 4291)
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
#define OS_LINUX
#elif defined __APPLE__
#define OS_MAC
#endif

#if defined __GNUC__
#define COMPILER_GCC
#elif defined _MSC_VER
#define COMPILER_MSVC
#elif defined __clang__
#define COMPILER_CLANG
#else
#define COMPILER_OTHER
#endif
