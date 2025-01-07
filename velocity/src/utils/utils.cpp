#include "utils.hpp"
#include "common.hpp"

#if defined COMPILER_MSVC

#    include <dbghelp.h>

#elif defined COMPILER_GCC

#    include <cxxabi.h>

#endif

namespace spade
{
    string padRight(const string &str, size_t length) {
        return str.size() < length ? std::string(length - str.size(), ' ') + str : str;
    }

    string padLeft(const string &str, size_t length) {
        return str.size() < length ? str + std::string(length - str.size(), ' ') : str;
    }

    bool isNumber(const string &s) {
        return !s.empty() && std::ranges::find_if(s, [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    int32 longToInt(int64 num) {
        int sign = num >> 63 == 0 ? 1 : -1;
        return static_cast<int32>(sign * (sign * num & 0xffffffff));
    }

    double rawToDouble(uint64 digits) {
        union Converter {
            uint64 digits;
            double number;
        } converter{.digits = digits};

        return converter.number;
    }

    uint64 doubleToRaw(double number) {
        union Converter {
            uint64 digits;
            double number;
        } converter{.number = number};

        return converter.digits;
    }

    int64 unsignedToSigned(uint64 number) {
        union Converter {
            uint64 number1;
            int64 number2;
        } converter{.number1 = number};

        return converter.number2;
    }

    uint64 signedToUnsigned(int64 number) {
        union Converter {
            uint64 number1;
            int64 number2;
        } converter{.number2 = number};

        return converter.number1;
    }

    int find(const string &text, char c, int start, int end) {
        for (int i = start; i < end; ++i) {
            if (text[i] == c) return i;
        }
        return text.length();
    }

    int find(const string &text, char c) { return find(text, c, 0, text.length()); }

    string getAbsolutePath(const string &path) {
        fs::path p(path);
        if (!p.is_absolute()) p = fs::current_path() / p;
        return p.string();
    }

    string cpp_demangle(const string &str) {
        if (str.empty()) return str;
#if defined COMPILER_MSVC
        char *outStr;
        auto length = UnDecorateSymbolName(str.c_str(), outStr, -1, UNDNAME_NAME_ONLY);
        if (length == 0) return str;
        return string{outStr, length};
#elif defined COMPILER_GCC
        int status;
        char *outStr = abi::__cxa_demangle(str.c_str(), null, null, &status);
        if (status != 0 || outStr == null) return str;
        return string{outStr};
#elif defined COMPILER_OTHER
        return str;
#endif
    }
} // namespace spade