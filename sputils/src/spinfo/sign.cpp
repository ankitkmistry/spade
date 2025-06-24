#include "sign.hpp"
#include "spimp/error.hpp"
#include "spimp/utils.hpp"

using namespace spade;

const Sign Sign::EMPTY = Sign("");

class SignParser {
    string text;
    size_t i = 0;

  public:
    explicit SignParser(const string &text) : text(text + "\033") {}

    vector<SignElement> parse() {
        if (text.empty())
            return {
                    SignElement{"", Sign::Kind::EMPTY}
            };
        vector<SignElement> elements;
        if (match('[')) {
            elements.emplace_back(IDENTIFIER(), Sign::Kind::TYPE_PARAM);
            expect(']');
        } else {
            string module;
            // allow the unnamed module
            if (isalpha(peek())) {
                module = IDENTIFIER();
                elements.emplace_back(module, Sign::Kind::MODULE);
                while (match(':')) {
                    expect(':');
                    module = IDENTIFIER();
                    elements.emplace_back(module, Sign::Kind::MODULE);
                }
            } else {
                elements.emplace_back(module, Sign::Kind::MODULE);
            }
            while (match('.')) elements.push_back(classOrMethodElement());
        }
        return elements;
    }

    SignElement classOrMethodElement() {
        const auto name = IDENTIFIER();
        vector<string> list;
        // Check type params
        if (match('[')) {
            list = idList();
            expect(']');
        }
        // Check params
        if (match('(')) {
            vector<SignParam> params;
            if (peek() != ')')
                params = paramsElement();
            expect(')');
            return {name, Sign::Kind::METHOD, list, params};
        }
        return {name, Sign::Kind::CLASS, list};
    }

    SignElement classElement() {
        const auto name = IDENTIFIER();
        vector<string> list;
        // Check type params
        if (match('[')) {
            list = idList();
            expect(']');
        }
        return {name, Sign::Kind::CLASS, list};
    }

    // SignElement methodElement() {
    //     auto name = IDENTIFIER();
    //     vector<string> list;
    //     // Check type params
    //     if (match('[')) {
    //         list = idList();
    //         expect(']');
    //     }
    //     // Check params
    //     expect('(');
    //     vector<SignParam> params = paramsElement();
    //     expect(')');
    //     return {name, Sign::Kind::METHOD, list, params};
    // }

    vector<SignParam> paramsElement() {
        vector<SignParam> params;
        params.push_back(paramElement());
        while (peek() == ',') {
            advance();
            params.push_back(paramElement());
        }
        params.shrink_to_fit();
        return params;
    }

    SignParam paramElement() {
        vector<SignElement> elements;
        if (match('[')) {
            elements.emplace_back(IDENTIFIER(), Sign::Kind::TYPE_PARAM);
            expect(']');
            return {SignParam::Kind::CLASS, Sign(elements)};
        }
        string module;
        // allow the unnamed module
        if (isalpha(peek())) {
            module = IDENTIFIER();
            elements.emplace_back(module, Sign::Kind::MODULE);
            while (match(':')) {
                expect(':');
                module = IDENTIFIER();
                elements.emplace_back(module, Sign::Kind::MODULE);
            }
        } else {
            elements.emplace_back(module, Sign::Kind::MODULE);
        }
        do {
            expect('.');
            elements.push_back(classElement());
        } while (peek() == '.');
        if (match('(')) {
            vector<SignParam> params;
            if (peek() != ')')
                params = paramsElement();
            expect(')');
            return {SignParam::Kind::CALLBACK, Sign(elements), params};
        }
        return {SignParam::Kind::CLASS, Sign(elements)};
    }

    vector<string> idList() {
        vector<string> list;
        list.push_back(IDENTIFIER());
        while (peek() == ',') {
            advance();
            list.push_back(IDENTIFIER());
        }
        list.shrink_to_fit();
        return list;
    }

    char peek() const {
        if (i >= text.size())
            throw SignatureError(text);
        size_t j = i;
        for (; j < text.size() && isspace(text[j]); ++j);
        return text[j];
    }

    char advance() {
        if (i >= text.size())
            throw SignatureError(text);
        while (isspace(text[i])) i++;
        return text[i++];
    }

    void expect(char c) {
        if (advance() != c)
            throw error(std::format("expected '{}' at col {}", c, i - 1));
    }

    bool match(char c) {
        if (peek() == c) {
            advance();
            return true;
        }
        return false;
    }

    static constexpr bool is_special_char(char c) {
        switch (c) {
        case '$':
        case '#':
        case '!':
        case '@':
        case '%':
        case '&':
        case '_':
            return true;
        default:
            return false;
        }
    }

    string IDENTIFIER() {
        const auto start = i;
        if (isalpha(peek()) || is_special_char(peek()))
            advance();
        else
            throw error(std::format("expected identifier at col {}", start));
        while (isalnum(peek()) || is_special_char(peek())) advance();
        return text.substr(start, i - start);
    }

    SignatureError error(const string &msg) const {
        return {text.substr(0, text.size() - 1), msg};
    }
};

Sign::Sign() = default;
Sign::Sign(const Sign &other) = default;
Sign::Sign(Sign &&other) noexcept = default;
Sign &Sign::operator=(const Sign &other) = default;
Sign &Sign::operator=(Sign &&other) noexcept = default;
Sign::~Sign() = default;

Sign::Sign(const string &text) {
    SignParser parser{text};
    elements = parser.parse();
}

Sign::Sign(const vector<SignElement> &elements) : elements(elements) {}

string SignParam::to_string() const {
    switch (kind) {
    case Kind::CLASS:
        return name.to_string();
    case Kind::CALLBACK: {
        string param_str;
        for (const auto &param: params) {
            param_str += param.to_string() + ", ";
        }
        if (!param_str.empty()) {
            param_str.pop_back();
            param_str.pop_back();
        }
        return name.to_string() + param_str;
    }
    default:
        throw Unreachable();
    }
}

string SignElement::to_string() const {
    string str;
    switch (kind) {
    case Sign::Kind::EMPTY:
    case Sign::Kind::MODULE:
        str = name;
        break;
    case Sign::Kind::CLASS:
        str = name;
        if (!type_params.empty()) {
            str.append("[");
            str.append(join(type_params, ", "));
            str.append("]");
        }
        break;
    case Sign::Kind::METHOD:
        str = name;
        if (!type_params.empty()) {
            str.append("[");
            str.append(join(type_params, ", "));
            str.append("]");
        }
        str.append("(");
        if (!params.empty()) {
            string param_str;
            for (const auto &param: params) {
                param_str += param.to_string() + ", ";
            }
            if (!param_str.empty()) {
                param_str.pop_back();
                param_str.pop_back();
                str.append(param_str);
            }
        }
        str.append(")");
        break;
    case Sign::Kind::TYPE_PARAM:
        str.append("[");
        str.append(name);
        str.append("]");
        break;
    }
    return str;
}

string Sign::to_string() const {
    string str;
    for (size_t i = 0; i < elements.size(); ++i) {
        const auto element = elements[i];
        if (i > 0) {
            switch (element.get_kind()) {
            case Kind::EMPTY:
                break;
            case Kind::MODULE:
                str.append("::");
                break;
            case Kind::CLASS:
            case Kind::METHOD:
                str.append(".");
                break;
            case Kind::TYPE_PARAM:
                break;
            }
        }
        str.append(element.to_string());
    }
    return str;
}

Sign::Kind Sign::get_kind() const {
    return elements.back().to_string() == "" ? Sign::Kind::EMPTY : elements.back().get_kind();
}

Sign Sign::get_parent() const {
    auto elements = this->elements;
    if (!elements.empty())
        elements.pop_back();
    return Sign(elements);
}

Sign Sign::get_parent_module() const {
    if (get_kind() == Kind::MODULE)
        return Sign(slice(elements, 0, -1));
    int i = 0;
    for (const auto &element: elements) {
        if (element.get_kind() != Kind::MODULE)
            break;
        i++;
    }
    return Sign(slice(elements, 0, i));
}

Sign Sign::get_parent_class() const {
    if (get_kind() != Kind::MODULE) {
        int i = elements.size() - 2;
        if (i < 0)
            return EMPTY;
        if (elements[i].get_kind() == Kind::CLASS)
            return Sign(slice(elements, 0, i));
    }
    return EMPTY;
}

const vector<string> &Sign::get_type_params() const {
    return elements.back().get_type_params();
}

const vector<SignParam> &Sign::get_params() const {
    return elements.back().get_params();
}

string Sign::get_name() const {
    return elements.back().to_string();
}

Sign Sign::operator|(const Sign &sign) const {
    SignParser parser{to_string() + "." + sign.to_string()};
    return Sign(parser.parse());
}

Sign Sign::operator|(const string &str) const {
    SignParser parser{to_string() + "." + str};
    return Sign(parser.parse());
}

Sign Sign::operator|(const SignElement &element) const {
    auto new_elements = elements;
    new_elements.push_back(element);
    SignParser parser{Sign(new_elements).to_string()};
    return Sign(parser.parse());
}

Sign &Sign::operator|=(const Sign &sign) {
    SignParser parser{to_string() + "." + sign.to_string()};
    elements = parser.parse();
    return *this;
}

Sign &Sign::operator|=(const string &str) {
    SignParser parser{to_string() + "." + str};
    elements = parser.parse();
    return *this;
}

Sign &Sign::operator|=(const SignElement &element) {
    elements.push_back(element);
    SignParser parser{to_string()};
    elements = parser.parse();
    return *this;
}
