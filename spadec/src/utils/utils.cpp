#include <cmath>
#include <cwchar>
#include "utils.hpp"

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

    string cpp_demangle(string str) {
        if (str.empty()) return str;
#if defined COMPILER_MSVC
        static bool sym_init = false;
        if (!sym_init) {
            sym_init = init();
        }
        constexpr int size = 4096;
        std::unique_ptr<char[]> buf(new char[size]);
        auto length = UnDecorateSymbolName(str.c_str(), buf.get(), size, UNDNAME_NAME_ONLY);
        if (length == 0) return str;
        return {buf.get(), buf.get() + length};
#elif defined COMPILER_GCC
        int status;
        char *outStr = abi::__cxa_demangle(str.c_str(), null, null, &status);
        if (status != 0 || outStr == null) return str;
        return string{outStr};
#elif defined COMPILER_OTHER
        return str;
#endif
    }

    bool unescape(string &text) {
        if (text.size() <= 1) return true;
        int i = 0;
        auto current = [&]() -> char {
            if (i - 1 >= text.size()) return EOF;
            return text[i - 1];
        };
        auto peek = [&]() -> char {
            if (i >= text.size()) return EOF;
            return text[i];
        };
        auto advance = [&]() -> char {
            if (i >= text.size()) return EOF;
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
                                if (!is_hex_digit(peek())) return false;
                                string val_str;
                                for (int i = 0; i < 2; ++i) {
                                    if (is_hex_digit(peek())) val_str += static_cast<char>(advance());
                                }
                                ss << std::stoi(val_str, null, 16);
                                break;
                            }
                            case 'u': {
                                if (!is_hex_digit(peek())) return false;
                                string val_str;
                                for (int i = 0; i < 4; ++i) {
                                    if (is_hex_digit(peek())) val_str += static_cast<char>(advance());
                                }
                                std::mbstate_t state;
                                string res(MB_CUR_MAX, '\0');
                                std::wcrtomb(res.data(), static_cast<wchar_t>(std::stoi(val_str, null, 16)), &state);
                                ss << res;
                                break;
                            }
                            case 'U': {
                                if (!is_hex_digit(peek())) return false;
                                string val_str;
                                for (int i = 0; i < 8; ++i) {
                                    if (is_hex_digit(peek())) val_str += static_cast<char>(advance());
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
                                        if (is_octal_digit(peek())) val_str += static_cast<char>(advance());
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