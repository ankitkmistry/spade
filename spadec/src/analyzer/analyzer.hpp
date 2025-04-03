#pragma once

#include "utils/error.hpp"
#include "utils/error_printer.hpp"
#include "parser/ast.hpp"
#include "info.hpp"
#include "scope.hpp"

namespace spade
{
    class Analyzer final : public ast::VisitorBase {
        // Internal modules
        enum class Internal { SPADE, SPADE_ANY, SPADE_INT, SPADE_FLOAT, SPADE_BOOL, SPADE_STRING, SPADE_VOID };
        std::unordered_map<Internal, std::shared_ptr<scope::Scope>> internals;
        void load_internal_modules();

        std::unordered_map<ast::Module *, ScopeInfo> module_scopes;
        // std::vector<std::shared_ptr<scope::Scope>> scope_stack;

        scope::Scope *cur_scope = null;
        scope::Scope *get_parent_scope() const;
        scope::Scope *get_current_scope() const;

        /// Performs name resolution
        std::shared_ptr<scope::Scope> find_name(const string &name) const;
        /// Performs context resolution
        void resolve_context(const std::shared_ptr<scope::Scope> &scope, const ast::AstNode &node);
        /// Performs cast checking
        void check_cast(scope::Compound *from, scope::Compound *to, const ast::AstNode &node, bool safe);
        /// Performs variable type inference resolution
        ExprInfo get_var_expr_info(std::shared_ptr<scope::Variable> var_scope, const ast::AstNode &node);
        // ExprInfo get_fun_ret_info(std::shared_ptr<scope::Function> fun_scope, const ast::AstNode &node);

        ErrorPrinter printer;

        template<ast::HasLineInfo T>
        AnalyzerError error(const string &msg, T node) {
            return AnalyzerError(msg, get_current_scope()->get_enclosing_module()->get_module_node()->get_file_path(), node);
        }

        template<ast::HasLineInfo T>
        void warning(const string &msg, T node) {
            printer.print(ErrorType::WARNING, error(msg, node));
        }

        template<ast::HasLineInfo T>
        void note(const string &msg, T node) {
            printer.print(ErrorType::NOTE, error(msg, node));
        }

      public:
        explicit Analyzer(ErrorPrinter printer) : printer(printer) {}

        void analyze(const std::vector<std::shared_ptr<ast::Module>> &modules);

      private:
        std::shared_ptr<scope::Scope> _res_reference;

      public:
        // Visitor
        void visit(ast::Reference &node);

      private:
        TypeInfo _res_type_info;

      public:
        // Type visitor
        void visit(ast::type::Reference &node);
        void visit(ast::type::Function &node);
        void visit(ast::type::TypeLiteral &node);
        void visit(ast::type::BinaryOp &node);
        void visit(ast::type::Nullable &node);
        void visit(ast::type::TypeBuilder &node);
        void visit(ast::type::TypeBuilderMember &node);
        // Expression visitor
      private:
        ExprInfo _res_expr_info;

      public:
        void visit(ast::expr::Constant &node);
        void visit(ast::expr::Super &node);
        void visit(ast::expr::Self &node);
        void visit(ast::expr::DotAccess &node);
        void visit(ast::expr::Call &node);
        void visit(ast::expr::Argument &node);
        void visit(ast::expr::Reify &node);
        void visit(ast::expr::Index &node);
        void visit(ast::expr::Slice &node);
        void visit(ast::expr::Unary &node);
        void visit(ast::expr::Cast &node);
        void visit(ast::expr::Binary &node);
        void visit(ast::expr::ChainBinary &node);
        void visit(ast::expr::Ternary &node);
        void visit(ast::expr::Assignment &node);
        // Statement visitor
        void visit(ast::stmt::Block &node);
        void visit(ast::stmt::If &node);
        void visit(ast::stmt::While &node);
        void visit(ast::stmt::DoWhile &node);
        void visit(ast::stmt::Throw &node);
        void visit(ast::stmt::Catch &node);
        void visit(ast::stmt::Try &node);
        void visit(ast::stmt::Continue &node);
        void visit(ast::stmt::Break &node);
        void visit(ast::stmt::Return &node);
        void visit(ast::stmt::Yield &node);
        void visit(ast::stmt::Expr &node);
        void visit(ast::stmt::Declaration &node);
        // Declaration visitor
        void visit(ast::decl::TypeParam &node);
        void visit(ast::decl::Constraint &node);
        void visit(ast::decl::Param &node);
        void visit(ast::decl::Params &node);
        void visit(ast::decl::Function &node);
        void visit(ast::decl::Variable &node);
        void visit(ast::decl::Parent &node);
        void visit(ast::decl::Enumerator &node);
        void visit(ast::decl::Compound &node);
        // Module level visitor
        void visit(ast::Import &node);
        void visit(ast::Module &node);
        void visit(ast::FolderModule &node);

        template<typename Scope_Type, typename Ast_Type>
            requires std::derived_from<Scope_Type, scope::Scope> && std::derived_from<Ast_Type, ast::AstNode>
        std::shared_ptr<Scope_Type> begin_scope(Ast_Type &node) {
            auto scope = std::make_shared<Scope_Type>(&node);
            cur_scope = &*scope;
            return scope;
        }

        template<typename Scope_Type>
            requires std::derived_from<Scope_Type, scope::Scope>
        std::shared_ptr<Scope_Type> find_scope(const string &name) {
            auto scope = get_current_scope()->get_variable(name);
            cur_scope = &*scope;
            return cast<Scope_Type>(scope);
        }

        inline void end_scope() {
            cur_scope = cur_scope->get_parent();
        }
    };
}    // namespace spade
