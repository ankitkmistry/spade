#include <memory>
#include <unordered_map>

#include "token.hpp"

namespace spasm
{
    const static std::unordered_map<string, TokenType> KEYWORDS = {
            {"module",    TokenType::MODULE   },
            {"import",    TokenType::IMPORT   },
            {"global",    TokenType::GLOBAL   },
            {"arg",       TokenType::ARG      },
            {"local",     TokenType::LOCAL    },
            {"exception", TokenType::EXCEPTION},
            {"method",    TokenType::METHOD   },
            {"class",     TokenType::CLASS    },
            {"field",     TokenType::FIELD    },
            {"end",       TokenType::END      },
    };

    bool TokenInfo::get_type_if_keyword(const string &text, TokenType &type) {
        return KEYWORDS.contains(text) ? (type = KEYWORDS.at(text), true) : false;
    }

    string Token::to_string() const {
        return std::format("[{}:{}]->[{}:{}] {} {}", get_line_start(), get_col_start(), get_line_end(), get_col_end(), TokenInfo::to_string(type),
                           text.c_str());
    }

    std::shared_ptr<Token> make_token(TokenType type, const string &text, int line, int col) {
        return std::make_shared<Token>(type, text, line, col);
    }
}    // namespace spasm
