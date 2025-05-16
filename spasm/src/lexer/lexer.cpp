#include "lexer.hpp"
#include "token.hpp"

namespace spasm
{
    int Lexer::current() const {
        if (end - 1 >= length())
            return EOF;
        return data[end - 1];
    }

    int Lexer::peek() const {
        if (end >= length())
            return EOF;
        return data[end];
    }

    int Lexer::advance() {
        if (end >= length())
            return EOF;
        return data[end++];
    }

    bool Lexer::match(int c) {
        if (peek() == c) {
            advance();
            return true;
        }
        return false;
    }

    bool Lexer::is_at_end() const {
        return peek() == EOF;
    }

    size_t Lexer::length() const {
        return data.size();
    }

    std::shared_ptr<Token> Lexer::get_token(TokenType type) {
        auto token = make_token(type, data.substr(start, end - start), line, col);
        col += static_cast<int>(end - start);
        start = static_cast<int>(end);
        return token;
    }

    LexerError Lexer::make_error(const string &msg) const {
        return {msg, file_path, line, col};
    }

    static bool is_binary_digit(int c) {
        return c == '0' || c == '1';
    }

    static bool is_octal_digit(int c) {
        return '0' <= c && c <= '7';
    }

    static bool is_decimal_digit(int c) {
        return '0' <= c && c <= '9';
    }

    static bool is_hex_digit(int c) {
        return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
    }

    void Lexer::complete_float_part(const std::function<bool(int)> &validator, char exp1, char exp2) {
        int c;
        bool allow_underscore = false;
        while (true) {
            if (validator(c = peek()) || (allow_underscore && c == '_')) {
                advance();
                if (!allow_underscore)
                    allow_underscore = true;
            } else
                break;
        }
        if (match(exp1) || match(exp2)) {
            if (match('+') || match('-')) {
                if (!is_decimal_digit(peek()))
                    throw make_error("expected decimal digit");
                while (is_decimal_digit(c = peek())) advance();
            } else {
                throw make_error("expected '+', '-'");
            }
        }
    }

    std::shared_ptr<Token> Lexer::match_identifier(TokenType type) {
        char c = current();
        if (!std::isalpha(c) && c != '_')
            throw make_error(std::format("expected {}", TokenInfo::get_repr(type)));
        while (std::isalnum(c = peek()) || c == '_') advance();
        auto token = get_token(type);
        if (type == TokenType::IDENTIFIER) {
            TokenType keyword_type;
            if (TokenInfo::get_type_if_keyword(token->get_text(), keyword_type)) {
                token->set_type(keyword_type);
                return token;
            }
        }
        return token;
    }

    std::shared_ptr<Token> Lexer::next_token() {
        while (!is_at_end()) {
            start = end;
            switch (int c = advance()) {
                case ',':
                    return get_token(TokenType::COMMA);
                case ':':
                    return get_token(TokenType::COLON);
                case '.':
                    return get_token(TokenType::DOT);
                case '(':
                    return get_token(TokenType::LPAREN);
                case ')':
                    return get_token(TokenType::RPAREN);
                case '[':
                    return get_token(TokenType::LBRACKET);
                case ']':
                    return get_token(TokenType::RBRACKET);
                // String
                case '"':
                    while (true) {
                        if (peek() == EOF)
                            throw make_error("expected '\"'");
                        if (match('\\'))
                            advance();
                        if (match('"'))
                            break;
                        advance();
                    }
                    return get_token(TokenType::STRING);
                case '\'':
                    while (true) {
                        if (peek() == EOF)
                            throw make_error("expected '''");
                        if (match('\\'))
                            advance();
                        if (match('\''))
                            break;
                        advance();
                    }
                    return get_token(TokenType::CSTRING);
                case '$':
                    advance();
                    return match_identifier(TokenType::LABEL);
                case '@':
                    advance();
                    return match_identifier(TokenType::PROPERTY);
                case '\n': {
                    auto token = get_token(TokenType::NEWLINE);
                    line++;
                    col = 1;
                    start = end;
                    return token;
                }
                // Whitespace
                case '#':
                    while (peek() != '\n') {
                        if (peek() == EOF)
                            return get_token(TokenType::END_OF_FILE);
                        advance();
                        col++;
                    }
                    break;
                case ' ':
                case '\t':
                case '\r':
                    start = end;
                    col++;
                    break;
                default: {
                    // Match identifiers
                    if (std::isalpha(c) || c == '_')
                        return match_identifier();
                    if (c == '-') {
                        if (is_decimal_digit(peek()))
                            // Also match negative integers
                            advance();
                        else if (match('>'))
                            return get_token(TokenType::ARROW);
                        else
                            return get_token(TokenType::DASH);
                    }
                    // Match integers and floats
                    if (c == '0') {
                        if (match('b') || match('B')) {
                            // Binary integer
                            if (!is_binary_digit(peek()))
                                throw make_error("expected binary digit");
                            while (is_binary_digit(c = peek()) || c == '_') advance();
                        } else if (match('x') || match('X')) {
                            // Hexadecimal integer
                            if (!is_hex_digit(peek()))
                                throw make_error("expected hexadecimal digit");
                            while (is_hex_digit(c = peek()) || c == '_') advance();
                            if (match('.')) {
                                // Match fractional part
                                complete_float_part(is_hex_digit, 'p', 'P');
                                return get_token(TokenType::FLOAT);
                            }
                        } else {
                            if (match('.')) {
                                // Match fractional part
                                // Such as 0.123 or 0.2e-2
                                complete_float_part(is_decimal_digit, 'e', 'E');
                                return get_token(TokenType::FLOAT);
                            }
                            // Octal integer
                            while (is_octal_digit(c = peek()) || c == '_') advance();
                        }
                        return get_token(TokenType::INTEGER);
                    } else if (is_decimal_digit(c)) {
                        // Decimal integer
                        while (is_decimal_digit(c = peek()) || c == '_') advance();
                        if (match('.')) {
                            // Match fractional part
                            complete_float_part(is_decimal_digit, 'e', 'E');
                            return get_token(TokenType::FLOAT);
                        }
                        return get_token(TokenType::INTEGER);
                    }
                    throw make_error(std::format("unexpected character: {:c}", c));
                }
            }
        }
        return get_token(TokenType::END_OF_FILE);
    }
}    // namespace spasm