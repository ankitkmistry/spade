#pragma once

#include "token.hpp"
#include "utils/common.hpp"
#include "utils/error.hpp"

namespace spade
{
    class Lexer final {
      private:
        string data;
        int start = 0;
        int end = 0;
        int line = 1;
        int col = 1;

        std::stack<std::shared_ptr<Token>> token_buffer;

        int current() const;
        int peek() const;
        int advance();
        bool match(int c);
        bool is_at_end() const;
        int length() const;

        std::shared_ptr<Token> get_token(TokenType type);
        LexerError make_error(const string &msg) const;
        void complete_float_part(const std::function<bool(int)> &validator, char exp1, char exp2);

      public:
        explicit Lexer(string data) : data(std::move(data)) {}

        std::shared_ptr<Token> next_token();
        void pushback_token(const std::shared_ptr<Token> &token);
    };
}    // namespace spade