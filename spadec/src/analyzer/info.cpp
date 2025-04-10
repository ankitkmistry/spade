#include <numeric>

#include "info.hpp"
#include "scope.hpp"
#include "spimp/error.hpp"

namespace spade
{
    string TypeInfo::to_string(bool decorated) const {
        if (is_type_literal())
            return "type";
        string result = type->to_string(decorated);
        if (!type_args.empty()) {
            result += "[";
            result += std::accumulate(type_args.begin(), type_args.end(), string(),
                                      [decorated](const string &s, const TypeInfo &type_info) {
                                          return s + (s.empty() ? "" : ",") + type_info.to_string(decorated);
                                      });
            result += "]";
        }
        return result + (b_nullable ? "?" : "");
    }

    string ExprInfo::to_string(bool decorated) const {
        switch (tag) {
            case Type::NORMAL:
            case Type::STATIC:
                return type_info.to_string(decorated);
            case Type::MODULE:
                return module->to_string(decorated);
            case Type::FUNCTION_SET:
                return function_set->to_string(decorated);
            default:
                throw Unreachable();    // to remove MSVC warning
        }
    }

    string ParamInfo::to_string(bool decorated) const {
        return std::format("{}{}{}", (b_variadic ? "*" : ""), (b_kwd_only ? name + ":" : ""), type_info.to_string(decorated));
    }

    string ArgInfo::to_string(bool decorated) const {
        return expr_info.to_string(decorated);
    }
}    // namespace spade