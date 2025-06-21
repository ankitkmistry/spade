#include "analyzer.hpp"
#include "info.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"

// NOTE: During statement analysis, get_current_function() is never null

namespace spade
{
    void Analyzer::visit(ast::stmt::Block &node) {
        auto scope = begin_block(node);
        for (const auto &stmt: node.get_statements()) {
            stmt->accept(this);
        }
        end_scope();
    }

    void Analyzer::visit(ast::stmt::If &node) {
        node.get_condition()->accept(this);
        node.get_body()->accept(this);
        if (auto body = node.get_else_body())
            body->accept(this);
    }

    void Analyzer::visit(ast::stmt::While &node) {
        node.get_condition()->accept(this);

        bool old_is_loop_val = is_loop;
        is_loop = true;
        node.get_body()->accept(this);
        is_loop = old_is_loop_val;

        if (auto body = node.get_else_body())
            body->accept(this);
    }

    void Analyzer::visit(ast::stmt::DoWhile &node) {
        bool old_is_loop_val = is_loop;
        is_loop = true;
        node.get_body()->accept(this);
        is_loop = old_is_loop_val;

        node.get_condition()->accept(this);
        if (auto body = node.get_else_body())
            body->accept(this);
    }

    void Analyzer::visit(ast::stmt::Throw &node) {
        node.get_expression()->accept(this);
        switch (_res_expr_info.tag) {
        case ExprInfo::Kind::NORMAL:
            if (_res_expr_info.type_info().tag == TypeInfo::Kind::BASIC &&
                _res_expr_info.type_info().basic().type->has_super(get_internal<scope::Compound>(Internal::SPADE_THROWABLE)))
                ;
            else
                throw error(std::format("expression type must be a subtype of '{}'", get_internal(Internal::SPADE_THROWABLE)->to_string()),
                            node.get_expression());
            break;
        case ExprInfo::Kind::STATIC:
            throw error("cannot throw a type", &node);
        case ExprInfo::Kind::MODULE:
            throw error("cannot throw a module", &node);
        case ExprInfo::Kind::FUNCTION_SET:
            throw error("cannot throw a function", &node);
        }
    }

    void Analyzer::visit(ast::stmt::Catch &node) {
        for (const auto &ref: node.get_references()) {
            ref->accept(this);
            if (_res_expr_info.tag != ExprInfo::Kind::STATIC)
                throw error("reference must be a type", ref);
            if (_res_expr_info.type_info().tag == TypeInfo::Kind::BASIC &&
                _res_expr_info.type_info().basic().type->has_super(get_internal<scope::Compound>(Internal::SPADE_THROWABLE)))
                ;
            else
                throw error(std::format("reference must be a subtype of '{}'", get_internal(Internal::SPADE_THROWABLE)->to_string()), ref);
        }
        declare_variable(*node.get_symbol());
        node.get_body()->accept(this);
    }

    void Analyzer::visit(ast::stmt::Try &node) {
        node.get_body()->accept(this);
        for (const auto &catch_stmt: node.get_catches()) {
            catch_stmt->accept(this);
        }
        if (auto finally = node.get_finally())
            finally->accept(this);
    }

    void Analyzer::visit(ast::stmt::Continue &node) {
        if (!is_loop)
            throw error("continue statement is not inside a loop", &node);
    }

    void Analyzer::visit(ast::stmt::Break &node) {
        if (!is_loop)
            throw error("break statement is not inside a loop", &node);
    }

    void Analyzer::visit(ast::stmt::Return &node) {
        auto ret_type = get_current_function()->get_ret_type();
        if (ret_type.tag == TypeInfo::Kind::BASIC && ret_type.basic().type == &*get_internal(Internal::SPADE_VOID)) {
            if (node.get_expression())
                throw error("void function cannot return a value", node.get_expression());
        } else if (auto expression = node.get_expression()) {
            expression->accept(this);
            resolve_assign(ret_type, _res_expr_info, node);
        } else
            throw error("return statement must have an expression", &node);
    }

    void Analyzer::visit(ast::stmt::Yield &node) {
        // TODO: Improve yield statement
        node.get_expression()->accept(this);
    }

    void Analyzer::visit(ast::stmt::Expr &node) {
        node.get_expression()->accept(this);
        resolve_indexer(_res_expr_info, true, node);
    }

    void Analyzer::visit(ast::stmt::Declaration &node) {
        auto decl = node.get_declaration();
        if (is<ast::decl::Variable>(decl)) {
            decl->accept(this);
        } else {
            // TODO: implement this
            warning("declarations other than variables and constants are not implemented yet", &node);
        }
    }
}    // namespace spade