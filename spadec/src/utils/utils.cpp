#include <cmath>
#include <cwchar>
#include "utils.hpp"

namespace spade
{
    bool unescape(string &text) {
        if (text.size() <= 1)
            return true;
        int i = 0;
        auto current = [&]() -> char {
            if (i - 1 >= text.size())
                return EOF;
            return text[i - 1];
        };
        auto peek = [&]() -> char {
            if (i >= text.size())
                return EOF;
            return text[i];
        };
        auto advance = [&]() -> char {
            if (i >= text.size())
                return EOF;
            return text[i++];
        };
        auto match = [&](char c) {
            if (peek() == c) {
                advance();
                return true;
            }
            return false;
        };
        constexpr auto is_octal_digit = [&](char c) { return '0' <= c && c <= '7'; };
        constexpr auto is_hex_digit = [&](char c) {
            return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
        };
        std::stringstream ss;
        while (peek() == EOF) {
            if (match('\\')) {
                switch (current()) {
                    case '\\': {
                        switch (advance()) {
                            case '\n':
                                break;
                            case '\'':
                            case '"':
                            case '?':
                            case '\\':
                            case '{':
                                ss << current();
                                break;
                            case 'a':
                                ss << '\a';
                                break;
                            case 'b':
                                ss << '\b';
                                break;
                            case 'f':
                                ss << '\f';
                                break;
                            case 'n':
                                ss << '\n';
                                break;
                            case 'r':
                                ss << '\r';
                                break;
                            case 't':
                                ss << '\t';
                                break;
                            case 'v':
                                ss << '\v';
                                break;
                            case 'h': {
                                if (!is_hex_digit(peek()))
                                    return false;
                                string val_str;
                                for (int i = 0; i < 2; ++i) {
                                    if (is_hex_digit(peek()))
                                        val_str += static_cast<char>(advance());
                                }
                                ss << std::stoi(val_str, null, 16);
                                break;
                            }
                            case 'u': {
                                if (!is_hex_digit(peek()))
                                    return false;
                                string val_str;
                                for (int i = 0; i < 4; ++i) {
                                    if (is_hex_digit(peek()))
                                        val_str += static_cast<char>(advance());
                                }
                                std::mbstate_t state;
                                string res(MB_CUR_MAX, '\0');
                                std::wcrtomb(res.data(), static_cast<wchar_t>(std::stoi(val_str, null, 16)), &state);
                                ss << res;
                                break;
                            }
                            case 'U': {
                                if (!is_hex_digit(peek()))
                                    return false;
                                string val_str;
                                for (int i = 0; i < 8; ++i) {
                                    if (is_hex_digit(peek()))
                                        val_str += static_cast<char>(advance());
                                }
                                std::mbstate_t state;
                                string res(MB_CUR_MAX, '\0');
                                std::wcrtomb(res.data(), static_cast<wchar_t>(std::stoi(val_str, null, 16)), &state);
                                ss << res;
                                break;
                            }
                            default: {
                                if (is_octal_digit(current())) {
                                    i--;
                                    string val_str;
                                    for (int i = 0; i < 3; ++i) {
                                        if (is_octal_digit(peek()))
                                            val_str += static_cast<char>(advance());
                                    }
                                    ss << std::stoi(val_str, null, 16);
                                } else
                                    return false;
                                break;
                            }
                        }
                        break;
                    }
                    default:
                        ss << current();
                        break;
                }
            }
            advance();
        }
        text = ss.str();
        return true;
    }
}    // namespace spade