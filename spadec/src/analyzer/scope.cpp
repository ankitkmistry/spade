#include "scope.hpp"

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
        switch (type) {
            case ScopeType::FOLDER_MODULE:
                std::cout << "[FOLDER_MODULE] ";
                break;
            case ScopeType::MODULE:
                std::cout << "[MODULE] ";
                break;
            case ScopeType::COMPOUND:
                std::cout << "[COMPOUND] ";
                break;
            case ScopeType::INIT:
                std::cout << "[INIT] ";
                break;
            case ScopeType::FUNCTION:
                std::cout << "[FUNCTION] ";
                break;
            case ScopeType::BLOCK:
                std::cout << "[BLOCK] ";
                break;
            case ScopeType::VARIABLE:
                std::cout << "[VARIABLE] ";
                break;
            case ScopeType::ENUMERATOR:
                std::cout << "[ENUMERATOR] ";
                break;
        }
        std::cout << path << std::endl;
        for (const auto &[_, member]: members) {
            member.second->print();
        }
    }
}    // namespace spade::scope