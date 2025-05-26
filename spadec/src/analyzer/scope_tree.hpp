#pragma once

#include "parser/ast.hpp"
#include "symbol_path.hpp"
#include "info.hpp"
#include "scope.hpp"
#include "utils/error.hpp"

namespace spade
{
    class ScopeTreeBuilder final : public ast::VisitorBase {
        std::shared_ptr<ast::Module> module;
        std::shared_ptr<scope::Module> module_scope;
        std::vector<std::shared_ptr<scope::Scope>> scope_stack;

      public:
        ScopeTreeBuilder(const std::shared_ptr<ast::Module> &module) : module(module) {}

        ScopeTreeBuilder(const ScopeTreeBuilder &other) = default;
        ScopeTreeBuilder(ScopeTreeBuilder &&other) noexcept = default;
        ScopeTreeBuilder &operator=(const ScopeTreeBuilder &other) = default;
        ScopeTreeBuilder &operator=(ScopeTreeBuilder &&other) noexcept = default;
        ~ScopeTreeBuilder() override = default;

      private:
        template<ast::HasLineInfo T>
        AnalyzerError error(const string &msg, T node) const {
            return AnalyzerError(msg, module->get_file_path(), node);
        }

        SymbolPath get_current_path() const;
        void add_symbol(const string &name, const std::shared_ptr<Token> &decl_site, std::shared_ptr<scope::Scope> scope);
        void check_modifiers(ast::AstNode *node, const std::vector<std::shared_ptr<Token>> &modifiers);

        template<typename Scope_Type, typename Ast_Type>
            requires std::derived_from<Scope_Type, scope::Scope> && std::derived_from<Ast_Type, ast::AstNode>
        std::shared_ptr<Scope_Type> begin_scope(Ast_Type &node) {
            auto scope = std::make_shared<Scope_Type>(&node);
            scope_stack.push_back(scope);
            return scope;
        }

        inline void end_scope() {
            scope_stack.pop_back();
        }

      public:
        void visit(ast::Reference &node) override;
        void visit(ast::type::Reference &node) override;
        void visit(ast::type::Function &node) override;
        void visit(ast::type::TypeLiteral &node) override;
        void visit(ast::type::BinaryOp &node) override;
        void visit(ast::type::Nullable &node) override;
        void visit(ast::type::TypeBuilder &node) override;
        void visit(ast::type::TypeBuilderMember &node) override;
        void visit(ast::expr::Constant &node) override;
        void visit(ast::expr::Super &node) override;
        void visit(ast::expr::Self &node) override;
        void visit(ast::expr::DotAccess &node) override;
        void visit(ast::expr::Call &node) override;
        void visit(ast::expr::Argument &node) override;
        void visit(ast::expr::Reify &node) override;
        void visit(ast::expr::Index &node) override;
        void visit(ast::expr::Slice &node) override;
        void visit(ast::expr::Unary &node) override;
        void visit(ast::expr::Cast &node) override;
        void visit(ast::expr::Binary &node) override;
        void visit(ast::expr::ChainBinary &node) override;
        void visit(ast::expr::Ternary &node) override;
        void visit(ast::expr::Lambda &node) override;
        void visit(ast::expr::Assignment &node) override;
        void visit(ast::stmt::Block &node) override;
        void visit(ast::stmt::If &node) override;
        void visit(ast::stmt::While &node) override;
        void visit(ast::stmt::DoWhile &node) override;
        void visit(ast::stmt::Throw &node) override;
        void visit(ast::stmt::Catch &node) override;
        void visit(ast::stmt::Try &node) override;
        void visit(ast::stmt::Continue &node) override;
        void visit(ast::stmt::Break &node) override;
        void visit(ast::stmt::Return &node) override;
        void visit(ast::stmt::Yield &node) override;
        void visit(ast::stmt::Expr &node) override;
        void visit(ast::stmt::Declaration &node) override;
        void visit(ast::decl::TypeParam &node) override;
        void visit(ast::decl::Constraint &node) override;
        void visit(ast::decl::Param &node) override;
        void visit(ast::decl::Params &node) override;
        void visit(ast::decl::Function &node) override;
        void visit(ast::decl::Variable &node) override;
        void visit(ast::decl::Parent &node) override;
        void visit(ast::decl::Enumerator &node) override;
        void visit(ast::decl::Compound &node) override;
        void visit(ast::Import &node) override;
        void visit(ast::Module &node) override;

        const std::shared_ptr<scope::Module> &build();
    };
}    // namespace spade