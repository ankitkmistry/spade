#include "analyzer.hpp"
#include "info.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "spimp/utils.hpp"

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
        // cond:        |         if <expr> {    ------+-------+
        //              |             <stmts...> <-----+       |
        // body_end:    | +------ } else {     <---------------+
        //              | |           <stmts...>  ----+
        //              | +-----> }   <---------------+

        // Control flow specific
        auto cf_cond = std::make_shared<CFNode>(*node.get_condition(), get_current_block());
        auto &cfg = get_current_function()->cfg();
        cfg.insert_vertex(cf_cond);
        if (last_cf_nodes.empty())
            throw error("unreachable code", &node);
        else
            for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_cond);

        last_cf_nodes = {cf_cond};

        // Evaluate the expression
        eval_expr(node.get_condition(), node);
        // Visit the body
        node.get_body()->accept(this);

        // Set the last cf node as the condition of the branch
        auto cf_body_end = last_cf_nodes;
        last_cf_nodes = {cf_cond};

        // Visit the else body if present
        if (const auto &body = node.get_else_body())
            body->accept(this);

        // Also direct the control flow from the end of the `if` block to the end of the `else` block
        extend_vec(last_cf_nodes, cf_body_end);
    }

    void Analyzer::visit(ast::stmt::While &node) {
        // loop_start:    | while <expr> {   <-------+-------+
        //                |     <stmts...>           |       |
        //                |     <<END>>      --------+       |
        //                | } else {    <--------------------+
        //                |     <stmts...>
        //                | }

        // Control flow specific
        auto cf_loop_start = std::make_shared<CFNode>(*node.get_condition(), get_current_block());
        auto &cfg = get_current_function()->cfg();
        cfg.insert_vertex(cf_loop_start);
        if (last_cf_nodes.empty())
            throw error("unreachable code", &node);
        else
            for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_loop_start);
        last_cf_nodes = {cf_loop_start};

        eval_expr(node.get_condition(), node);

        bool old_is_loop_val = is_loop;
        is_loop = true;
        node.get_body()->accept(this);
        is_loop = old_is_loop_val;

        {    // Add the control flow
            if (last_cf_nodes.empty())
                throw error("loop is redundant", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_loop_start);
            last_cf_nodes.clear();
            last_cf_nodes = {cf_loop_start};
        }

        if (const auto &body = node.get_else_body())
            body->accept(this);
    }

    void Analyzer::visit(ast::stmt::DoWhile &node) {
        // loop_start:    | do {     <-----------+
        //                |     <stmts...>       |
        // cond:          | } while <expr> ------+----------+
        //                | else {                          |
        //                |     <stmts...>       <----------+
        //                | }

        // Control flow specific
        auto cf_loop_start = std::make_shared<CFNode>(node, get_current_block());
        auto &cfg = get_current_function()->cfg();
        {    // Add the control flow
            cfg.insert_vertex(cf_loop_start);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_loop_start);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_loop_start};
        }

        bool old_is_loop_val = is_loop;
        is_loop = true;
        node.get_body()->accept(this);
        is_loop = old_is_loop_val;

        auto cf_cond = std::make_shared<CFNode>(*node.get_condition(), get_current_block());
        {    // Add the control flow
            cfg.insert_vertex(cf_cond);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_cond);
            cfg.insert_edge(cf_cond, cf_loop_start);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_cond};
        }

        eval_expr(node.get_condition(), node);

        if (const auto &body = node.get_else_body())
            body->accept(this);
    }

    void Analyzer::visit(ast::stmt::Throw &node) {
        auto cf_node = std::make_shared<CFNode>(node, get_current_block());
        {    // Add the control flow
            auto &cfg = get_current_function()->cfg();
            cfg.insert_vertex(cf_node);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_node);

            cfg.insert_edge(cf_node, end_cf_node);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_node};
        }

        auto expr_info = eval_expr(node.get_expression(), node);
        last_cf_nodes.clear();

        switch (expr_info.tag) {
        case ExprInfo::Kind::NORMAL:
            if (expr_info.type_info().tag == TypeInfo::Kind::BASIC &&
                expr_info.type_info().basic().type->has_super(get_internal<scope::Compound>(Internal::SPADE_THROWABLE)))
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

    // TODO: implement control flow for try/catch, break/continue

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
        if (const auto &finally = node.get_finally())
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
        auto cf_node = std::make_shared<CFNode>(node, get_current_block());
        {    // Add the control flow
            auto &cfg = get_current_function()->cfg();
            cfg.insert_vertex(cf_node);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_node);

            cfg.insert_edge(cf_node, end_cf_node);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_node};    // to let eval_expr() use it
        }

        if (get_current_function()->is_init() && node.get_expression())
            throw ErrorGroup<AnalyzerError>()
                    .error(error("return statement with value is not allowed in a ctor"))
                    .help(error("remove the expression of the return statement"));

        const auto ret_type = get_current_function()->get_ret_type();

        if (ret_type.tag == TypeInfo::Kind::BASIC && ret_type.basic().type == &*get_internal(Internal::SPADE_VOID)) {
            if (node.get_expression())
                throw error("void function cannot return a value", node.get_expression());
        } else if (const auto &expression = node.get_expression()) {
            auto expr_info = eval_expr(expression, node);
            resolve_assign(ret_type, expr_info, node);
        } else
            throw error("return statement must have an expression", &node);

        // Control flow specific
        last_cf_nodes.clear();
    }

    void Analyzer::visit(ast::stmt::Yield &node) {
        auto cf_node = std::make_shared<CFNode>(node, get_current_block());
        {    // Add the control flow
            auto &cfg = get_current_function()->cfg();
            cfg.insert_vertex(cf_node);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_node);

            cfg.insert_edge(cf_node, end_cf_node);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_node};
        }

        if (get_current_function()->is_init())
            throw error("yield statement is not allowed in a ctor");

        // TODO: Improve yield statement
        eval_expr(node.get_expression(), node);

        // Control flow specific
        last_cf_nodes.clear();
    }

    void Analyzer::visit(ast::stmt::Expr &node) {
        auto cf_node = std::make_shared<CFNode>(*node.get_expression(), get_current_block());
        {    // Add the control flow
            auto &cfg = get_current_function()->cfg();
            cfg.insert_vertex(cf_node);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_node);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_node};
        }

        auto expr_info = eval_expr(node.get_expression(), node);

        // Diagnostic specific
        if (const auto compound = get_current_compound())
            if (const auto scope = expr_info.value_info.scope)
                if (scope->get_type() == scope::ScopeType::FUNCTION) {
                    const auto fn = cast<scope::Function>(scope);
                    if (fn->is_init() && (compound == fn->get_enclosing_compound() || compound->has_super(fn->get_enclosing_compound())))
                        // Don't show diagnostic if the situation is like this in a ctor
                        // super(1, 2)  # super ctor call
                        // init(1, 2)   # self ctor call
                        return;
                }

        if (!is<ast::expr::Assignment>(node.get_expression())) {
            switch (expr_info.tag) {
            case ExprInfo::Kind::NORMAL:
            case ExprInfo::Kind::STATIC:
                switch (expr_info.type_info().tag) {
                case TypeInfo::Kind::BASIC:
                    if (expr_info.type_info().basic().type != get_internal(Internal::SPADE_VOID)) {
                        warning("value of the expression is unused", &node);
                        end_warning();
                    }
                    break;
                case TypeInfo::Kind::FUNCTION:
                    warning("value of the expression is unused", &node);
                    end_warning();
                    break;
                }
                break;
            case ExprInfo::Kind::MODULE:
                warning("value of the expression is unused", &node);
                end_warning();
                break;
            case ExprInfo::Kind::FUNCTION_SET:
                warning("value of the expression is unused", &node);
                end_warning();
                break;
            }
        }
    }

    void Analyzer::visit(ast::stmt::Declaration &node) {
        auto cf_node = std::make_shared<CFNode>(node, get_current_block());
        {    // Add the control flow
            auto &cfg = get_current_function()->cfg();
            cfg.insert_vertex(cf_node);

            if (last_cf_nodes.empty())
                throw error("unreachable code", &node);
            else
                for (const auto &last_cf_node: last_cf_nodes) cfg.insert_edge(last_cf_node, cf_node);

            last_cf_nodes.clear();
            last_cf_nodes = {cf_node};
        }

        auto decl = node.get_declaration();
        if (is<ast::decl::Variable>(decl)) {
            decl->accept(this);
        } else {
            // TODO: implement this
            warning("declarations other than variables and constants are not implemented yet", &node);
            end_warning();
        }
    }
}    // namespace spade