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
                return functions.to_string(decorated);
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
        for (const auto &[path, fun]: functions)
            fun_sets[fun->get_parent()->get_path()] = cast<scope::FunctionSet>(fun->get_parent());
        return fun_sets;
    }

    string FunctionInfo::to_string(bool decorated) const {
        auto fun_sets = get_function_sets();
        string result;
        for (const auto &[_, fun_set]: fun_sets) {
            result += fun_set->to_string(decorated) + ", ";
        }
        result.pop_back();
        result.pop_back();
        return result;
    }
}    // namespace spade