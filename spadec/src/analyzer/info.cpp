#include "info.hpp"
#include "scope.hpp"

namespace spade
{
    bool BasicType::weak_equals(const BasicType &other) const {
        return type == other.type && type_args == other.type_args;
    }

    bool BasicType::operator==(const BasicType &other) const {
        return type == other.type && b_nullable == other.b_nullable && type_args == other.type_args;
    }

    FunctionType::FunctionType() : m_ret_type(std::make_unique<TypeInfo>()), m_pos_only_params(), m_pos_kwd_params(), m_kwd_only_params() {}

    FunctionType::FunctionType(const FunctionType &other)
        : m_ret_type(std::make_unique<TypeInfo>(*other.m_ret_type)),
          m_pos_only_params(other.m_pos_only_params),
          m_pos_kwd_params(other.m_pos_kwd_params),
          m_kwd_only_params(other.m_kwd_only_params) {}

    FunctionType &FunctionType::operator=(const FunctionType &other) {
        m_ret_type = std::make_unique<TypeInfo>(*other.m_ret_type);
        m_pos_only_params = other.m_pos_only_params;
        m_pos_kwd_params = other.m_pos_kwd_params;
        m_kwd_only_params = other.m_kwd_only_params;
        return *this;
    }

    FunctionType::FunctionType(FunctionType &&) = default;
    FunctionType &FunctionType::operator=(FunctionType &&) = default;
    FunctionType::~FunctionType() = default;

    bool FunctionType::is_variadic() const {
        return !pos_only_params().empty() && pos_only_params().back().b_variadic || !pos_kwd_params().empty() && pos_kwd_params().back().b_variadic ||
               !kwd_only_params().empty() && kwd_only_params().back().b_variadic;
    }

    bool FunctionType::is_default() const {
        // pos_only parameters are never default
        return std::any_of(std::execution::par_unseq, pos_kwd_params().begin(), pos_kwd_params().end(),
                           [](const auto &param) { return param.b_default; }) ||
               std::any_of(std::execution::par_unseq, kwd_only_params().begin(), kwd_only_params().end(),
                           [](const auto &param) { return param.b_default; });
    }

    size_t FunctionType::min_param_count() const {
        std::atomic<size_t> result = m_pos_only_params.size();
        // Pos only parameters are never default or variadic
        // So they are counted as min_param_count
        std::for_each(std::execution::par_unseq, m_pos_kwd_params.begin(), m_pos_kwd_params.end(), [&](const ParamInfo &param) {
            if (!param.b_default && !param.b_variadic)
                result.fetch_add(1, std::memory_order_relaxed);
        });
        std::for_each(std::execution::par_unseq, m_kwd_only_params.begin(), m_kwd_only_params.end(), [&](const ParamInfo &param) {
            if (!param.b_default && !param.b_variadic)
                result.fetch_add(1, std::memory_order_relaxed);
        });
        return result;
    }

    size_t FunctionType::param_count() const {
        return m_pos_only_params.size() + m_pos_kwd_params.size() + m_kwd_only_params.size();
    }

    bool FunctionType::weak_equals(const FunctionType &other) const {
        return return_type() == other.return_type() && other.pos_only_params() == other.pos_only_params() &&
               other.pos_kwd_params() == other.pos_kwd_params() && other.kwd_only_params() == other.kwd_only_params();
    }

    bool FunctionType::operator==(const FunctionType &other) const {
        return b_nullable == other.b_nullable && return_type() == other.return_type() && other.pos_only_params() == other.pos_only_params() &&
               other.pos_kwd_params() == other.pos_kwd_params() && other.kwd_only_params() == other.kwd_only_params();
    }

    bool ParamInfo::operator==(const ParamInfo &other) const {
        return b_const == other.b_const &&          //
               b_variadic == other.b_variadic &&    //
               b_default == other.b_default &&      //
               b_kwd_only == other.b_kwd_only &&    //
               name == other.name &&                //
               type_info == other.type_info;
    }

    bool operator==(const FunctionType &fn_type, scope::Function *function) {
        if (fn_type.return_type() != function->get_ret_type() ||               //
            fn_type.pos_only_params() != function->get_pos_only_params() ||    //
            fn_type.pos_kwd_params() != function->get_pos_kwd_params() ||      //
            fn_type.kwd_only_params() != function->get_pos_only_params())
            return false;
        return true;
    }

    string BasicType::to_string(bool decorated) const {
        if (is_type_literal())
            return "type";
        string result = type->to_string(decorated);
        if (!type_args.empty()) {
            result += "[";
            result += std::accumulate(type_args.begin(), type_args.end(), string(), [decorated](const string &s, const TypeInfo &type_info) {
                return s + (s.empty() ? "" : ",") + type_info.to_string(decorated);
            });
            result += "]";
        }
        return result + (b_nullable ? "?" : "");
    }

    string FunctionType::to_string(bool decorated) const {
        string result = "(";
        result += params_string(m_pos_only_params, m_pos_kwd_params, m_kwd_only_params);
        result += ") -> ";
        result += m_ret_type->to_string(false);
        return b_nullable ? std::format("{}({})?", (decorated ? "function " : ""), result)
                          : std::format("{}{}", (decorated ? "function " : ""), result);
    }

    string TypeInfo::to_string(bool decorated) const {
        switch (tag) {
        case Kind::BASIC:
            return basic().to_string(decorated);
        case Kind::FUNCTION:
            return function().to_string(decorated);
        }
    }

    string ExprInfo::to_string(bool decorated) const {
        switch (tag) {
        case Kind::NORMAL:
        case Kind::STATIC:
            return type_info().to_string(decorated);
        case Kind::MODULE:
            return module()->to_string(decorated);
        case Kind::FUNCTION_SET:
            return functions().to_string(decorated);
        default:
            throw Unreachable();    // to remove MSVC warning
        }
    }

    string ParamInfo::to_string(bool decorated) const {
        return std::format("{}{}{}{}", (b_const ? "const " : ""), (b_variadic ? "*" : ""), (b_kwd_only ? name + ":" : ""),
                           type_info.to_string(decorated));
    }

    string params_string(const std::vector<ParamInfo> &pos_only_params, const std::vector<ParamInfo> &pos_kwd_params,
                         const std::vector<ParamInfo> &kwd_only_params) {
        string pos_only;
        for (size_t i = 0; i < pos_only_params.size(); i++) {
            pos_only += pos_only_params[i].to_string(false);
            if (i != pos_only_params.size() - 1)
                pos_only += ", ";
        }

        string pos_kwd;
        for (size_t i = 0; i < pos_kwd_params.size(); i++) {
            pos_kwd += pos_kwd_params[i].to_string(false);
            if (i != pos_kwd_params.size() - 1)
                pos_kwd += ", ";
        }

        string kwd_only;
        for (size_t i = 0; i < kwd_only_params.size(); i++) {
            kwd_only += kwd_only_params[i].to_string(false);
            if (i != kwd_only_params.size() - 1)
                kwd_only += ", ";
        }

        if (!pos_only.empty()) {
            if (!pos_kwd.empty()) {
                if (!kwd_only.empty())
                    return std::format("{}, *, {}, /, {}", pos_only, pos_kwd, kwd_only);
                else
                    return std::format("{}, *, {}", pos_only, pos_kwd);
            } else {
                if (!kwd_only.empty())
                    return std::format("{}, *, /, {}", pos_only, kwd_only);
                else
                    return std::format("{}, *,", pos_only);
            }
        } else {
            if (!pos_kwd.empty()) {
                if (!kwd_only.empty())
                    return std::format("{}, /, {}", pos_kwd, kwd_only);
                else
                    return std::format("{}", pos_kwd);
            } else {
                if (!kwd_only.empty())
                    return std::format("/, {}", kwd_only);
                else
                    return "";
            }
        }
    }

    string ArgumentInfo::to_string(bool decorated) const {
        return expr_info.to_string(decorated);
    }

    FunctionInfo::FunctionInfo(const scope::FunctionSet *fun_set) {
        if (fun_set) {
            for (const auto &[_, member]: fun_set->get_members()) {
                auto path = member.second->get_path();
                auto fun = cast<scope::Function>(&*member.second);
                functions[path] = fun;
            }
        }
    }

    FunctionInfo &FunctionInfo::operator=(const scope::FunctionSet *fun_set) {
        std::unordered_map<SymbolPath, scope::Function *> new_functions;
        if (fun_set) {
            for (const auto &[_, member]: fun_set->get_members()) {
                auto path = member.second->get_path();
                auto fun = cast<scope::Function>(&*member.second);
                new_functions[path] = fun;
            }
        }
        functions = new_functions;
        return *this;
    }

    void FunctionInfo::add(const SymbolPath &path, scope::Function *function, bool override) {
        if (override || !functions.contains(path))
            functions[path] = function;
    }

    void FunctionInfo::extend(const FunctionInfo &other, bool override) {
        for (const auto &[path, fun]: other.functions) {
            add(path, fun, override);
        }
    }

    std::unordered_map<SymbolPath, scope::FunctionSet *> FunctionInfo::get_function_sets() const {
        std::unordered_map<SymbolPath, scope::FunctionSet *> fun_sets;
        for (const auto &[path, fun]: functions) fun_sets[fun->get_parent()->get_path()] = cast<scope::FunctionSet>(fun->get_parent());
        return fun_sets;
    }

    string FunctionInfo::to_string(bool decorated) const {
        auto fun_sets = get_function_sets();
        string result;
        for (const auto &[_, fun_set]: fun_sets) {
            result += fun_set->to_string(decorated) + (b_nullable ? "?" : "") + ", ";
        }
        result.pop_back();
        result.pop_back();
        return result;
    }
}    // namespace spade