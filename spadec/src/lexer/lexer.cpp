#include "lexer.hpp"
#include "utils/error.hpp"

namespace spade
{
    int Lexer::current() const {
        if (end - 1 >= length()) return EOF;
        return data[end - 1];
    }

    int Lexer::peek() const {
        if (end >= length()) return EOF;
        return data[end];
    }

    int Lexer::advance() {
        if (end >= length()) return EOF;
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

    int Lexer::length() const {
        return static_cast<int>(data.size());
    }

    std::shared_ptr<Token> Lexer::get_token(TokenType type) {
        auto token = make_token(type, data.substr(start, end - start), line, col);
        col += end - start;
        start = end;
        return token;
    }

    LexerError Lexer::make_error(const string &msg) const {
        return {msg, line, col};
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
                if (!allow_underscore) allow_underscore = true;
            } else
                break;
        }
        if (match(exp1) || match(exp2)) {
            if (match('+') || match('-')) {
                if (!is_decimal_digit(peek())) throw make_error("expected decimal digit");
                while (is_decimal_digit(c = peek())) advance();
            } else {
                throw make_error("expected '+', '-'");
            }
        }
    }

    std::shared_ptr<Token> Lexer::next_token() {
        if (!token_buffer.empty()) {
            auto token = token_buffer.top();
            token_buffer.pop();
            return token;
        }
        while (!is_at_end()) {
            start = end;
            switch (int c = advance()) {
                case '(':
                    return get_token(TokenType::LPAREN);
                case ')':
                    return get_token(TokenType::RPAREN);
                case '{':
                    return get_token(TokenType::LBRACE);
                case '}':
                    return get_token(TokenType::RBRACE);
                case '[':
                    return get_token(TokenType::LBRACKET);
                case ']':
                    return get_token(TokenType::RBRACKET);
                case '<':
                    if (match('<')) return get_token(TokenType::LT);
                    if (match('=')) return get_token(TokenType::LE);
                    return get_token(TokenType::LT);
                case '>':
                    if (match('>')) {
                        if (match('>')) return get_token(TokenType::URSHIFT);
                        return get_token(TokenType::RSHIFT);
                    }
                    if (match('=')) return get_token(TokenType::GE);
                    return get_token(TokenType::GT);
                case '!':
                    if (match('=')) return get_token(TokenType::NE);
                    return get_token(TokenType::BANG);
                case '?':
                    if (match('?')) return get_token(TokenType::ELVIS);
                    return get_token(TokenType::HOOK);
                case '~':
                    return get_token(TokenType::TILDE);
                case '+':
                    return get_token(TokenType::PLUS);
                case '-':
                    if (match('>')) return get_token(TokenType::ARROW);
                    return get_token(TokenType::DASH);
                case '*':
                    if (match('*')) return get_token(TokenType::STAR_STAR);
                    return get_token(TokenType::STAR);
                case '/': {
                    if (match('*')) {
                        while (true) {
                            if (match('*') && match('/')) break;
                            advance();
                        }
                    } else
                        return get_token(TokenType::SLASH);
                    break;
                }
                case '%':
                    return get_token(TokenType::PERCENT);
                case '&':
                    return get_token(TokenType::AMPERSAND);
                case '|':
                    return get_token(TokenType::PIPE);
                case '^':
                    return get_token(TokenType::CARET);
                case '.':
                    return get_token(TokenType::DOT);
                case ',':
                    return get_token(TokenType::COMMA);
                case '=':
                    if (match('=')) return get_token(TokenType::EQ);
                    return get_token(TokenType::EQUAL);
                case ':':
                    return get_token(TokenType::COLON);
                // String
                case '"':
                    while (true) {
                        if (peek() == EOF) throw make_error("expected '\"'");
                        if (match('\\')) advance();
                        if (match('"')) break;
                        advance();
                    }
                    return get_token(TokenType::STRING);
                case '\'':
                    while (true) {
                        if (peek() == EOF) throw make_error("expected '''");
                        if (match('\\')) advance();
                        if (match('\'')) break;
                        advance();
                    }
                    return get_token(TokenType::STRING);
                // Whitespace
                case '#':
                    while (peek() != '\n') {
                        if (peek() == EOF) return get_token(TokenType::END_OF_FILE);
                        advance();
                    }
                    break;
                case ' ':
                case '\t':
                case '\r':
                    start = end;
                    col++;
                    break;
                case '\n':
                    line++;
                    col = 1;
                    start = end;
                    break;
                default: {
                    if (std::isalpha(c) || c == '_') {
                        while (std::isalnum(c = peek()) || c == '_') advance();
                        auto token = get_token(TokenType::IDENTIFIER);
                        TokenType keyword_type;
                        if (TokenInfo::get_type_if_keyword(token->get_text(), keyword_type)) {
                            token->set_type(keyword_type);
                            return token;
                        }
                        return token;
                    }
                    if (is_decimal_digit(c)) {
                        if (c == '0') {
                            if (match('b') || match('B')) {
                                if (!is_binary_digit(peek())) throw make_error("expected binary digit");
                                while (is_binary_digit(c = peek()) || c == '_') advance();
                            } else if (match('x') || match('X')) {
                                if (!is_hex_digit(peek())) throw make_error("expected hexadecimal digit");
                                while (is_hex_digit(c = peek()) || c == '_') advance();
                                if (match('.')) {
                                    complete_float_part(is_hex_digit, 'p', 'P');
                                    return get_token(TokenType::FLOAT);
                                }
                            } else {
                                if (match('.')) {
                                    complete_float_part(is_decimal_digit, 'e', 'E');
                                    return get_token(TokenType::FLOAT);
                                }
                                while (is_octal_digit(c = peek()) || c == '_') advance();
                            }
                        } else {
                            while (is_decimal_digit(c = peek()) || c == '_') advance();
                            if (match('.')) {
                                complete_float_part(is_decimal_digit, 'e', 'E');
                                return get_token(TokenType::FLOAT);
                            }
                        }
                        return get_token(TokenType::INTEGER);
                    }
                    throw make_error(std::format("unexpected character: {:c}", c));
                }
            }
        }
        return get_token(TokenType::END_OF_FILE);
    }

    void Lexer::pushback_token(const std::shared_ptr<Token> &token) {
        token_buffer.push(token);
    }
}    // namespace spade