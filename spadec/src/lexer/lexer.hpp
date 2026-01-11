#pragma once

#include "token.hpp"
#include "utils/common.hpp"
#include "utils/error.hpp"
#include <functional>

namespace spadec
{
    class Lexer final {
      private:
        fs::path file_path;
        string data;
        int start = 0;
        int end = 0;
        int line = 1;
        int col = 1;

        int current() const;
        int peek() const;
        int advance();
        bool match(int c);
        bool is_at_end() const;
        int length() const;


        std::shared_ptr<Token> get_token(TokenType type);
        std::shared_ptr<Token> get_token(TokenType type, const string &text);

        LexerError make_error(const string &msg) const;

        void complete_float_part(const std::function<bool(int)> &validator, char exp1, char exp2);
        string handle_escape();

      public:
        explicit Lexer(const fs::path &file_path, const string &data) : file_path(file_path), data(data) {}

        std::shared_ptr<Token> next_token();
    };
}    // namespace spadec
