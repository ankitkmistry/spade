#include "scope.hpp"
#include "info.hpp"

namespace spade::scope
{
    bool Scope::new_variable(const string &name, const std::shared_ptr<Token> &name_tok, std::shared_ptr<Scope> value) {
        if (members.contains(name))
            return false;
        members[name] = {name_tok, value};
        value->parent = this;
        return true;
    }

    bool Scope::del_variable(const string &name) {
        if (members.contains(name))
            return false;
        members[name].second->parent = null;
        members.erase(name);
        return true;
    }

    Module *Scope::get_enclosing_module() const {
        if (parent == null)
            return null;
        if (parent->type == ScopeType::MODULE)
            return cast<Module>(parent);
        return parent->get_enclosing_module();
    }

    Compound *Scope::get_enclosing_compound() const {
        if (parent == null)
            return null;
        if (parent->type == ScopeType::COMPOUND)
            return cast<Compound>(parent);
        return parent->get_enclosing_compound();
    }

    Function *Scope::get_enclosing_function() const {
        if (parent == null)
            return null;
        if (parent->type == ScopeType::FUNCTION)
            return cast<Function>(parent);
        return parent->get_enclosing_function();
    }

    void Scope::print() const {
        std::cout << to_string() << std::endl;
        for (const auto &[_, member]: members) {
            member.second->print();
        }
    }

    bool Function::has_same_params_as(const Function &other) {
        // treat normal parameters as positional parameters
        if (pos_only_params.size() + pos_kwd_params.size() != other.pos_only_params.size() + other.pos_kwd_params.size())
            return false;
        for (size_t i = 0; i < pos_only_params.size(); i++) {
            if (pos_only_params[i] != other.pos_only_params[i])
                return false;
        }
        for (size_t i = 0; i < pos_kwd_params.size(); i++) {
            if (pos_kwd_params[i] != other.pos_kwd_params[i])
                return false;
        }
        // check for keyword only parameters
        std::unordered_map<string, ParamInfo> kwd_only_other_params_map;
        for (const auto &param: other.kwd_only_params) {
            kwd_only_other_params_map[param.name] = param;
        }
        for (size_t i = 0; i < kwd_only_params.size(); i++) {
            auto param = kwd_only_params[i];
            if (kwd_only_other_params_map.contains(param.name)) {
                auto other_param = kwd_only_other_params_map[param.name];
                if (param != other_param)
                    return false;
            } else
                return false;
        }
        return true;
    }

    string Function::to_string(bool decorated) const {
        string result = parent->get_path().to_string();
        // TODO: add support for type arguments
        if (node) {
            result += "(";
            for (size_t i = 0; i < pos_only_params.size(); i++) {
                result += pos_only_params[i].to_string(false);
                if (i != pos_only_params.size() - 1)
                    result += ",";
            }
            for (size_t i = 0; i < pos_kwd_params.size(); i++) {
                result += pos_kwd_params[i].to_string(false);
                if (i != pos_kwd_params.size() - 1)
                    result += ",";
            }
            for (size_t i = 0; i < kwd_only_params.size(); i++) {
                result += kwd_only_params[i].to_string(false);
                if (i != kwd_only_params.size() - 1)
                    result += ",";
            }
            if (result.back() == ',')
                result.pop_back();
            result += ")";
        }
        return (decorated ? (is_init() ? "init " : "function ") : "") + result;
    }
}    // namespace spade::scope