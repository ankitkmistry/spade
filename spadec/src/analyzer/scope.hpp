#pragma once

#include <algorithm>
#include <unordered_set>

#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "spimp/error.hpp"
#include "symbol_path.hpp"

namespace spade::scope
{
    class Module;
    class Compound;
    class Function;

    enum class ScopeType { FOLDER_MODULE, MODULE, COMPOUND, FUNCTION, FUNCTION_SET, BLOCK, VARIABLE, ENUMERATOR };

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
        /// parent of the scope
        Scope *parent = null;
        /// scopes that are members (can be referenced) e.g. variables, functions
        std::unordered_map<string, Member> members;

      public:
        Scope(ScopeType type, ast::AstNode *node, const SymbolPath &path = {}) : type(type), node(node), path(path) {}

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

        Scope *get_parent() const {
            return parent;
        }

        const std::unordered_map<string, Member> &get_members() const {
            return members;
        }

        bool new_variable(const string &name, const std::shared_ptr<Token> &name_tok, std::shared_ptr<Scope> value);

        bool new_variable(const std::shared_ptr<Token> &name_tok, std::shared_ptr<Scope> value) {
            return new_variable(name_tok->get_text(), name_tok, value);
        }

        bool del_variable(const string &name);

        std::shared_ptr<Scope> get_variable(const string &name) const {
            return members.contains(name) ? members.at(name).second : null;
        }

        bool has_variable(const string &name) const {
            return members.contains(name);
        }

        std::shared_ptr<Token> get_decl_site(const string &name) const {
            return members.contains(name) ? members.at(name).first : null;
        }

        Module *get_enclosing_module() const;
        Compound *get_enclosing_compound() const;
        Function *get_enclosing_function() const;

        virtual string to_string() const = 0;

        void print() const;
    };

    class FolderModule final : public Scope {
      public:
        FolderModule(ast::FolderModule *node) : Scope(ScopeType::FOLDER_MODULE, node) {}

        ast::FolderModule *get_module_node() const {
            return cast<ast::FolderModule>(node);
        }

        string to_string() const override {
            return "module " + path.to_string();
        }
    };

    class Module final : public Scope {
      public:
        Module(ast::Module *node) : Scope(ScopeType::MODULE, node) {}

        ast::Module *get_module_node() const {
            return cast<ast::Module>(node);
        }

        string to_string() const override {
            return "module " + path.to_string();
        }
    };

    class Compound final : public Scope {
        string name;
        std::unordered_set<Compound *> supers;

      public:
        Compound(string name) : Scope(ScopeType::COMPOUND, null), name(name) {}

        Compound(ast::decl::Compound *node) : Scope(ScopeType::COMPOUND, node), name(node->get_name()->get_text()) {}

        void inherit_from(const std::shared_ptr<Compound> &super) {
            for (const auto &[name, member]: super->members) {
                new_variable(name, member.first, member.second);
            }
            supers.insert(&*super);
        }

        void inherit_from(Compound *super) {
            for (const auto &[name, member]: super->members) {
                new_variable(name, member.first, member.second);
            }
            supers.insert(super);
        }

        const string &get_name() const {
            return name;
        }

        bool has_super(const std::shared_ptr<Compound> &super) const {
            if (supers.contains(&*super))
                return true;
            else {
                for (const auto &p: supers) {
                    if (p->has_super(super))
                        return true;
                }
                return false;
            }
        }

        bool has_super(Compound *super) const {
            if (supers.contains(super))
                return true;
            for (const auto &p: supers) {
                if (p->has_super(super))
                    return true;
            }
            return false;
        }

        const std::unordered_set<Compound *> &get_supers() const {
            return supers;
        }

        ast::decl::Compound *get_compound_node() const {
            return cast<ast::decl::Compound>(node);
        }

        string to_string() const override {
            if (!node)
                return "class " + path.to_string();
            switch (get_compound_node()->get_token()->get_type()) {
                case TokenType::CLASS:
                    return "class " + path.to_string();
                case TokenType::INTERFACE:
                    return "interface " + path.to_string();
                case TokenType::ENUM:
                    return "enum " + path.to_string();
                case TokenType::ANNOTATION:
                    return "annotation " + path.to_string();
                default:
                    throw Unreachable();    // surely some parser error
            }
        }
    };

    class Function final : public Scope {
      public:
        enum class ProtoEval { NOT_STARTED, PROGRESS, DONE };

      private:
        /// Flag if the function prototype is being evaluated
        ProtoEval proto_eval = ProtoEval::NOT_STARTED;
        std::vector<TypeInfo> pos_only_param_types;
        std::vector<TypeInfo> pos_kwd_param_types;
        std::vector<TypeInfo> kwd_only_param_types;
        TypeInfo ret_type;

      public:
        Function(ast::decl::Function *node) : Scope(ScopeType::FUNCTION, node) {}

        ast::decl::Function *get_function_node() const {
            return cast<ast::decl::Function>(node);
        }

        bool is_init() const {
            return get_function_node()->get_name()->get_type() == TokenType::INIT;
        }

        bool is_abstract() const {
            const auto &modifiers = get_function_node()->get_modifiers();
            return std::find_if(modifiers.begin(), modifiers.end(), [](const std::shared_ptr<Token> &modifier) {
                       return modifier->get_type() == TokenType::ABSTRACT;
                   }) != modifiers.end();
        }

        bool is_final() const {
            const auto &modifiers = get_function_node()->get_modifiers();
            return std::find_if(modifiers.begin(), modifiers.end(), [](const std::shared_ptr<Token> &modifier) {
                       return modifier->get_type() == TokenType::FINAL;
                   }) != modifiers.end();
        }

        bool is_override() const {
            const auto &modifiers = get_function_node()->get_modifiers();
            return std::find_if(modifiers.begin(), modifiers.end(), [](const std::shared_ptr<Token> &modifier) {
                       return modifier->get_type() == TokenType::OVERRIDE;
                   }) != modifiers.end();
        }

        bool is_static() const {
            const auto &modifiers = get_function_node()->get_modifiers();
            return std::find_if(modifiers.begin(), modifiers.end(), [](const std::shared_ptr<Token> &modifier) {
                       return modifier->get_type() == TokenType::STATIC;
                   }) != modifiers.end();
        }

        size_t param_count() const {
            return pos_only_param_types.size() + pos_kwd_param_types.size() + kwd_only_param_types.size();
        }

        const std::vector<TypeInfo> &get_pos_only_param_types() const {
            return pos_only_param_types;
        }

        const std::vector<TypeInfo> &get_pos_kwd_param_types() const {
            return pos_kwd_param_types;
        }

        const std::vector<TypeInfo> &get_kwd_only_param_types() const {
            return kwd_only_param_types;
        }

        const TypeInfo &get_ret_type() const {
            return ret_type;
        }

        void set_ret_type(const TypeInfo &type) {
            ret_type = type;
        }

        ProtoEval get_proto_eval() const {
            return proto_eval;
        }

        void set_proto_eval(ProtoEval eval) {
            proto_eval = eval;
        }

        string to_string() const override {
            return (is_init() ? "init " : "function ") + path.to_string();
        }
    };

    class FunctionSet final : public Scope {
        bool redecl_check = false;

      public:
        FunctionSet() : Scope(ScopeType::FUNCTION_SET, null) {}

        FunctionSet(const FunctionSet &) = default;
        FunctionSet(FunctionSet &&) = default;
        FunctionSet &operator=(const FunctionSet &) = default;
        FunctionSet &operator=(FunctionSet &&) = default;

        bool is_redecl_check() const {
            return redecl_check;
        }

        void set_redecl_check(bool value) {
            redecl_check = value;
        }

        string to_string() const override {
            return "function set " + path.to_string();
        }
    };

    class Block final : public Scope {
      public:
        Block(ast::stmt::Block *node) : Scope(ScopeType::BLOCK, node) {}

        ast::stmt::Block *get_block_node() const {
            return cast<ast::stmt::Block>(node);
        }

        string to_string() const override {
            return "block";
        }
    };

    class Variable final : public Scope {
      public:
        enum class Eval { NOT_STARTED, PROGRESS, DONE };

      private:
        /// flag if the current scope is being evaluated
        Eval eval = Eval::NOT_STARTED;
        /// type info of the variable
        TypeInfo type_info;

      public:
        Variable(ast::decl::Variable *node) : Scope(ScopeType::VARIABLE, node) {}

        bool is_const() const {
            return get_variable_node()->get_token()->get_type() == TokenType::CONST;
        }

        bool is_static() const {
            const auto &modifiers = get_variable_node()->get_modifiers();
            return std::find_if(modifiers.begin(), modifiers.end(), [](const std::shared_ptr<Token> &modifier) {
                       return modifier->get_type() == TokenType::STATIC;
                   }) != modifiers.end();
        }

        const TypeInfo &get_type_info() const {
            return type_info;
        }

        void set_type_info(const TypeInfo &type_info) {
            this->type_info = type_info;
        }

        Eval get_eval() const {
            return eval;
        }

        void set_eval(Eval eval) {
            this->eval = eval;
        }

        ast::decl::Variable *get_variable_node() const {
            return cast<ast::decl::Variable>(node);
        }

        string to_string() const override {
            switch (get_variable_node()->get_token()->get_type()) {
                case TokenType::VAR:
                    if (get_enclosing_compound())
                        return "field " + path.to_string();
                    return "var " + path.to_string();
                case TokenType::CONST:
                    if (get_enclosing_compound())
                        return "const field " + path.to_string();
                    return "const " + path.to_string();
                default:
                    throw Unreachable();
            }
        }
    };

    class Enumerator final : public Scope {
      public:
        Enumerator(ast::decl::Enumerator *node) : Scope(ScopeType::ENUMERATOR, node) {}

        ast::decl::Enumerator *get_enumerator_node() const {
            return cast<ast::decl::Enumerator>(node);
        }

        string to_string() const override {
            return "enumerator " + path.to_string();
        }
    };
}    // namespace spade::scope