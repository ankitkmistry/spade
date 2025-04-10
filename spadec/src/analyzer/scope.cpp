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

    string Function::to_string(bool decorated) const {
        string result = parent->get_path().to_string();
        // TODO: add support for type arguments
        if (node) {
            result += "(";
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
            if (!pos_only.empty())
                if (!pos_kwd.empty())
                    if (!kwd_only.empty())
                        result += std::format("{}, *, {}, /, {}", pos_only, pos_kwd, kwd_only);
                    else
                        result += std::format("{}, *, {}", pos_only, pos_kwd);
                else if (!kwd_only.empty())
                    result += std::format("{}, *, /, {}", pos_only, kwd_only);
                else
                    result += std::format("{}, *", pos_only);
            else if (!pos_kwd.empty())
                if (!kwd_only.empty())
                    result += std::format("{}, /, {}", pos_kwd, kwd_only);
                else
                    result += std::format("{}", pos_kwd);
            else if (!kwd_only.empty())
                result += std::format("/, {}", kwd_only);
            else
                result += "";
            result += ")";
        }
        return (decorated ? (is_init() ? "init " : "function ") : "") + result;
    }
}    // namespace spade::scope