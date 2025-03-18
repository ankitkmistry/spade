#pragma once

#include <numeric>
#include <unordered_set>

#include "analyzer/analyzer.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"


namespace spade
{
    class Analyzer : public ast::VisitorBase {
        std::unordered_map<SymbolPath, ast::AstNode *> symbol_table;
        std::unordered_set<std::shared_ptr<Scope>> scopes;
        std::stack<std::shared_ptr<Scope>> scope_stack;

      public:
        void analyze(const std::vector<std::shared_ptr<ast::Module>> &modules);

        // Visitor
        void visit(ast::Reference &node);
        // Type visitor
        void visit(ast::type::Reference &node);
        void visit(ast::type::Function &node);
        void visit(ast::type::TypeLiteral &node);
        void visit(ast::type::TypeOf &node);
        void visit(ast::type::BinaryOp &node);
        void visit(ast::type::Nullable &node);
        void visit(ast::type::TypeBuilder &node);
        void visit(ast::type::TypeBuilderMember &node);
        // Expression visitor
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
    };
}    // namespace spade
