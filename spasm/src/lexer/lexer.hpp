#pragma once

#include <cstddef>
#include <filesystem>
#include <functional>

#include "../utils/common.hpp"
#include "../utils/error.hpp"
#include "lexer/token.hpp"
#include "token.hpp"

namespace spasm
{
    class Lexer final {
      private:
        fs::path file_path;
        string data;
        size_t start = 0;
        size_t end = 0;
        int line = 1;
        int col = 1;

        int current() const;
        int peek() const;
        int advance();
        bool match(int c);
        std::shared_ptr<Token> match_identifier(TokenType type = TokenType::IDENTIFIER);
        bool is_at_end() const;
        size_t length() const;

        std::shared_ptr<Token> get_token(TokenType type);
        LexerError make_error(const string &msg) const;
        void complete_float_part(const std::function<bool(int)> &validator, char exp1, char exp2);

      public:
        explicit Lexer(const fs::path &file_path, const string &data) : file_path(fs::canonical(file_path)), data(data) {}

        std::shared_ptr<Token> next_token();

        const fs::path &get_file_path() const {
            return file_path;
        }
    };
}    // namespace spasm
