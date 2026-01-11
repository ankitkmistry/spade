#pragma once

#include <ostream>

#include "ast.hpp"
#include "lexer/token.hpp"
#include "../utils/utils.hpp"

namespace spadec
{
    namespace details
    {
        class TreeNode {
            string text;
            TreeNode *parent = null;
            std::vector<TreeNode> nodes;

          public:
            explicit TreeNode(const string &text = "") : text(text) {}

            TreeNode(const TreeNode &) = default;
            TreeNode(TreeNode &&) = default;
            TreeNode &operator=(const TreeNode &) = default;
            TreeNode &operator=(TreeNode &&) = default;
            ~TreeNode() = default;

            TreeNode *add_child(const TreeNode &node) {
                nodes.push_back(node);
                nodes.back().parent = this;
                return &nodes.back();
            }

            TreeNode *get_parent() {
                return parent;
            }

            const TreeNode *get_parent() const {
                return parent;
            }

            const string &get_text() const {
                return text;
            }

            string &get_text() {
                return text;
            }

            const std::vector<TreeNode> get_nodes() const {
                return nodes;
            }
        };
    }    // namespace details

    class Printer : ast::VisitorBase {
        details::TreeNode m_root;
        details::TreeNode *m_node;

        void start_level() {
            m_node = m_node->add_child(details::TreeNode());
        }

        void end_level() {
            m_node = m_node->get_parent();
        }

        template<typename... Args>
        void print(std::format_string<Args...> fmt, Args &&...args) {
            m_node->get_text() += std::format<Args...>(fmt, std::forward<Args>(args)...);
        }

        void print(const std::shared_ptr<Token> &token, const string &name) {
            if (!token)
                return;
            start_level();
            print("{}: {}", name, token->get_type() == TokenType::STRING ? escape_str(token->to_string()) : token->to_string());
            end_level();
        }

        void print(const std::shared_ptr<ast::AstNode> &node, const string &) {
            if (!node)
                return;
            start_level();
            node->accept(this);
            end_level();
        }

        template<typename T>
            requires std::derived_from<T, ast::AstNode>
        void print(const std::vector<std::shared_ptr<T>> &vec, const string &name) {
            start_level();
            print("{}: ", name);
            if (vec.empty()) {
                print("[]");
            } else {
                // start_level();
                for (size_t i = 0; const auto &node: vec) {
                    print(node, std::format("[{}]", i));
                    i++;
                }
                // end_level();
            }
            end_level();
        }

        void print(const std::vector<std::shared_ptr<Token>> &vec, const string &name) {
            start_level();
            print("{}: ", name);
            if (vec.empty()) {
                print("[]");
            } else {
                // start_level();
                for (size_t i = 0; const auto &token: vec) {
                    print(token, std::format("[{}]", i));
                    i++;
                }
                // end_level();
            }
            end_level();
        }

        void write_repr(const ast::AstNode &node) {
            print("[{:02d}:{:02d}]->[{:02d}:{:02d}] ", node.get_line_start(), node.get_col_start(), node.get_line_end(), node.get_col_end());
        }

        // Visitor
        void visit(ast::Reference &node) override;
        // Type visitor
        void visit(ast::type::Reference &node) override;
        void visit(ast::type::Function &node) override;
        void visit(ast::type::TypeLiteral &node) override;
        void visit(ast::type::Nullable &node) override;
        void visit(ast::type::TypeBuilder &node) override;
        void visit(ast::type::TypeBuilderMember &node) override;
        // Expression visitor
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
        // Statement visitor
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
        // Declaration visitor
        void visit(ast::decl::TypeParam &node) override;
        void visit(ast::decl::Constraint &node) override;
        void visit(ast::decl::Param &node) override;
        void visit(ast::decl::Params &node) override;
        void visit(ast::decl::Function &node) override;
        void visit(ast::decl::Variable &node) override;
        void visit(ast::decl::Parent &node) override;
        void visit(ast::decl::Enumerator &node) override;
        void visit(ast::decl::Compound &node) override;
        // Module level visitor
        void visit(ast::Import &node) override;
        void visit(ast::Module &node) override;

      public:
        explicit Printer(ast::AstNode *ast) : m_root(), m_node(&m_root) {
            ast->accept(this);
        }

        void write_to(std::ostream &os) const;
    };
}    // namespace spadec
