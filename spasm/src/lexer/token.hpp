#pragma once

#include "utils/common.hpp"

namespace spasm
{
    enum class TokenType {
        DASH,
        ARROW,
        COMMA,
        COLON,
        DOT,
        LPAREN,
        RPAREN,
        LBRACKET,
        RBRACKET,
        NEWLINE,

        // Keywords
        MODULE,
        IMPORT,
        GLOBAL,
        ARG,
        LOCAL,
        EXCEPTION,
        METHOD,
        CLASS,
        FIELD,
        END,

        // Other
        INTEGER,
        FLOAT,
        STRING,        // \".*?\"
        CSTRING,       // \'.\'
        LABEL,         // $[a-zA-Z][a-zA-Z0-9]
        PROPERTY,      // @[a-zA-Z][a-zA-Z0-9]
        IDENTIFIER,    // [a-zA-Z][a-zA-Z0-9]

        // End of file marker
        END_OF_FILE,
    };

    class Token final {
        TokenType type;
        string text;
        int line;
        int col;

      public:
        Token(TokenType type, const string &text, int line, int col) : type(type), text(text), line(line), col(col) {}

        Token(const Token &other) = default;
        Token(Token &&other) noexcept = default;
        Token &operator=(const Token &other) = default;
        Token &operator=(Token &&other) noexcept = default;
        ~Token() = default;

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
                if (c == '\n')
                    res++;
            }
            return res;
        }

        int get_col_end() const {
            if (auto pos = text.find_last_of('\n'); pos != string::npos) {
                return text.size() - pos - (type == TokenType::END_OF_FILE ? 0 : 1);
            }
            return col + text.size() - (type == TokenType::END_OF_FILE ? 0 : 1);
        }
    };

    std::shared_ptr<Token> make_token(TokenType type, const string &text, int line, int col);

    class TokenInfo final {
      private:
        constexpr static std::string_view get_token_type_repr(TokenType type) {
            switch (type) {
                case TokenType::DASH:
                    return "-";
                case TokenType::ARROW:
                    return "->";
                case TokenType::COMMA:
                    return ",";
                case TokenType::LPAREN:
                    return "(";
                case TokenType::RPAREN:
                    return ")";
                case TokenType::DOT:
                    return ".";
                case TokenType::LBRACKET:
                    return "[";
                case TokenType::RBRACKET:
                    return "]";
                case TokenType::NEWLINE:
                    return "<newline>";
                case TokenType::MODULE:
                    return "module";
                case TokenType::IMPORT:
                    return "import";
                case TokenType::GLOBAL:
                    return "global";
                case TokenType::ARG:
                    return "arg";
                case TokenType::LOCAL:
                    return "local";
                case TokenType::EXCEPTION:
                    return "exception";
                case TokenType::METHOD:
                    return "method";
                case TokenType::CLASS:
                    return "class";
                case TokenType::FIELD:
                    return "field";
                case TokenType::END:
                    return "end";
                case TokenType::INTEGER:
                    return "<integer>";
                case TokenType::FLOAT:
                    return "<float>";
                case TokenType::STRING:
                    return "<string>";
                case TokenType::CSTRING:
                    return "<cstring>";
                case TokenType::LABEL:
                    return "<label>";
                case TokenType::PROPERTY:
                    return "<property>";
                case TokenType::IDENTIFIER:
                    return "<identifier>";
                case TokenType::END_OF_FILE:
                    return "<EOF>";
                default:
                    return "";
            }
        }

      public:
        TokenInfo() = delete;
        ~TokenInfo() = delete;

        static bool get_type_if_keyword(const string &text, TokenType &type);

        constexpr static string get_repr(const TokenType type) {
            auto repr = get_token_type_repr(type);
            if (repr.front() == '<' || repr.back() == '>') {
                return string(repr);
            }
            return "'" + string(repr) + "'";
        }

        constexpr static string to_string(const TokenType type) {
            switch (type) {
                case TokenType::DASH:
                    return "DASH";
                case TokenType::ARROW:
                    return "ARROW";
                case TokenType::COMMA:
                    return "COMMA";
                case TokenType::COLON:
                    return "COLON";
                case TokenType::DOT:
                    return "DOT";
                case TokenType::LPAREN:
                    return "LPAREN";
                case TokenType::RPAREN:
                    return "RPAREN";
                case TokenType::LBRACKET:
                    return "LBRACKET";
                case TokenType::RBRACKET:
                    return "RBRACKET";
                case TokenType::NEWLINE:
                    return "NEWLINE";
                case TokenType::MODULE:
                    return "MODULE";
                case TokenType::IMPORT:
                    return "IMPORT";
                case TokenType::GLOBAL:
                    return "GLOBAL";
                case TokenType::ARG:
                    return "ARG";
                case TokenType::LOCAL:
                    return "LOCAL";
                case TokenType::EXCEPTION:
                    return "EXCEPTION";
                case TokenType::METHOD:
                    return "METHOD";
                case TokenType::CLASS:
                    return "CLASS";
                case TokenType::FIELD:
                    return "FIELD";
                case TokenType::END:
                    return "END";
                case TokenType::INTEGER:
                    return "INTEGER";
                case TokenType::FLOAT:
                    return "FLOAT";
                case TokenType::STRING:
                    return "STRING";
                case TokenType::CSTRING:
                    return "CSTRING";
                case TokenType::LABEL:
                    return "LABEL";
                case TokenType::PROPERTY:
                    return "PROPERTY";
                case TokenType::IDENTIFIER:
                    return "IDENTIFIER";
                case TokenType::END_OF_FILE:
                    return "END_OF_FILE";
                default:
                    return "";
            }
        }
    };
}    // namespace spasm