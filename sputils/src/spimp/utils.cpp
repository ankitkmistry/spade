#include "utils.hpp"
#include "common.hpp"

#include <filesystem>
#if defined COMPILER_MSVC
#    include <windows.h>
#    include <dbghelp.h>

#elif defined COMPILER_GCC
#    include <cxxabi.h>
#endif
namespace spade
{
#if defined COMPILER_MSVC
    bool init() {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        if (!SymInitialize(GetCurrentProcess(), NULL, TRUE)) {
            return false;
        }
        return true;
    }
#endif

    std::string cpp_demangle(std::string str) {
        if (str.empty())
            return str;
#if defined COMPILER_MSVC
        static bool sym_init = false;
        if (!sym_init) {
            sym_init = init();
        }
        constexpr int size = 4096;
        std::unique_ptr<char[]> buf(new char[size]);
        auto length = UnDecorateSymbolName(str.c_str(), buf.get(), size, UNDNAME_NAME_ONLY);
        if (length == 0)
            return str;
        return {buf.get(), buf.get() + length};
#elif defined COMPILER_GCC
        int status;
        char *outStr = abi::__cxa_demangle(str.c_str(), null, null, &status);
        if (status != 0 || outStr == null)
            return str;
        return string{outStr};
#elif defined COMPILER_OTHER
        return str;
#endif
    }

    std::string join(const std::vector<std::string> &list, const std::string &delimiter) {
        std::string text;
        for (int i = 0; i < list.size(); ++i) {
            text += list[i];
            if (i < list.size() - 1)
                text += delimiter;
        }
        return text;
    }

    std::string pad_right(const std::string &str, size_t length) {
        return str.size() < length ? std::string(length - str.size(), ' ') + str : str;
    }

    std::string pad_left(const std::string &str, size_t length) {
        return str.size() < length ? str + std::string(length - str.size(), ' ') : str;
    }

    bool is_number(const std::string &s) {
        return !s.empty() && std::ranges::find_if(s, [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    int32_t long_to_int(int64_t num) {
        int sign = num >> 63 == 0 ? 1 : -1;
        return static_cast<int32_t>(sign * (sign * num & 0xffffffff));
    }

    double raw_to_double(uint64_t digits) {
        union Converter {
            uint64_t digits;
            double number;
        } converter{.digits = digits};

        return converter.number;
    }

    uint64_t double_to_raw(double number) {
        union Converter {
            uint64_t digits;
            double number;
        } converter{.number = number};

        return converter.digits;
    }

    int64_t unsigned_to_signed(uint64_t number) {
        union Converter {
            uint64_t number1;
            int64_t number2;
        } converter{.number1 = number};

        return converter.number2;
    }

    uint64_t signed_to_unsigned(int64_t number) {
        union Converter {
            uint64_t number1;
            int64_t number2;
        } converter{.number2 = number};

        return converter.number1;
    }

    std::string get_absolute_path(const std::string &path) {
        std::filesystem::path p(path);
        if (!p.is_absolute())
            p = std::filesystem::current_path() / p;
        return p.string();
    }
}    // namespace spade