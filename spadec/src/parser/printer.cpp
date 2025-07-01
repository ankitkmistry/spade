#include <ostream>

#include "printer.hpp"
#include "analyzer/symbol_path.hpp"

namespace spade
{
    void Printer::visit(ast::Reference &node) {
        write_repr(node);
        print("Reference '");
        for (size_t i = 0; const auto &elm: node.get_path()) {
            print("{}", elm->get_text());
            if (i < node.get_path().size() - 1)
                print(".");
            i++;
        }
        print("'");
    }

    void Printer::visit(ast::expr::Argument &node) {
        write_repr(node);
        print("expr::Argument");
        print(node.get_name(), "name");
        print(node.get_expr(), "expr");
    }

    void Printer::visit(ast::expr::Slice &node) {
        write_repr(node);
        print("expr::Slice");
        start_level();
        print("kind: ");
        switch (node.get_kind()) {
        case ast::expr::Slice::Kind::INDEX:
            print("INDEX");
            break;
        case ast::expr::Slice::Kind::SLICE:
            print("SLICE");
            break;
        }
        end_level();
        print(node.get_from(), "from");
        print(node.get_to(), "to");
        print(node.get_step(), "step");
    }

    void Printer::visit(ast::type::TypeBuilderMember &node) {
        write_repr(node);
        print("type::TypeBuilderMember");
        print(node.get_name(), "name");
        print(node.get_type(), "type");
    }

    void Printer::visit(ast::type::Reference &type) {
        write_repr(type);
        print("type::Reference");
        print(type.get_reference(), "reference");
        print(type.get_type_args(), "type_args");
    }

    void Printer::visit(ast::type::Function &type) {
        write_repr(type);
        print("type::Function");
        print(type.get_param_types(), "param_types");
        print(type.get_return_type(), "return_type");
    }

    void Printer::visit(ast::type::TypeLiteral &type) {
        write_repr(type);
        print("type::TypeLiteral");
    }

    void Printer::visit(ast::type::Nullable &type) {
        write_repr(type);
        print("type::Nullable");
        print(type.get_type(), "type");
    }

    void Printer::visit(ast::type::TypeBuilder &node) {
        write_repr(node);
        print("type::TypeBuilder");
        print(node.get_members(), "members");
    }

    void Printer::visit(ast::expr::Constant &expr) {
        write_repr(expr);
        print("expr::Constant ({})", expr.get_token()->to_string(true));
    }

    void Printer::visit(ast::expr::Super &expr) {
        write_repr(expr);
        print("expr::Super");
        print(expr.get_reference(), "reference");
    }

    void Printer::visit(ast::expr::Self &expr) {
        write_repr(expr);
        print("expr::Self");
    }

    void Printer::visit(ast::expr::DotAccess &expr) {
        write_repr(expr);
        print("expr::DotAccess");
        print(expr.get_caller(), "caller");
        print(expr.get_safe(), "safe");
        print(expr.get_member(), "member");
    }

    void Printer::visit(ast::expr::Call &expr) {
        write_repr(expr);
        print("expr::Call");
        print(expr.get_caller(), "caller");
        print(expr.get_args(), "args");
    }

    void Printer::visit(ast::expr::Reify &expr) {
        write_repr(expr);
        print("expr::Reify");
        print(expr.get_caller(), "caller");
        print(expr.get_type_args(), "type_args");
    }

    void Printer::visit(ast::expr::Index &expr) {
        write_repr(expr);
        print("expr::Index");
        print(expr.get_caller(), "caller");
        print(expr.get_slices(), "slices");
    }

    void Printer::visit(ast::expr::Unary &expr) {
        write_repr(expr);
        print("expr::Unary");
        print(expr.get_op(), "op");
        print(expr.get_expr(), "expr");
    }

    void Printer::visit(ast::expr::Cast &expr) {
        write_repr(expr);
        print("expr::Cast");
        print(expr.get_expr(), "expr");
        print(expr.get_safe(), "safe");
        print(expr.get_type(), "type");
    }

    void Printer::visit(ast::expr::Binary &expr) {
        write_repr(expr);
        print("expr::Binary");
        print(expr.get_left(), "left");
        print(expr.get_op1(), "op1");
        print(expr.get_op2(), "op2");
        print(expr.get_right(), "right");
    }

    void Printer::visit(ast::expr::ChainBinary &expr) {
        write_repr(expr);
        print("expr::ChainBinary");
        print(expr.get_exprs(), "exprs");
        print(expr.get_ops(), "ops");
    }

    void Printer::visit(ast::expr::Ternary &expr) {
        write_repr(expr);
        print("expr::Ternary");
        print(expr.get_condition(), "condition");
        print(expr.get_on_true(), "on_true");
        print(expr.get_on_false(), "on_false");
    }

    void Printer::visit(ast::expr::Lambda &expr) {
        write_repr(expr);
        print("expr::Lambda");
        print(expr.get_params(), "params");
        print(expr.get_return_type(), "return_type");
        print(expr.get_definition(), "definition");
    }

    void Printer::visit(ast::expr::Assignment &expr) {
        write_repr(expr);
        print("expr::Assignment");
        print(expr.get_assignees(), "assignees");
        print(expr.get_op1(), "op1");
        print(expr.get_op2(), "op2");
        print(expr.get_exprs(), "exprs");
    }

    void Printer::visit(ast::stmt::Block &stmt) {
        write_repr(stmt);
        print("stmt::Block");
        print(stmt.get_statements(), "statements");
    }

    void Printer::visit(ast::stmt::If &stmt) {
        write_repr(stmt);
        print("stmt::If");
        print(stmt.get_condition(), "condition");
        print(stmt.get_body(), "body");
        print(stmt.get_else_body(), "else_body");
    }

    void Printer::visit(ast::stmt::While &stmt) {
        write_repr(stmt);
        print("stmt::While");
        print(stmt.get_condition(), "condition");
        print(stmt.get_body(), "body");
        print(stmt.get_else_body(), "else_body");
    }

    void Printer::visit(ast::stmt::DoWhile &stmt) {
        write_repr(stmt);
        print("stmt::DoWhile");
        print(stmt.get_body(), "body");
        print(stmt.get_condition(), "condition");
        print(stmt.get_else_body(), "else_body");
    }

    void Printer::visit(ast::stmt::Throw &stmt) {
        write_repr(stmt);
        print("stmt::Throw");
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(ast::stmt::Catch &stmt) {
        write_repr(stmt);
        print("stmt::Catch");
        print(stmt.get_references(), "references");
        print(stmt.get_symbol(), "symbol");
        print(stmt.get_body(), "body");
    }

    void Printer::visit(ast::stmt::Try &stmt) {
        write_repr(stmt);
        print("stmt::Try");
        print(stmt.get_body(), "body");
        print(stmt.get_catches(), "catches");
        print(stmt.get_finally(), "finally");
    }

    void Printer::visit(ast::stmt::Continue &stmt) {
        write_repr(stmt);
        print("stmt::Continue");
    }

    void Printer::visit(ast::stmt::Break &stmt) {
        write_repr(stmt);
        print("stmt::Break");
    }

    void Printer::visit(ast::stmt::Return &stmt) {
        write_repr(stmt);
        print("stmt::Return");
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(ast::stmt::Yield &stmt) {
        write_repr(stmt);
        print("stmt::Yield");
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(ast::stmt::Expr &stmt) {
        write_repr(stmt);
        print("stmt::Expr");
        print(stmt.get_expression(), "expression");
    }

    void Printer::visit(ast::stmt::Declaration &node) {
        write_repr(node);
        print("stmt::Declaration");
        print(node.get_declaration(), "declaration");
    }

    void Printer::visit(ast::decl::Param &node) {
        write_repr(node);
        print("decl::Param");
        print(node.get_modifiers(), "modifiers");
        print(node.get_is_const(), "is_const");
        print(node.get_variadic(), "variadic");
        print(node.get_name(), "name");
        print(node.get_type(), "type");
        print(node.get_default_expr(), "default_expr");
    }

    void Printer::visit(ast::decl::Params &node) {
        write_repr(node);
        print("decl::Params");
        print(node.get_modifiers(), "modifiers");
        print(node.get_pos_only(), "pos_only");
        print(node.get_pos_kwd(), "pos_kwd");
        print(node.get_kwd_only(), "kwd_only");
    }

    void Printer::visit(ast::decl::Constraint &node) {
        write_repr(node);
        print("decl::Constraint");
        print(node.get_arg(), "arg");
        print(node.get_type(), "type");
    }

    void Printer::visit(ast::decl::Parent &node) {
        write_repr(node);
        print("decl::Parent");
        print(node.get_reference(), "reference");
        print(node.get_type_args(), "type_args");
    }

    void Printer::visit(ast::decl::Enumerator &node) {
        write_repr(node);
        print("decl::Enumerator");
        print(node.get_name(), "name");
        if (node.get_expr())
            print(node.get_expr(), "expr");
        if (node.get_args())
            print(*node.get_args(), "args");
    }

    void Printer::visit(ast::decl::Function &node) {
        write_repr(node);
        print("decl::Function");
        print(node.get_modifiers(), "modifiers");
        print(node.get_name(), "name");
        print(node.get_type_params(), "type_params");
        print(node.get_constraints(), "constraints");
        print(node.get_params(), "params");
        print(node.get_return_type(), "return_type");
        print(node.get_definition(), "definition");
    }

    void Printer::visit(ast::decl::TypeParam &node) {
        write_repr(node);
        print("decl::TypeParam");
        print(node.get_variance(), "variance");
        print(node.get_name(), "name");
        print(node.get_default_type(), "default_type");
    }

    void Printer::visit(ast::decl::Variable &node) {
        write_repr(node);
        print("decl::Variable");
        print(node.get_modifiers(), "modifiers");
        print(node.get_token(), "token");
        print(node.get_name(), "name");
        print(node.get_expr(), "expr");
    }

    void Printer::visit(ast::decl::Compound &node) {
        write_repr(node);
        print("decl::Compound");
        print(node.get_modifiers(), "modifiers");
        print(node.get_token(), "token");
        print(node.get_name(), "name");
        print(node.get_type_params(), "type_params");
        print(node.get_constraints(), "constraints");
        print(node.get_parents(), "parents");
        print(node.get_enumerators(), "enumerators");
        print(node.get_members(), "members");
    }

    void Printer::visit(ast::Import &node) {
        write_repr(node);
        SymbolPath path;
        for (const auto &element: node.get_elements()) path /= element;
        print("Import from='{}'", path.to_string());
        if (const auto &alias = node.get_alias()) {
            print(" as='{}'", alias->get_text());
        }
    }

    void Printer::visit(ast::Module &node) {
        write_repr(node);
        print("Module '{}'", node.get_file_path().generic_string());
        print(node.get_imports(), "imports");
        print(node.get_members(), "members");
    }

    static void write(std::ostream &os, const details::TreeNode *node, bool entry = true) {
        static std::vector<bool> bool_vec;
        if (entry)
            bool_vec.clear();

        for (size_t i = 0; i < bool_vec.size(); i++) {
            if (i == bool_vec.size() - 1) {
                if (bool_vec[i])
                    os << "└──";
                else
                    os << "├──";
            } else if (bool_vec[i])
                os << "   ";
            else
                os << "│  ";
        }
        os << node->get_text() << "\n";

        bool_vec.push_back(false);
        for (size_t i = 0; const auto &child: node->get_nodes()) {
            if (i == node->get_nodes().size() - 1)
                bool_vec.back() = true;
            write(os, &child, false);
            i++;
        }
        bool_vec.pop_back();
    }

    void Printer::write_to(std::ostream &os) const {
        write(os, &m_root);
    }
}    // namespace spade