#include "scope.hpp"
#include "info.hpp"

namespace spadec::scope
{
    bool Scope::new_variable(const string &name, const std::shared_ptr<Token> &name_tok, const std::shared_ptr<Scope> &value) {
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

    Block *Scope::get_enclosing_block() const {
        if (parent == null)
            return null;
        if (parent->type == ScopeType::BLOCK)
            return cast<Block>(parent);
        return parent->get_enclosing_block();
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

    bool operator==(const Function &fun1, const Function &fun2) {
        return fun1.get_ret_type() == fun2.get_ret_type() &&                  //
               fun1.get_pos_only_params() == fun2.get_pos_only_params() &&    //
               fun1.get_pos_kwd_params() == fun2.get_pos_kwd_params() &&      //
               fun1.get_kwd_only_params() == fun2.get_kwd_only_params();
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
}    // namespace spadec::scope