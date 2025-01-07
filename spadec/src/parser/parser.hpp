#pragma once

#include "utils/utils.hpp"
#include "ast.hpp"
#include "lexer/lexer.hpp"

namespace spade
{
    class Parser final {
        static constexpr int FILL_CONSTANT = 64;

      private:
        fs::path file_path;
        Lexer *lexer;
        std::vector<std::shared_ptr<Token>> tokens;
        int index = 0;

        void fill_tokens_buffer(int n);

        template<typename... Ts>
            requires(std::same_as<TokenType, Ts> && ...)
        static string make_expected_string(Ts... types) {
            std::vector<TokenType> list{types...};
            string result;
            for (int i = 0; i < list.size(); ++i) {
                result += TokenInfo::get_repr(list[i]);
                if (i < list.size() - 1) result += ", ";
            }
            return result;
        }

        std::shared_ptr<Token> current();
        std::shared_ptr<Token> peek(int i = 0);
        std::shared_ptr<Token> advance();

        template<typename... T>
            requires(std::same_as<TokenType, T> && ...)
        std::shared_ptr<Token> match(T... types) {
            const std::vector<TokenType> ts = {types...};
            for (auto t: ts) {
                if (peek()->get_type() == t) return advance();
            }
            return null;
        }

        std::shared_ptr<Token> match(const string &text) {
            if (peek()->get_text() == text) return advance();
            return null;
        }

        template<typename... T>
            requires(std::same_as<TokenType, T> && ...)
        std::shared_ptr<Token> expect(T... types) {
            const std::vector<TokenType> ts = {types...};
            for (auto t: ts) {
                if (peek()->get_type() == t) return advance();
            }
            throw error(std::format("expected {}", make_expected_string(types...)));
        }

        static ParserError error(const string &msg, const std::shared_ptr<Token> &token);
        ParserError error(const string &msg);

        template<typename R, typename C1, typename C2>
        std::shared_ptr<R> rule_or(std::function<std::shared_ptr<C1>()> rule1, std::function<std::shared_ptr<C2>()> rule2) {
            int tok_idx = index;
            try {
                return spade::cast<R>(rule1());
            } catch (const ParserError &) {
                index = tok_idx;
                return spade::cast<R>(rule2());
            }
        }

        template<typename T>
        std::shared_ptr<T> rule_or(std::function<std::shared_ptr<T>()> rule1, std::function<std::shared_ptr<T>()> rule2) {
            int tok_idx = index;
            try {
                return rule1();
            } catch (const ParserError &) {
                index = tok_idx;
                return rule2();
            }
        }

        template<typename T>
        std::shared_ptr<T> rule_optional(std::function<std::shared_ptr<T>()> rule) {
            int tok_idx = index;
            try {
                return rule();
            } catch (const ParserError &) {
                index = tok_idx;
                return null;
            }
        }

        // Parser rules
        std::shared_ptr<ast::Module> module();
        std::shared_ptr<ast::Import> import();
        std::shared_ptr<ast::Reference> reference();

        // Declarations
        std::shared_ptr<ast::Declaration> declaration();
        std::shared_ptr<ast::Declaration> compound_decl();
        std::shared_ptr<ast::Declaration> member_decl();
        std::shared_ptr<ast::Declaration> init_decl();
        std::shared_ptr<ast::Declaration> variable_decl();
        std::shared_ptr<ast::Declaration> function_decl();

        std::vector<std::shared_ptr<Token>> modifiers();
        std::shared_ptr<ast::decl::TypeParam> type_param();
        std::shared_ptr<ast::decl::Constraint> constraint();
        std::shared_ptr<ast::decl::Parent> parent();
        std::shared_ptr<ast::decl::Enumerator> enumerator();
        std::shared_ptr<ast::decl::Params> params();
        std::shared_ptr<ast::decl::Param> param();

        // Statements
        std::shared_ptr<ast::Statement> statements();
        std::shared_ptr<ast::Statement> block();
        std::shared_ptr<ast::Statement> statement();
        std::shared_ptr<ast::Statement> if_stmt();
        std::shared_ptr<ast::Statement> while_stmt();
        std::shared_ptr<ast::Statement> do_while_stmt();
        std::shared_ptr<ast::Statement> try_stmt();
        std::shared_ptr<ast::Statement> catch_stmt();

        // Expressions
        std::shared_ptr<ast::Expression> expression();
        std::shared_ptr<ast::Expression> assignment();
        std::shared_ptr<ast::Expression> ternary();
        // Binary
        std::shared_ptr<ast::Expression> or_();
        std::shared_ptr<ast::Expression> and_();
        std::shared_ptr<ast::Expression> not_();
        std::shared_ptr<ast::Expression> conditional();
        std::shared_ptr<ast::Expression> relational();
        std::shared_ptr<ast::Expression> bit_or();
        std::shared_ptr<ast::Expression> bit_xor();
        std::shared_ptr<ast::Expression> bit_and();
        std::shared_ptr<ast::Expression> shift();
        std::shared_ptr<ast::Expression> term();
        std::shared_ptr<ast::Expression> factor();
        std::shared_ptr<ast::Expression> power();
        std::shared_ptr<ast::Expression> cast();
        std::shared_ptr<ast::Expression> elvis();
        // Unary
        std::shared_ptr<ast::Expression> unary();
        // Postfix
        std::shared_ptr<ast::Expression> postfix();
        std::shared_ptr<ast::expr::Argument> argument();
        std::shared_ptr<ast::expr::Slice> slice();
        // Primary
        std::shared_ptr<ast::Expression> primary();

        // Type expressions
        std::shared_ptr<ast::Type> type();
        std::shared_ptr<ast::Type> union_type();
        std::shared_ptr<ast::Type> intersection_type();
        std::shared_ptr<ast::Type> nullable_type();
        std::shared_ptr<ast::Type> primary_type();
        std::shared_ptr<ast::type::TypeBuilderMember> type_builder_member();

        // Comma separated lists
        std::vector<std::shared_ptr<ast::Type>> type_list();
        std::vector<std::shared_ptr<ast::Expression>> assignee_list();
        std::vector<std::shared_ptr<ast::Expression>> expr_list();
        std::vector<std::shared_ptr<ast::expr::Argument>> argument_list();
        std::vector<std::shared_ptr<ast::expr::Slice>> slice_list();
        std::vector<std::shared_ptr<ast::Reference>> reference_list();
        std::vector<std::shared_ptr<ast::decl::Param>> param_list();
        std::vector<std::shared_ptr<ast::decl::TypeParam>> type_param_list();
        std::vector<std::shared_ptr<ast::decl::Constraint>> constraint_list();
        std::vector<std::shared_ptr<ast::decl::Parent>> parent_list();
        std::vector<std::shared_ptr<ast::decl::Enumerator>> enumerator_list();
        std::vector<std::shared_ptr<ast::type::TypeBuilderMember>> type_builder_member_list();

      public:
        explicit Parser(const fs::path &file_path, Lexer *lexer) : file_path(file_path), lexer(lexer) {}

        Parser(const Parser &other) = default;
        Parser(Parser &&other) noexcept = default;
        Parser &operator=(const Parser &other) = default;
        Parser &operator=(Parser &&other) noexcept = default;
        ~Parser() = default;

        std::shared_ptr<ast::Module> parse();

        Lexer *get_lexer() const {
            return lexer;
        }
    };
}    // namespace spade