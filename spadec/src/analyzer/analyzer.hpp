#pragma once

#include "utils/error.hpp"
#include "parser/ast.hpp"
#include "info.hpp"
#include "scope.hpp"
#include <memory>

namespace spade
{
    class Analyzer final : public ast::VisitorBase {
        // Internal modules
        enum class Internal { SPADE, SPADE_ANY, SPADE_INT, SPADE_FLOAT, SPADE_BOOL, SPADE_STRING, SPADE_VOID };
        std::unordered_map<Internal, std::shared_ptr<scope::Scope>> internals;
        void load_internal_modules();

        std::unordered_map<ast::Module *, ScopeInfo> module_scopes;
        std::vector<std::shared_ptr<scope::Scope>> scope_stack;

        std::shared_ptr<scope::Module> get_module_of(std::shared_ptr<scope::Scope> scope) const;
        std::shared_ptr<scope::Module> get_current_module() const;
        std::shared_ptr<scope::Scope> get_parent_scope() const;
        std::shared_ptr<scope::Scope> get_current_scope() const;
        std::shared_ptr<scope::Scope> find_name(const string &name) const;
        void resolve_context(const std::shared_ptr<scope::Scope> &scope, const ast::AstNode &node);

        template<ast::HasLineInfo T>
        AnalyzerError error(const string &msg, T node) {
            return AnalyzerError(msg, get_current_module()->get_module_node()->get_file_path(), node);
        }

      public:
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
        void visit(ast::decl::Init &node);
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
            scope_stack.push_back(scope);
            return scope;
        }

        template<typename Scope_Type>
            requires std::derived_from<Scope_Type, scope::Scope>
        std::shared_ptr<Scope_Type> find_scope(const string &name) {
            auto scope = get_current_scope()->get_variable(name);
            scope_stack.push_back(scope);
            return cast<Scope_Type>(scope);
        }

        inline void end_scope() {
            scope_stack.pop_back();
        }
    };
}    // namespace spade
