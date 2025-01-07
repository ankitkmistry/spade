#ifndef VELOCITY_CONSTANTS_HPP
#define VELOCITY_CONSTANTS_HPP

#define FRAMES_MAX 65536

#if defined _WIN32
#define OS_WINDOWS
#pragma warning (disable : 4996)
#pragma warning (disable : 4291)
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
#define OS_LINUX
#elif defined (__APPLE__)
#define OS_MAC
#endif

#if defined __GNUC__
#define COMPILER_GCC
#elif defined _MSC_VER
#define COMPILER_MSVC
#else
#define COMPILER_OTHER
#endif

#endif //VELOCITY_CONSTANTS_HPP
