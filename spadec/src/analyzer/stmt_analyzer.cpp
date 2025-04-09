#include "analyzer.hpp"

namespace spade
{
    void Analyzer::visit(ast::stmt::Block &node) {}

    void Analyzer::visit(ast::stmt::If &node) {}

    void Analyzer::visit(ast::stmt::While &node) {}

    void Analyzer::visit(ast::stmt::DoWhile &node) {}

    void Analyzer::visit(ast::stmt::Throw &node) {}

    void Analyzer::visit(ast::stmt::Catch &node) {}

    void Analyzer::visit(ast::stmt::Try &node) {}

    void Analyzer::visit(ast::stmt::Continue &node) {}

    void Analyzer::visit(ast::stmt::Break &node) {}

    void Analyzer::visit(ast::stmt::Return &node) {}

    void Analyzer::visit(ast::stmt::Yield &node) {}

    void Analyzer::visit(ast::stmt::Expr &node) {}

    void Analyzer::visit(ast::stmt::Declaration &node) {}
}    // namespace spade