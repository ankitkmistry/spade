#pragma once

#include <unordered_set>

#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "symbol_path.hpp"

namespace spade::scope
{
    enum class ScopeType { FOLDER_MODULE, MODULE, COMPOUND, INIT, FUNCTION, BLOCK, VARIABLE, ENUMERATOR };

    class Scope {
      public:
        using Member = std::pair<std::shared_ptr<Token>, std::shared_ptr<Scope>>;

      protected:
        /// the type of the scope
        ScopeType type;
        /// the path of the scope
        SymbolPath path;
        /// the ast node of the scope
        ast::AstNode *node;
        /// scopes that are members (can be referenced) e.g. variables, functions
        std::unordered_map<string, Member> members;

      public:
        Scope(ScopeType type, ast::AstNode *node, const SymbolPath &path = {}) : type(type), path(path), node(node) {}

        virtual ~Scope() = default;

        int get_line_start() const {
            return node->get_line_start();
        }

        int get_line_end() const {
            return node->get_line_end();
        }

        int get_col_start() const {
            return node->get_col_start();
        }

        int get_col_end() const {
            return node->get_col_end();
        }

        ScopeType get_type() const {
            return type;
        }

        const SymbolPath &get_path() const {
            return path;
        }

        void set_path(const SymbolPath &path) {
            this->path = path;
        }

        ast::AstNode *get_node() const {
            return node;
        }

        const std::unordered_map<string, std::pair<std::shared_ptr<Token>, std::shared_ptr<Scope>>> &get_members() const {
            return members;
        }

        bool new_variable(const string &name, const std::shared_ptr<Token> &name_tok, std::shared_ptr<Scope> value) {
            if (members.contains(name))
                return false;
            members[name] = {name_tok, value};
            return true;
        }

        bool new_variable(const std::shared_ptr<Token> &name_tok, std::shared_ptr<Scope> value) {
            auto name = name_tok->get_text();
            if (members.contains(name))
                return false;
            members[name] = {name_tok, value};
            return true;
        }

        bool del_variable(const string &name) {
            // if (members.contains(name))
            //     return false;
            // members.erase(name);
            // return true;
            return members.contains(name) ? (members.erase(name), true) : false;
        }

        std::shared_ptr<Scope> get_variable(const string &name) const {
            return members.contains(name) ? members.at(name).second : null;
        }

        bool has_variable(const string &name) const {
            return members.contains(name);
        }

        std::shared_ptr<Token> get_decl_site(const string &name) const {
            return members.contains(name) ? members.at(name).first : null;
        }

        void print() const {
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
            for (const auto &[_,member]: members) {
                member.second->print();
            }
        }
    };

    class FolderModule : public Scope {
      public:
        FolderModule(ast::FolderModule *node) : Scope(ScopeType::FOLDER_MODULE, node) {}

        ast::FolderModule *get_module_node() const {
            return cast<ast::FolderModule>(node);
        }
    };

    class Module : public Scope {
      public:
        Module(ast::Module *node) : Scope(ScopeType::MODULE, node) {}

        ast::Module *get_module_node() const {
            return cast<ast::Module>(node);
        }
    };

    class Compound : public Scope {
        string name;
        std::unordered_set<std::shared_ptr<Compound>> parents;

      public:
        Compound(string name) : Scope(ScopeType::COMPOUND, null), name(name) {}

        Compound(ast::decl::Compound *node) : Scope(ScopeType::COMPOUND, node), name(node->get_name()->get_text()) {}

        void inherit_from(const std::shared_ptr<Compound> &parent) {
            for (const auto &[name, member]: parent->members) {
                new_variable(name, member.first, member.second);
            }
            parents.insert(parent);
        }

        const string &get_name() const {
            return name;
        }

        bool has_parent(const std::shared_ptr<Compound> &parent) const {
            if (parents.contains(parent))
                return true;
            else {
                for (const auto &p: parents) {
                    if (p->has_parent(parent))
                        return true;
                }
                return false;
            }
        }

        const std::unordered_set<std::shared_ptr<Compound>> &get_parents() const {
            return parents;
        }

        ast::decl::Compound *get_compound_node() const {
            return cast<ast::decl::Compound>(node);
        }
    };

    class Init : public Scope {
      public:
        Init(ast::decl::Init *node) : Scope(ScopeType::INIT, node) {}

        ast::decl::Init *get_init_node() const {
            return cast<ast::decl::Init>(node);
        }
    };

    class Function : public Scope {
      public:
        Function(ast::decl::Function *node) : Scope(ScopeType::FUNCTION, node) {}

        ast::decl::Function *get_function_node() const {
            return cast<ast::decl::Function>(node);
        }
    };

    class Block : public Scope {
      public:
        Block(ast::stmt::Block *node) : Scope(ScopeType::BLOCK, node) {}

        ast::stmt::Block *get_block_node() const {
            return cast<ast::stmt::Block>(node);
        }
    };

    class Variable : public Scope {
        TypeInfo type_info;

      public:
        Variable(ast::decl::Variable *node) : Scope(ScopeType::VARIABLE, node) {}

        bool is_const() const {
            return get_variable_node()->get_token()->get_type() == TokenType::CONST;
        }

        const TypeInfo &get_type_info() const {
            return type_info;
        }

        void set_type_info(const TypeInfo &type_info) {
            this->type_info = type_info;
        }

        ast::decl::Variable *get_variable_node() const {
            return cast<ast::decl::Variable>(node);
        }
    };

    class Enumerator : public Scope {
      public:
        Enumerator(ast::decl::Enumerator *node) : Scope(ScopeType::ENUMERATOR, node) {}

        ast::decl::Enumerator *get_enumerator_node() const {
            return cast<ast::decl::Enumerator>(node);
        }
    };
}    // namespace spade::scope