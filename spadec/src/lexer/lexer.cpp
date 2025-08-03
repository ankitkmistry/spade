#include "lexer.hpp"
#include "utils/error.hpp"

namespace spadec
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

    int Lexer::length() const {
        return static_cast<int>(data.size());
    }

    std::shared_ptr<Token> Lexer::get_token(TokenType type) {
        auto token = make_token(type, data.substr(start, end - start), line, col);
        col += end - start;
        start = end;
        return token;
    }

    std::shared_ptr<Token> Lexer::get_token(TokenType type, const string &text) {
        auto token = make_token(type, text, line, col);
        col += end - start;
        start = end;
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

    static constexpr int hex_value(const char c) {
        switch (tolower(c)) {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'a':
            return 10;
        case 'b':
            return 11;
        case 'c':
            return 12;
        case 'd':
            return 13;
        case 'e':
            return 14;
        case 'f':
            return 15;
        }
        return -1;
    };

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

    string Lexer::handle_escape() {
        if (peek() != '\\')
            return "";
        advance();    // consume back-slash

        string str;
        char c;
        switch (c = advance()) {
        case '\'':
            str += '\'';
            break;
        case '"':
            str += '"';
            break;
        case '\\':
            str += '\\';
            break;
        case 'a':
            str += '\a';
            break;
        case 'b':
            str += '\b';
            break;
        case 'f':
            str += '\f';
            break;
        case 'n':
            str += '\n';
            break;
        case 'r':
            str += '\r';
            break;
        case 't':
            str += '\t';
            break;
        case 'v':
            str += '\v';
            break;
        case 'u': {
            if (!is_hex_digit(peek()))
                throw make_error("expected hex digit");
            uint32_t code_point = hex_value(advance());

            if (!is_hex_digit(peek()))
                throw make_error("expected hex digit");
            code_point <<= 4;
            code_point |= hex_value(advance());

            if (!is_hex_digit(peek()))
                throw make_error("expected hex digit");
            code_point <<= 4;
            code_point |= hex_value(advance());

            if (!is_hex_digit(peek()))
                throw make_error("expected hex digit");
            code_point <<= 4;
            code_point |= hex_value(advance());

            str += static_cast<char>((code_point >> 24) & 0xff);
            str += static_cast<char>((code_point >> 16) & 0xff);
            str += static_cast<char>((code_point >> 8) & 0xff);
            str += static_cast<char>((code_point >> 0) & 0xff);
            break;
        }
        case EOF:
            throw make_error("expected escape sequence");
        default: {
            // Octal escaping
            // For example: '\033' -> escape, '\0' -> null, '\12' -> line feed
            if (is_octal_digit(c)) {
                char num = c - '0';
                if (is_octal_digit(peek())) {
                    c = advance();
                    num <<= 3;
                    num |= c - '0';
                    if (is_octal_digit(peek())) {
                        c = advance();
                        num <<= 3;
                        num |= c - '0';
                    }
                }
                str += num;
            } else
                throw make_error(std::format("unknown escape sequence: '\\{}'", c));
            break;
        }
        }
        return str;
    }

    std::shared_ptr<Token> Lexer::next_token() {
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
                if (match('<'))
                    return get_token(TokenType::LSHIFT);
                if (match('='))
                    return get_token(TokenType::LE);
                return get_token(TokenType::LT);
            case '>':
                if (match('>')) {
                    if (match('>'))
                        return get_token(TokenType::URSHIFT);
                    return get_token(TokenType::RSHIFT);
                }
                if (match('='))
                    return get_token(TokenType::GE);
                return get_token(TokenType::GT);
            case '!':
                if (match('='))
                    return get_token(TokenType::NE);
                throw make_error("unexpected character: !");
            case '?':
                if (match('?'))
                    return get_token(TokenType::ELVIS);
                return get_token(TokenType::HOOK);
            case '~':
                return get_token(TokenType::TILDE);
            case '+':
                return get_token(TokenType::PLUS);
            case '-':
                if (match('>'))
                    return get_token(TokenType::ARROW);
                return get_token(TokenType::DASH);
            case '*':
                if (match('*'))
                    return get_token(TokenType::STAR_STAR);
                return get_token(TokenType::STAR);
            case '/': {
                if (match('*')) {
                    col++;
                    while (true) {
                        if (match('*')) {
                            col++;
                            if (match('/')) {
                                col++;
                                break;
                            }
                        }
                        advance();
                        if (current() == '\n') {
                            line++;
                            col = 1;
                        } else
                            col++;
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
                if (match('='))
                    return get_token(TokenType::EQ);
                return get_token(TokenType::EQUAL);
            case ':':
                return get_token(TokenType::COLON);
            // String
            case '"': {
                string str;
                while (true) {
                    if (peek() == EOF)
                        throw make_error("expected '\"'");
                    str += handle_escape();
                    if (match('"'))
                        break;
                    str += advance();
                }
                return get_token(TokenType::STRING, str);
            }
            case '\'': {
                string str;
                while (true) {
                    if (peek() == EOF)
                        throw make_error("expected '''");
                    str += handle_escape();
                    if (match('\''))
                        break;
                    str += advance();
                }
                return get_token(TokenType::STRING, str);
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
            case '\n':
                start = end;
                line++;
                col = 1;
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
                            if (!is_binary_digit(peek()))
                                throw make_error("expected binary digit");
                            while (is_binary_digit(c = peek()) || c == '_') advance();
                        } else if (match('x') || match('X')) {
                            if (!is_hex_digit(peek()))
                                throw make_error("expected hexadecimal digit");
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
}    // namespace spadec