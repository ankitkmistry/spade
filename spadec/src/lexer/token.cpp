#include <unordered_map>

#include "token.hpp"
#include "utils/utils.hpp"

namespace spade
{
    const static std::unordered_map<string, TokenType> KEYWORDS = {
            {
             "import", TokenType::IMPORT,
             },
            {
             "enum", TokenType::ENUM,
             },
            {
             "class", TokenType::CLASS,
             },
            {
             "interface", TokenType::INTERFACE,
             },
            {
             "annotation", TokenType::ANNOTATION,
             },
            {
             "init", TokenType::INIT,
             },
            {
             "fun", TokenType::FUN,
             },
            {
             "const", TokenType::CONST,
             },
            {
             "var", TokenType::VAR,
             },
            {
             "abstract", TokenType::ABSTRACT,
             },
            {
             "final", TokenType::FINAL,
             },
            {
             "static", TokenType::STATIC,
             },
            {
             "override", TokenType::OVERRIDE,
             },
            {
             "private", TokenType::PRIVATE,
             },
            {
             "protected", TokenType::PROTECTED,
             },
            {
             "internal", TokenType::INTERNAL,
             },
            {
             "public", TokenType::PUBLIC,
             },
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"do", TokenType::DO},
            {"for", TokenType::FOR},
            {"in", TokenType::IN},
            {"match", TokenType::MATCH},
            {"when", TokenType::WHEN},
            {"throw", TokenType::THROW},
            {"try", TokenType::TRY},
            {"catch", TokenType::CATCH},
            {"finally", TokenType::FINALLY},
            {"continue", TokenType::CONTINUE},
            {"break", TokenType::BREAK},
            {"return", TokenType::RETURN},
            {"yield", TokenType::YIELD},
            {"as", TokenType::AS},
            {"is", TokenType::IS},
            {"not", TokenType::NOT},
            {"and", TokenType::AND},
            {"or", TokenType::OR},
            {"super", TokenType::SUPER},
            {"self", TokenType::SELF},
            {"true", TokenType::TRUE},
            {"false", TokenType::FALSE},
            {"null", TokenType::NULL_},
            {"object", TokenType::OBJECT},
            {"type", TokenType::TYPE},
            {"_", TokenType::UNDERSCORE}
    };

    string Token::to_string(bool escape) const {
        return std::format("[{:02d}:{:02d}] {} {}", line, col, TokenInfo::to_string(type), escape ? escape_str(text) : text);
    }

    std::shared_ptr<Token> make_token(TokenType type, const string &text, int line, int col) {
        return std::make_shared<Token>(type, text, line, col);
    }

    bool TokenInfo::get_type_if_keyword(const string &text, TokenType &type) {
        return KEYWORDS.contains(text) ? (type = KEYWORDS.at(text), true) : false;
    }
}    // namespace spade