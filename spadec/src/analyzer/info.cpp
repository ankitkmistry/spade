#include <numeric>

#include "info.hpp"
#include "scope.hpp"
#include "spimp/error.hpp"

namespace spade
{
    string TypeInfo::to_string() const {
        if (is_type_literal())
            return "type";
        string result = type->to_string();
        if (!type_args.empty()) {
            result += "[";
            result += std::accumulate(type_args.begin(), type_args.end(), string(), [](const string &s, TypeInfo type_info) {
                string t_info_str = type_info.to_string();
                return s + (s.empty() ? "" : ",") + t_info_str.substr(t_info_str.find_first_of(' ') + 1);
            });
            result += "]";
        }
        return result + (b_nullable ? "?" : "");
    }

    string ExprInfo::to_string() const {
        switch (tag) {
            case Type::NORMAL:
            case Type::STATIC:
                return type_info.to_string();
            case Type::MODULE:
                return module->to_string();
            case Type::FUNCTION:
                return function->to_string();
            default:
                throw Unreachable();    // to remove MSVC warning
        }
    }
}    // namespace spade