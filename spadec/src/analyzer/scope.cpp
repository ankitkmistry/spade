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

    bool Compound::has_super(Compound *super) const {
        if (supers.contains(super))
            return true;
        for (const auto &p: supers) {
            if (p->has_super(super))
                return true;
        }
        return false;
    }

    string Function::to_string(bool decorated) const {
        string result = parent->get_path().to_string();
        // TODO: add support for type arguments
        if (node) {
            result += "(";
            result += params_string(pos_only_params, pos_kwd_params, kwd_only_params);
            result += ")";
        }
        return (decorated ? (is_init() ? "ctor " : "function ") : "") + result;
    }
}    // namespace spade::scope