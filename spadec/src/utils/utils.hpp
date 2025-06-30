#pragma once

#include "../utils/common.hpp"

namespace spade
{
    inline static string escape_str(const string &text) {
        string result;
        for (const auto c: text) {
            switch (c) {
            case '\'':
                result += "\\'";
                break;
            case '\"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\a':
                result += "\\a";
                break;
            case '\b':
                result += "\\b";
                break;
            case '\f':
                result += "\\f";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            case '\v':
                result += "\\v";
                break;
            default:
                result += c;
                break;
            }
        }
        return result;
    }
}    // namespace spade