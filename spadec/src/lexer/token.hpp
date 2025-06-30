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
        // Declarations
        CLASS,
        INTERFACE,
        ENUM,
        ANNOTATION,
        VAR,
        CONST,
        FUN,
        INIT,
        // Modifiers
        ABSTRACT,
        FINAL,
        STATIC,
        OVERRIDE,
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
        TRY,
        CATCH,
        FINALLY,
        BREAK,
        CONTINUE,
        THROW,
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
        Token(TokenType type, const string &text, int line, int col) : type(type), text(text), line(line), col(col) {}

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

        string to_string(bool escaped = false) const;

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
            if (const auto pos = text.find_last_of('\n'); pos != string::npos) {
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
            case TokenType::LPAREN:
                return "(";
            case TokenType::RPAREN:
                return ")";
            case TokenType::LBRACE:
                return "{";
            case TokenType::RBRACE:
                return "}";
            case TokenType::LBRACKET:
                return "[";
            case TokenType::RBRACKET:
                return "]";
            case TokenType::LT:
                return "<";
            case TokenType::LE:
                return "<=";
            case TokenType::EQ:
                return "==";
            case TokenType::NE:
                return "!=";
            case TokenType::GE:
                return ">=";
            case TokenType::GT:
                return ">";
            case TokenType::HOOK:
                return "?";
            case TokenType::TILDE:
                return "~";
            case TokenType::PLUS:
                return "+";
            case TokenType::DASH:
                return "-";
            case TokenType::ELVIS:
                return "??";
            case TokenType::STAR:
                return "*";
            case TokenType::STAR_STAR:
                return "**";
            case TokenType::SLASH:
                return "/";
            case TokenType::PERCENT:
                return "%";
            case TokenType::LSHIFT:
                return "<<";
            case TokenType::RSHIFT:
                return ">>";
            case TokenType::URSHIFT:
                return ">>>";
            case TokenType::AMPERSAND:
                return "&";
            case TokenType::PIPE:
                return "|";
            case TokenType::CARET:
                return "^";
            case TokenType::DOT:
                return ".";
            case TokenType::ARROW:
                return "->";
            case TokenType::COMMA:
                return ",";
            case TokenType::EQUAL:
                return "=";
            case TokenType::COLON:
                return ":";
            case TokenType::IMPORT:
                return "import";
            case TokenType::ENUM:
                return "enum";
            case TokenType::CLASS:
                return "class";
            case TokenType::INTERFACE:
                return "interface";
            case TokenType::ANNOTATION:
                return "annotation";
            case TokenType::INIT:
                return "init";
            case TokenType::FUN:
                return "fun";
            case TokenType::CONST:
                return "const";
            case TokenType::VAR:
                return "var";
            case TokenType::ABSTRACT:
                return "abstract";
            case TokenType::FINAL:
                return "final";
            case TokenType::STATIC:
                return "static";
            case TokenType::PRIVATE:
                return "private";
            case TokenType::PROTECTED:
                return "protected";
            case TokenType::INTERNAL:
                return "internal";
            case TokenType::PUBLIC:
                return "public";
            case TokenType::IF:
                return "if";
            case TokenType::ELSE:
                return "else";
            case TokenType::WHILE:
                return "while";
            case TokenType::DO:
                return "do";
            case TokenType::FOR:
                return "for";
            case TokenType::IN:
                return "in";
            case TokenType::MATCH:
                return "match";
            case TokenType::WHEN:
                return "when";
            case TokenType::THROW:
                return "throw";
            case TokenType::TRY:
                return "try";
            case TokenType::CATCH:
                return "catch";
            case TokenType::FINALLY:
                return "finally";
            case TokenType::CONTINUE:
                return "continue";
            case TokenType::BREAK:
                return "break";
            case TokenType::RETURN:
                return "return";
            case TokenType::YIELD:
                return "yield";
            case TokenType::AS:
                return "as";
            case TokenType::IS:
                return "is";
            case TokenType::NOT:
                return "not";
            case TokenType::AND:
                return "and";
            case TokenType::OR:
                return "or";
            case TokenType::SUPER:
                return "super";
            case TokenType::SELF:
                return "self";
            case TokenType::TRUE:
                return "true";
            case TokenType::FALSE:
                return "false";
            case TokenType::NULL_:
                return "null";
            case TokenType::OBJECT:
                return "object";
            case TokenType::TYPE:
                return "type";
            case TokenType::IDENTIFIER:
                return "<identifier>";
            case TokenType::INTEGER:
                return "<integer>";
            case TokenType::FLOAT:
                return "<float>";
            case TokenType::STRING:
                return "<string>";
            case TokenType::UNDERSCORE:
                return "_";
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
            case TokenType::LPAREN:
                return "LPAREN";
            case TokenType::RPAREN:
                return "RPAREN";
            case TokenType::LBRACE:
                return "LBRACE";
            case TokenType::RBRACE:
                return "RBRACE";
            case TokenType::LBRACKET:
                return "LBRACKET";
            case TokenType::RBRACKET:
                return "RBRACKET";
            case TokenType::LT:
                return "LT";
            case TokenType::LE:
                return "LE";
            case TokenType::EQ:
                return "EQ";
            case TokenType::NE:
                return "NE";
            case TokenType::GE:
                return "GE";
            case TokenType::GT:
                return "GT";
            case TokenType::HOOK:
                return "HOOK";
            case TokenType::TILDE:
                return "TILDE";
            case TokenType::PLUS:
                return "PLUS";
            case TokenType::DASH:
                return "DASH";
            case TokenType::ELVIS:
                return "ELVIS";
            case TokenType::STAR:
                return "STAR";
            case TokenType::STAR_STAR:
                return "STAR_STAR";
            case TokenType::SLASH:
                return "SLASH";
            case TokenType::PERCENT:
                return "PERCENT";
            case TokenType::LSHIFT:
                return "LSHIFT";
            case TokenType::RSHIFT:
                return "RSHIFT";
            case TokenType::URSHIFT:
                return "URSHIFT";
            case TokenType::AMPERSAND:
                return "AMPERSAND";
            case TokenType::PIPE:
                return "PIPE";
            case TokenType::CARET:
                return "CARET";
            case TokenType::DOT:
                return "DOT";
            case TokenType::ARROW:
                return "ARROW";
            case TokenType::COMMA:
                return "COMMA";
            case TokenType::EQUAL:
                return "EQUAL";
            case TokenType::COLON:
                return "COLON";
            case TokenType::IMPORT:
                return "IMPORT";
            case TokenType::ENUM:
                return "ENUM";
            case TokenType::CLASS:
                return "CLASS";
            case TokenType::INTERFACE:
                return "INTERFACE";
            case TokenType::ANNOTATION:
                return "ANNOTATION";
            case TokenType::INIT:
                return "INIT";
            case TokenType::FUN:
                return "FUN";
            case TokenType::CONST:
                return "CONST";
            case TokenType::VAR:
                return "VAR";
            case TokenType::ABSTRACT:
                return "ABSTRACT";
            case TokenType::FINAL:
                return "FINAL";
            case TokenType::STATIC:
                return "STATIC";
            case TokenType::PRIVATE:
                return "PRIVATE";
            case TokenType::PROTECTED:
                return "PROTECTED";
            case TokenType::INTERNAL:
                return "INTERNAL";
            case TokenType::PUBLIC:
                return "PUBLIC";
            case TokenType::IF:
                return "IF";
            case TokenType::ELSE:
                return "ELSE";
            case TokenType::WHILE:
                return "WHILE";
            case TokenType::DO:
                return "DO";
            case TokenType::FOR:
                return "FOR";
            case TokenType::IN:
                return "IN";
            case TokenType::MATCH:
                return "MATCH";
            case TokenType::WHEN:
                return "WHEN";
            case TokenType::THROW:
                return "THROW";
            case TokenType::TRY:
                return "TRY";
            case TokenType::CATCH:
                return "CATCH";
            case TokenType::FINALLY:
                return "FINALLY";
            case TokenType::CONTINUE:
                return "CONTINUE";
            case TokenType::BREAK:
                return "BREAK";
            case TokenType::RETURN:
                return "RETURN";
            case TokenType::YIELD:
                return "YIELD";
            case TokenType::AS:
                return "AS";
            case TokenType::IS:
                return "IS";
            case TokenType::NOT:
                return "NOT";
            case TokenType::AND:
                return "AND";
            case TokenType::OR:
                return "OR";
            case TokenType::SUPER:
                return "SUPER";
            case TokenType::SELF:
                return "SELF";
            case TokenType::TRUE:
                return "TRUE";
            case TokenType::FALSE:
                return "FALSE";
            case TokenType::NULL_:
                return "NULL_";
            case TokenType::OBJECT:
                return "OBJECT";
            case TokenType::TYPE:
                return "TYPE";
            case TokenType::IDENTIFIER:
                return "IDENTIFIER";
            case TokenType::INTEGER:
                return "INTEGER";
            case TokenType::FLOAT:
                return "FLOAT";
            case TokenType::STRING:
                return "STRING";
            case TokenType::UNDERSCORE:
                return "UNDERSCORE";
            case TokenType::END_OF_FILE:
                return "END_OF_FILE";
            default:
                return "";
            }
        }
    };
}    // namespace spade