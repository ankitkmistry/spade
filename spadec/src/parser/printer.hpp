#pragma once

#include <ostream>
#include <sstream>

#include "ast.hpp"
#include "utils/common.hpp"

namespace spade::ast
{
    class Printer final : public VisitorBase {
      private:
        int level = 0;
        std::stringstream ss;
        std::shared_ptr<AstNode> root;
        std::function<string(int)> leading_conv = [](int level) {
            int times = level - 1;
            string s;
            for (int i = 0; i < times; i++) {
                if (i == times - 1) s += "|-";
                else
                    s += "| ";
            }
            return s;
        };

        void start_level();
        void end_level();

        void write_repr(const AstNode *node);

        void print(const std::shared_ptr<Token> &token, const string &name);
        void print(const std::shared_ptr<AstNode> &node, const string &name = "");

        template<typename T>
            requires std::derived_from<T, AstNode>
        void print(const std::vector<std::shared_ptr<T>> &vec, const string &name = "") {
            start_level();
            ss << leading_conv(level);
            if (!name.empty()) ss << name << ": ";
            if (vec.empty()) ss << "[]";
            else {
                start_level();
                int i = 0;
                for (const auto &node: vec) {
                    print(node, std::format("[{}]", i));
                    i++;
                }
                end_level();
            }
            end_level();
        }

        void print(const std::vector<std::shared_ptr<Token>> &vec, const string &name = "") {
            start_level();
            ss << leading_conv(level);
            if (!name.empty()) ss << name << ": ";
            if (vec.empty()) ss << "[]";
            else {
                start_level();
                int i = 0;
                for (const auto &token: vec) {
                    print(token, std::format("[{}]", i));
                    i++;
                }
                end_level();
            }
            end_level();
        }

      public:
        explicit Printer(const std::shared_ptr<AstNode> &root) : root(root) {}

        string to_string() const {
            auto self = const_cast<Printer *>(this);
            self->print(self->root);
            auto res = ss.str();
            self->ss.str("");
            self->ss.clear();
            return res;
        }

        std::shared_ptr<AstNode> get_root() const {
            return root;
        }

        void set_root(const std::shared_ptr<AstNode> &root) {
            this->root = root;
        }

      protected:
        void visit(Reference &node) override;
        void visit(expr::Argument &node) override;
        void visit(expr::Slice &node) override;
        void visit(type::TypeBuilderMember &node) override;
        void visit(decl::Param &node) override;
        void visit(decl::Params &node) override;
        void visit(decl::Constraint &node) override;
        void visit(decl::Parent &node) override;
        void visit(decl::Enumerator &node) override;

        void visit(type::Reference &type) override;
        void visit(type::Function &type) override;
        void visit(type::TypeLiteral &type) override;
        void visit(type::BinaryOp &type) override;
        void visit(type::Nullable &type) override;
        void visit(type::TypeBuilder &node) override;

        void visit(expr::Constant &expr) override;
        void visit(expr::Super &expr) override;
        void visit(expr::Self &expr) override;
        void visit(expr::DotAccess &expr) override;
        void visit(expr::Call &expr) override;
        void visit(expr::Reify &expr) override;
        void visit(expr::Index &expr) override;
        void visit(expr::Unary &expr) override;
        void visit(expr::Cast &expr) override;
        void visit(expr::Binary &expr) override;
        void visit(expr::ChainBinary &expr) override;
        void visit(expr::Ternary &expr) override;
        void visit(expr::Assignment &expr) override;

        void visit(stmt::Block &stmt) override;
        void visit(stmt::If &stmt) override;
        void visit(stmt::While &stmt) override;
        void visit(stmt::DoWhile &stmt) override;
        void visit(stmt::Throw &stmt) override;
        void visit(stmt::Catch &stmt) override;
        void visit(stmt::Try &stmt) override;
        void visit(stmt::Continue &stmt) override;
        void visit(stmt::Break &stmt) override;
        void visit(stmt::Return &stmt) override;
        void visit(stmt::Yield &stmt) override;
        void visit(stmt::Expr &stmt) override;
        void visit(stmt::Declaration &node) override;

        void visit(decl::Function &node) override;
        void visit(decl::TypeParam &node) override;
        void visit(decl::Variable &node) override;
        void visit(decl::Init &node) override;
        void visit(decl::Compound &node) override;

        void visit(Import &node) override;
        void visit(Module &node) override;
        void visit(FolderModule &node) override;
    };
}    // namespace spade::ast