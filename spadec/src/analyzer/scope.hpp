#pragma once

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <execution>
#include <unordered_set>
#include <utility>

#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
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
            return get_decl_site() ? get_decl_site()->get_line_start() : node ? node->get_line_start() : -1;
        }

        int get_line_end() const {
            return get_decl_site() ? get_decl_site()->get_line_end() : node ? node->get_line_end() : -1;
        }

        int get_col_start() const {
            return get_decl_site() ? get_decl_site()->get_col_start() : node ? node->get_col_start() : -1;
        }

        int get_col_end() const {
            return get_decl_site() ? get_decl_site()->get_col_end() : node ? node->get_col_end() : -1;
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

        void set_members(const std::unordered_map<string, Member> &members) {
            this->members = members;
        }

        std::unordered_map<string, Member> &get_members() {
            return members;
        }

        const std::unordered_map<string, Member> &get_members() const {
            return members;
        }

        std::shared_ptr<Token> get_decl_site() const {
            return parent ? parent->get_decl_site(path.get_name()) : null;
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

        virtual string to_string(bool decorated = true) const = 0;

        void print() const;
    };

    class FolderModule final : public Scope {
      public:
        FolderModule() : Scope(ScopeType::FOLDER_MODULE, null) {}

        string to_string(bool decorated = true) const override {
            return decorated ? "module " + path.to_string() : path.to_string();
        }
    };

    class Module final : public Scope {
        std::shared_ptr<ast::Module> module_sptr;
        std::unordered_map<string, std::shared_ptr<scope::Scope>> imports;
        std::vector<std::shared_ptr<scope::Scope>> open_imports;

      public:
        Module(ast::Module *node) : Scope(ScopeType::MODULE, node) {}

        void claim(const std::shared_ptr<ast::Module> &ptr) {
            module_sptr = ptr;
        }

        bool new_import(const string &name, std::shared_ptr<scope::Scope> node) {
            return imports.contains(name) ? false : (imports[name] = node, true);
        }

        std::shared_ptr<scope::Scope> get_import(const string &name) const {
            return imports.contains(name) ? imports.at(name) : null;
        }

        bool has_import(const string &name) const {
            return imports.contains(name);
        }

        void new_open_import(std::shared_ptr<scope::Scope> node) {
            open_imports.push_back(node);
        }

        const std::vector<std::shared_ptr<scope::Scope>> &get_open_imports() const {
            return open_imports;
        }

        ast::Module *get_module_node() const {
            return cast<ast::Module>(node);
        }

        string to_string(bool decorated = true) const override {
            return decorated ? "module " + path.to_string() : path.to_string();
        }
    };

    class ModifierMixin {
      protected:
        /**
         * @brief a bitset of modifiers
         * 
         * 0 -> abstract
         * 1 -> final
         * 2 -> static
         * 3 -> override
         * 4 -> private
         * 5 -> internal
         * 6 -> module private
         * 7 -> protected
         * 8 -> public
         */
        std::bitset<9> modifiers;

        ModifierMixin() = default;

        ModifierMixin(const std::vector<std::shared_ptr<Token>> &mod_toks) {
            for (const auto &mod_tok: mod_toks) {
                switch (mod_tok->get_type()) {
                case TokenType::ABSTRACT:
                    modifiers[0] = true;    // abstract
                    break;
                case TokenType::FINAL:
                    modifiers[1] = true;    // final
                    break;
                case TokenType::STATIC:
                    modifiers[2] = true;    // static
                    break;
                case TokenType::OVERRIDE:
                    modifiers[3] = true;    // override
                    break;
                case TokenType::PRIVATE:
                    modifiers[4] = true;    // private
                    break;
                case TokenType::INTERNAL:
                    modifiers[5] = true;    // internal
                    break;
                case TokenType::PROTECTED:
                    modifiers[7] = true;    // protected
                    break;
                case TokenType::PUBLIC:
                    modifiers[8] = true;    // public
                    break;
                default:
                    break;
                }
            }
            if (!modifiers[4] && !modifiers[5] && !modifiers[7] && !modifiers[8])
                modifiers[6] = true;    // module private
        }

      public:
        virtual ~ModifierMixin() = default;

        bool is_abstract() const {
            return modifiers[0];
        }

        bool is_final() const {
            return modifiers[1];
        }

        bool is_static() const {
            return modifiers[2];
        }

        bool is_override() const {
            return modifiers[3];
        }

        bool is_private() const {
            return modifiers[4];
        }

        bool is_internal() const {
            return modifiers[5];
        }

        bool is_module_private() const {
            return modifiers[6];
        }

        bool is_protected() const {
            return modifiers[7];
        }

        bool is_public() const {
            return modifiers[8];
        }

        void set_abstract(bool value) {
            modifiers[0] = value;
        }

        void set_final(bool value) {
            modifiers[1] = value;
        }

        void set_static(bool value) {
            modifiers[2] = value;
        }

        void set_override(bool value) {
            modifiers[3] = value;
        }

        void set_private(bool value) {
            modifiers[4] = value;
        }

        void set_internal(bool value) {
            modifiers[5] = value;
        }

        void set_module_private(bool value) {
            modifiers[6] = value;
        }

        void set_protected(bool value) {
            modifiers[7] = value;
        }

        void set_public(bool value) {
            modifiers[8] = value;
        }
    };

    class Variable;

    class Compound final : public Scope, public ModifierMixin {
      public:
        enum class Eval { NOT_STARTED, PROGRESS, DONE };

      private:
        string name;
        std::unordered_set<Compound *> supers;
        std::unordered_map<string, std::shared_ptr<Variable>> super_fields;
        std::unordered_map<string, FunctionInfo> super_functions;
        Eval eval = Eval::NOT_STARTED;

      public:
        Compound(string name) : Scope(ScopeType::COMPOUND, null), name(name) {}

        Compound(ast::decl::Compound *node)
            : Scope(ScopeType::COMPOUND, node), ModifierMixin(node->get_modifiers()), name(node->get_name()->get_text()) {}

        const string &get_name() const {
            return name;
        }

        Eval get_eval() const {
            return eval;
        }

        void set_eval(Eval value) {
            eval = value;
        }

        void inherit_from(Compound *super) {
            supers.insert(super);
        }

        bool has_super(Compound *super) const;

        const std::unordered_map<string, std::shared_ptr<Variable>> &get_super_fields() const {
            return super_fields;
        }

        void set_super_fields(const std::unordered_map<string, std::shared_ptr<Variable>> &fields) {
            super_fields = fields;
        }

        const std::unordered_map<string, FunctionInfo> &get_super_functions() const {
            return super_functions;
        }

        std::unordered_map<string, FunctionInfo> &get_super_functions() {
            return super_functions;
        }

        void set_super_functions(const std::unordered_map<string, FunctionInfo> &functions) {
            super_functions = functions;
        }

        const std::unordered_set<Compound *> &get_supers() const {
            return supers;
        }

        ast::decl::Compound *get_compound_node() const {
            return cast<ast::decl::Compound>(node);
        }

        string to_string(bool decorated = true) const override {
            if (!decorated)
                return path.to_string();
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

    class Function final : public Scope, public ModifierMixin {
      public:
        enum class ProtoEval { NOT_STARTED, PROGRESS, DONE };

      private:
        /// Flag if the function prototype is being evaluated
        ProtoEval proto_eval = ProtoEval::NOT_STARTED;
        std::vector<ParamInfo> pos_only_params;
        std::vector<ParamInfo> pos_kwd_params;
        std::vector<ParamInfo> kwd_only_params;
        TypeInfo ret_type;

      public:
        Function(ast::decl::Function *node)
            : Scope(ScopeType::FUNCTION, node), ModifierMixin(node ? node->get_modifiers() : std::vector<std::shared_ptr<Token>>()) {}

        ast::decl::Function *get_function_node() const {
            return cast<ast::decl::Function>(node);
        }

        bool is_init() const {
            return get_function_node()->get_name()->get_type() == TokenType::INIT;
        }

        bool has_param(const string &name) const {
            auto it = std::find_if(pos_only_params.begin(), pos_only_params.end(), [&name](const ParamInfo &param) { return param.name == name; });
            if (it != pos_only_params.end())
                return true;

            it = std::find_if(pos_kwd_params.begin(), pos_kwd_params.end(), [&name](const ParamInfo &param) { return param.name == name; });
            if (it != pos_kwd_params.end())
                return true;

            it = std::find_if(kwd_only_params.begin(), kwd_only_params.end(), [&name](const ParamInfo &param) { return param.name == name; });
            return it != kwd_only_params.end();
        }

        ParamInfo get_param(const string &name) const {
            auto it = std::find_if(pos_only_params.begin(), pos_only_params.end(), [&name](const ParamInfo &param) { return param.name == name; });
            if (it != pos_only_params.end())
                return *it;

            it = std::find_if(pos_kwd_params.begin(), pos_kwd_params.end(), [&name](const ParamInfo &param) { return param.name == name; });
            if (it != pos_kwd_params.end())
                return *it;

            it = std::find_if(kwd_only_params.begin(), kwd_only_params.end(), [&name](const ParamInfo &param) { return param.name == name; });
            if (it != kwd_only_params.end())
                return *it;
            throw Unreachable();    // surely some parser error
        }

        bool is_variadic() const {
            return !pos_only_params.empty() && pos_only_params.back().b_variadic || !pos_kwd_params.empty() && pos_kwd_params.back().b_variadic ||
                   !kwd_only_params.empty() && kwd_only_params.back().b_variadic;
        }

        bool is_default() const {
            // pos_only parameters are never default
            return std::any_of(std::execution::par_unseq, pos_kwd_params.begin(), pos_kwd_params.end(),
                               [](const auto &param) { return param.b_default; }) ||
                   std::any_of(std::execution::par_unseq, kwd_only_params.begin(), kwd_only_params.end(),
                               [](const auto &param) { return param.b_default; });
        }

        size_t min_param_count() const {
            std::atomic<size_t> result = pos_only_params.size();
            // Pos only parameters are never default or variadic
            // So they are counted as min_param_count
            std::for_each(std::execution::par_unseq, pos_kwd_params.begin(), pos_kwd_params.end(), [&](const ParamInfo &param) {
                if (!param.b_default && !param.b_variadic)
                    result.fetch_add(1, std::memory_order_relaxed);
            });
            std::for_each(std::execution::par_unseq, kwd_only_params.begin(), kwd_only_params.end(), [&](const ParamInfo &param) {
                if (!param.b_default && !param.b_variadic)
                    result.fetch_add(1, std::memory_order_relaxed);
            });
            return result;
        }

        size_t param_count() const {
            return pos_only_params.size() + pos_kwd_params.size() + kwd_only_params.size();
        }

        const std::vector<ParamInfo> &get_pos_only_params() const {
            return pos_only_params;
        }

        const std::vector<ParamInfo> &get_pos_kwd_params() const {
            return pos_kwd_params;
        }

        const std::vector<ParamInfo> &get_kwd_only_params() const {
            return kwd_only_params;
        }

        void set_pos_only_params(const std::vector<ParamInfo> &params) {
            pos_only_params = params;
        }

        void set_pos_kwd_params(const std::vector<ParamInfo> &params) {
            pos_kwd_params = params;
        }

        void set_kwd_only_params(const std::vector<ParamInfo> &params) {
            kwd_only_params = params;
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

        string to_string(bool decorated = true) const override;
    };

    bool operator==(const Function &fun1, const Function &fun2);

    inline bool operator!=(const Function &fun1, const Function &fun2) {
        return !(fun1 == fun2);
    }

    class FunctionSet final : public Scope {
        bool redecl_check = false;

      public:
        FunctionSet() : Scope(ScopeType::FUNCTION_SET, null) {}

        FunctionSet(const FunctionSet &) = default;
        FunctionSet(FunctionSet &&) = default;
        FunctionSet &operator=(const FunctionSet &) = default;
        FunctionSet &operator=(FunctionSet &&) = default;

        bool is_init() const {
            return !members.empty() && cast<Function>(members.begin()->second.second)->is_init();
        }

        bool is_redecl_check() const {
            return redecl_check;
        }

        void set_redecl_check(bool value) {
            redecl_check = value;
        }

        string to_string(bool decorated = true) const override {
            return (decorated ? (is_init() ? "ctor " : "function ") : "") + path.to_string();
        }
    };

    class Block final : public Scope {
      public:
        Block(ast::stmt::Block *node) : Scope(ScopeType::BLOCK, node) {}

        ast::stmt::Block *get_block_node() const {
            return cast<ast::stmt::Block>(node);
        }

        string to_string(bool decorated = true) const override {
            return "block";
        }
    };

    class Variable final : public Scope, public ModifierMixin {
      public:
        enum class Eval { NOT_STARTED, PROGRESS, DONE };

      private:
        /// flag if the current scope is being evaluated
        Eval eval = Eval::NOT_STARTED;
        /// type info of the variable
        TypeInfo type_info;

      public:
        Variable(ast::decl::Variable *node)
            : Scope(ScopeType::VARIABLE, node), ModifierMixin(node ? node->get_modifiers() : std::vector<std::shared_ptr<Token>>{}) {}

        bool is_const() const {
            return get_variable_node()->get_token()->get_type() == TokenType::CONST;
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

        string to_string(bool decorated = true) const override {
            if (!decorated)
                return path.to_string();
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

        string to_string(bool decorated = true) const override {
            return decorated ? path.to_string() : "enumerator " + path.to_string();
        }
    };
}    // namespace spade::scope