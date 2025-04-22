#include "printer.hpp"

#include <sstream>

#include "utils/utils.hpp"

namespace spade::ast
{
    void Printer::start_level() {
        if (!ss.str().empty() && ss.str().back() != '\n')
            ss << '\n';
        level++;
    }

    void Printer::end_level() {
        if (!ss.str().empty() && ss.str().back() != '\n')
            ss << '\n';
        level--;
    }

    void Printer::write_repr(const AstNode *node) {
        ss << std::format("[{:02d}:{:02d}]->[{:02d}:{:02d}] ", node->get_line_start(), node->get_col_start(),
                          node->get_line_end(), node->get_col_end());
    }

    void Printer::print(const std::shared_ptr<Token> &token, const string &name) {
        start_level();
        ss << leading_conv(level);
        if (!name.empty())
            ss << name << ": ";
        ss << (token ? token->to_string() : "null");
        end_level();
    }

    void Printer::print(const std::shared_ptr<AstNode> &node, const string &name) {
        start_level();
        ss << leading_conv(level);
        if (!name.empty())
            ss << name << ": ";
        if (!node)
            ss << "null";
        else
            node->accept(this);
        end_level();
    }

    void Printer::visit(Reference &node) {
        write_repr(cast<AstNode>(&node));
        ss << "Reference '";
        int i = 0;
        for (const auto &elm: node.get_path()) {
            ss << elm->get_text();
            if (i < node.get_path().size() - 1)
                ss << ".";
            i++;
        }
        ss << "'";
    }

    void Printer::visit(expr::Argument &node) {
        write_repr(cast<AstNode>(&node));
        ss << "expr::Argument";
        print(node.get_name(), "name");
        print(node.get_expr(), "expr");
    }

    void Printer::visit(expr::Slice &node) {
        write_repr(cast<AstNode>(&node));
        ss << "expr::Slice";
        start_level();
        ss << leading_conv(level);
        ss << "kind: ";
        switch (node.get_kind()) {
            case expr::Slice::Kind::INDEX:
                ss << "INDEX";
                break;
            case expr::Slice::Kind::SLICE:
                ss << "SLICE";
                break;
        }
        end_level();
        print(node.get_from(), "from");
        print(node.get_to(), "to");
        print(node.get_step(), "step");
    }

    void Printer::visit(type::TypeBuilderMember &node) {
        write_repr(cast<AstNode>(&node));
        ss << "type::TypeBuilderMember";
        print(node.get_name(), "name");
        print(node.get_type(), "type");
    }

    void Printer::visit(type::Reference &type) {
        write_repr(cast<AstNode>(&type));
        ss << "type::Reference";
        print(type.get_reference(), "reference");
        print(type.get_type_args(), "type_args");
    }

    void Printer::visit(type::Function &type) {
        write_repr(cast<AstNode>(&type));
        ss << "type::Function";
        print(type.get_param_types(), "param_types");
        print(type.get_return_type(), "return_type");
    }

    void Printer::visit(type::TypeLiteral &type) {
        write_repr(cast<AstNode>(&type));
        ss << "type::TypeLiteral";
    }

    void Printer::visit(type::BinaryOp &type) {
        write_repr(cast<AstNode>(&type));
        ss << "type::BinaryOp";
        print(type.get_op(), "op");
        print(type.get_left(), "left");
        print(type.get_right(), "right");
    }

    void Printer::visit(type::Nullable &type) {
        write_repr(cast<AstNode>(&type));
        ss << "type::Nullable";
        print(type.get_type(), "type");
    }

    void Printer::visit(type::TypeBuilder &node) {
        write_repr(cast<AstNode>(&node));
        ss << "type::TypeBuilder";
        print(node.get_members(), "members");
    }

    void Printer::visit(expr::Constant &expr) {
        write_repr(&expr);
        ss << "expr::Constant '" << expr.get_token()->to_string() << "'";
    }

    void Printer::visit(expr::Super &expr) {
        write_repr(&expr);
        ss << "expr::Super";
        print(expr.get_reference(), "reference");
    }

    void Printer::visit(expr::Self &expr) {
        write_repr(&expr);
        ss << "expr::Self";
    }

    void Printer::visit(expr::DotAccess &expr) {
        write_repr(&expr);
        ss << "expr::DotAccess";
        print(expr.get_caller(), "caller");
        print(expr.get_safe(), "safe");
        print(expr.get_member(), "member");
    }

    void Printer::visit(expr::Call &expr) {
        write_repr(&expr);
        ss << "expr::Call";
        print(expr.get_caller(), "caller");
        print(expr.get_args(), "args");
    }

    void Printer::visit(expr::Reify &expr) {
        write_repr(&expr);
        ss << "expr::Reify";
        print(expr.get_caller(), "caller");
        print(expr.get_type_args(), "type_args");
    }

    void Printer::visit(expr::Index &expr) {
        write_repr(&expr);
        ss << "expr::Index";
        print(expr.get_caller(), "caller");
        print(expr.get_slices(), "slices");
    }

    void Printer::visit(expr::Unary &expr) {
        write_repr(&expr);
        ss << "expr::Unary";
        print(expr.get_op(), "op");
        print(expr.get_expr(), "expr");
    }

    void Printer::visit(expr::Cast &expr) {
        write_repr(&expr);
        ss << "expr::Cast";
        print(expr.get_expr(), "expr");
        print(expr.get_safe(), "safe");
        print(expr.get_type(), "type");
    }

    void Printer::visit(expr::Binary &expr) {
        write_repr(&expr);
        ss << "expr::Binary";
        print(expr.get_left(), "left");
        if (expr.get_op2()) {
            print(expr.get_op1(), "op1");
            print(expr.get_op2(), "op2");
        } else {
            print(expr.get_op1(), "op");
        }
        print(expr.get_right(), "right");
    }

    void Printer::visit(expr::ChainBinary &expr) {
        write_repr(&expr);
        ss << "expr::ChainBinary";
        print(expr.get_exprs(), "exprs");
        print(expr.get_ops(), "ops");
    }

    void Printer::visit(expr::Ternary &expr) {
        write_repr(&expr);
        ss << "expr::Ternary";
        print(expr.get_condition(), "condition");
        print(expr.get_on_true(), "on_true");
        print(expr.get_on_false(), "on_false");
    }

    void Printer::visit(expr::Assignment &expr) {
        write_repr(&expr);
        ss << "expr::Assignment";
        print(expr.get_assignees(), "assignees");
        print(expr.get_op1(), "op1");
        print(expr.get_op2(), "op2");
        print(expr.get_exprs(), "exprs");
    }

    void Printer::visit(stmt::Block &stmt) {
        write_repr(&stmt);
        ss << "stmt::Block";
        print(stmt.get_statements(), "statements");
    }

    void Printer::visit(stmt::If &stmt) {
        write_repr(&stmt);
        ss << "stmt::If";
        print(stmt.get_condition(), "condition");
        print(stmt.get_body(), "body");
        print(stmt.get_else_body(), "else_body");
    }

    void Printer::visit(stmt::While &stmt) {
        write_repr(&stmt);
        ss << "stmt::While";
        print(stmt.get_condition(), "condition");
        print(stmt.get_body(), "body");
        print(stmt.get_else_body(), "else_body");
    }

    void Printer::visit(stmt::DoWhile &stmt) {
        write_repr(&stmt);
        ss << "stmt::DoWhile";
        print(stmt.get_body(), "body");
        print(stmt.get_condition(), "condition");
        print(stmt.get_else_body(), "else_body");
    }

    void Printer::visit(stmt::Throw &stmt) {
        write_repr(&stmt);
        ss << "stmt::Throw";
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(stmt::Catch &stmt) {
        write_repr(&stmt);
        ss << "stmt::Catch";
        print(stmt.get_references(), "references");
        print(stmt.get_symbol(), "symbol");
        print(stmt.get_body(), "body");
    }

    void Printer::visit(stmt::Try &stmt) {
        write_repr(&stmt);
        ss << "stmt::Try";
        print(stmt.get_body(), "body");
        print(stmt.get_catches(), "catches");
        print(stmt.get_finally(), "finally");
    }

    void Printer::visit(stmt::Continue &stmt) {
        write_repr(&stmt);
        ss << "stmt::Continue";
    }

    void Printer::visit(stmt::Break &stmt) {
        write_repr(&stmt);
        ss << "stmt::Break";
    }

    void Printer::visit(stmt::Return &stmt) {
        write_repr(&stmt);
        ss << "stmt::Return";
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(stmt::Yield &stmt) {
        write_repr(&stmt);
        ss << "stmt::Yield";
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(stmt::Expr &stmt) {
        write_repr(&stmt);
        ss << "stmt::Expr";
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(stmt::Declaration &node) {
        write_repr(&node);
        ss << "stmt::Declaration";
        print(node.get_declaration(), "declaration");
    }

    void Printer::visit(decl::Param &node) {
        write_repr(&node);
        ss << "decl::Param";
        print(node.get_modifiers(), "modifiers");
        print(node.get_is_const(), "is_const");
        print(node.get_variadic(), "variadic");
        print(node.get_name(), "name");
        print(node.get_type(), "type");
        print(node.get_default_expr(), "default_expr");
    }

    void Printer::visit(decl::Params &node) {
        write_repr(&node);
        ss << "decl::Params";
        print(node.get_modifiers(), "modifiers");
        print(node.get_pos_only(), "pos_only");
        print(node.get_pos_kwd(), "pos_kwd");
        print(node.get_kwd_only(), "kwd_only");
    }

    void Printer::visit(decl::Constraint &node) {
        write_repr(&node);
        ss << "decl::Constraint";
        print(node.get_arg(), "arg");
        print(node.get_type(), "type");
    }

    void Printer::visit(decl::Parent &node) {
        write_repr(&node);
        ss << "decl::Parent";
        print(node.get_reference(), "reference");
        print(node.get_type_args(), "type_args");
    }

    void Printer::visit(decl::Enumerator &node) {
        write_repr(&node);
        ss << "decl::Enumerator";
        print(node.get_name(), "name");
        print(node.get_expr(), "expr");
        if (node.get_args())
            print(*node.get_args(), "args");
    }

    void Printer::visit(decl::Function &node) {
        write_repr(&node);
        ss << "decl::Function";
        print(node.get_modifiers(), "modifiers");
        print(node.get_name(), "name");
        print(node.get_type_params(), "type_params");
        print(node.get_constraints(), "constraints");
        print(node.get_params(), "params");
        print(node.get_return_type(), "return_type");
        print(node.get_definition(), "definition");
    }

    void Printer::visit(decl::TypeParam &node) {
        write_repr(&node);
        ss << "decl::TypeParam";
        print(node.get_variance(), "variance");
        print(node.get_name(), "name");
        print(node.get_default_type(), "default_type");
    }

    void Printer::visit(decl::Variable &node) {
        write_repr(&node);
        ss << "decl::Variable";
        print(node.get_modifiers(), "modifiers");
        print(node.get_token(), "token");
        print(node.get_name(), "name");
        print(node.get_expr(), "expr");
    }

    void Printer::visit(decl::Compound &node) {
        write_repr(&node);
        ss << "decl::Compound";
        print(node.get_modifiers(), "modifiers");
        print(node.get_token(), "token");
        print(node.get_name(), "name");
        print(node.get_type_params(), "type_params");
        print(node.get_constraints(), "constraints");
        print(node.get_parents(), "parents");
        print(node.get_enumerators(), "enumerators");
        print(node.get_members(), "members");
    }

    void Printer::visit(Import &node) {
        write_repr(&node);
        ss << "Import from='" << node.get_path() << "'";
        if (auto alias = node.get_alias()) {
            ss << " as='" << alias->get_text() << "'";
        }
    }

    void Printer::visit(Module &node) {
        write_repr(&node);
        ss << "Module '" << node.get_file_path().generic_string() << "'";
        print(node.get_imports(), "imports");
        print(node.get_members(), "members");
    }

    void Printer::visit(FolderModule &node) {
        // Parser does not generate FolderModule nodes
        throw Unreachable();
    }
}    // namespace spade::ast