#pragma once

#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include <memory>

namespace spade::scope
{
    enum class ScopeType { FOLDER_MODULE, MODULE, COMPOUND, INIT, FUNCTION, BLOCK, VARIABLE, ENUMERATOR };

    class Scope {
      protected:
        /// the type of the scope
        ScopeType type;

        /// the ast node of the scope
        ast::AstNode *node;
        /// scopes that are members (can be referenced) e.g. variables, functions
        std::unordered_map<string, std::pair<std::shared_ptr<Token>, std::shared_ptr<Scope>>> members;

      public:
        Scope(ScopeType type, ast::AstNode *node) : type(type), node(node) {}

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

        void print(const string &name) const {
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
            std::cout << name << '\n';
            for (const auto &[member_name, member]: members) {
                member.second->print(name + '.' + member_name);
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
      public:
        Compound(ast::decl::Compound *node) : Scope(ScopeType::COMPOUND, node) {}

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
      public:
        Variable(ast::decl::Variable *node) : Scope(ScopeType::VARIABLE, node) {}

        bool is_const() const {
            return get_variable_node()->get_token()->get_type() == TokenType::CONST;
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