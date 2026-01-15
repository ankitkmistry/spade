#include "utils.hpp"

#include <bit>
#include <filesystem>
#include <boost/core/demangle.hpp>

namespace spade
{
    std::string cpp_demangle(std::string str) {
        if (str.empty())
            return str;
        return boost::core::demangle(str.c_str());
    }

    std::string join(const std::vector<std::string> &list, const std::string &delimiter) {
        std::string text;
        for (size_t i = 0; i < list.size(); ++i) {
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
        // TODO: raw_to_double and double_to_raw are machine dependent
        // and may produce incorrect results if the machine does not
        // use "IEEE 754" to represent floating-point values

        union Converter {
            uint64_t digits;
            double number;
        } converter{.digits = digits};

        return converter.number;
    }

    uint64_t double_to_raw(double number) {
        // TODO: raw_to_double and double_to_raw are machine dependent
        // and may produce incorrect results if the machine does not
        // use "IEEE 754" to represent floating-point values

        union Converter {
            uint64_t digits;
            double number;
        } converter{.number = number};

        return converter.digits;
    }

    int64_t unsigned_to_signed(uint64_t number) {
        return std::bit_cast<int64_t>(number);
    }

    uint64_t signed_to_unsigned(int64_t number) {
        return std::bit_cast<uint64_t>(number);
    }

    std::string get_absolute_path(const std::string &path) {
        std::filesystem::path p(path);
        if (!p.is_absolute())
            p = std::filesystem::current_path() / p;
        return p.string();
    }
}    // namespace spade
