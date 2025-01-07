#pragma once

#include "utils/common.hpp"

namespace spade
{
    enum class TokenType {
        // Brackets
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,
        LT,
        LE,
        EQ,
        NE,
        GE,
        GT,
        // Operators
        BANG,
        HOOK,
        TILDE,
        PLUS,
        DASH,
        ELVIS,
        STAR,
        STAR_STAR,
        SLASH,
        PERCENT,
        LSHIFT,
        RSHIFT,
        URSHIFT,
        AMPERSAND,
        PIPE,
        CARET,

        DOT,
        ARROW,
        COMMA,
        EQUAL,
        COLON,

        // Keywords
        // Heading
        IMPORT,
        // Clauses
        EXTENDS,
        IMPLEMENTS,
        // Declarations
        ENUM,
        CLASS,
        INTERFACE,
        ANNOTATION,
        INIT,
        FUN,
        CONST,
        VAR,
        // Modifiers
        ABSTRACT,
        FINAL,
        STATIC,
        // Accessors
        PRIVATE,
        PROTECTED,
        INTERNAL,
        PUBLIC,
        // Statements
        IF,
        ELSE,
        WHILE,
        DO,
        FOR,
        IN,
        MATCH,
        WHEN,
        THROW,
        TRY,
        CATCH,
        FINALLY,
        CONTINUE,
        BREAK,
        RETURN,
        YIELD,
        // Operators
        AS,
        IS,
        NOT,
        AND,
        OR,
        // Primary expressions
        SUPER,
        SELF,
        // Literals
        TRUE,
        FALSE,
        NULL_,
        // Special keywords
        OBJECT,
        TYPE,
        TYPEOF,
        // Other types
        IDENTIFIER,
        INTEGER,
        FLOAT,
        STRING,
        UNDERSCORE,
        // End of file
        END_OF_FILE
    };

    class Token final {
      private:
        TokenType type;
        string text;
        int line;
        int col;

      public:
        Token(TokenType type, string text, int line, int col) : type(type), text(std::move(text)), line(line), col(col) {}

        Token(const Token &other) = default;
        Token(Token &&other) noexcept = default;
        Token &operator=(const Token &other) = default;
        Token &operator=(Token &&other) noexcept = default;
        ~Token() = default;

        friend bool operator==(const Token &lhs, const Token &rhs) {
            return lhs.type == rhs.type && lhs.text == rhs.text;
        }

        friend bool operator!=(const Token &lhs, const Token &rhs) {
            return !(lhs == rhs);
        }

        bool operator==(TokenType type) const {
            return this->type == type;
        }

        bool operator!=(TokenType type) const {
            return this->type != type;
        }

        bool operator==(const string &text) const {
            return this->text == text;
        }

        bool operator!=(const string &text) const {
            return this->text != text;
        }

        string to_string() const;

        TokenType get_type() const {
            return type;
        }

        void set_type(TokenType type) {
            this->type = type;
        }

        const string &get_text() const {
            return text;
        }

        void set_text(const string &text) {
            this->text = text;
        }

        int get_line() const {
            return line;
        }

        void set_line(int line) {
            this->line = line;
        }

        int get_col() const {
            return col;
        }

        void set_col(int col) {
            this->col = col;
        }

        int get_line_start() const {
            return line;
        }

        int get_col_start() const {
            return col;
        }

        int get_line_end() const {
            int res = line;
            for (auto c: text) {
                if (c == '\n') res++;
            }
            return res;
        }

        int get_col_end() const {
            int res = col;
            for (auto c: text) {
                if (c == '\n') res = 0;
                else
                    res++;
            }
            return res;
        }
    };

    std::shared_ptr<Token> make_token(TokenType type, const string &text, int line, int col);

    class TokenInfo final {
      public:
        TokenInfo() = delete;
        ~TokenInfo() = delete;

        static bool get_type_if_keyword(const string &text, TokenType &type);
        static string get_repr(TokenType type);
        static string to_string(TokenType type);
    };
}    // namespace spade