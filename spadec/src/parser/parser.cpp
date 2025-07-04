#include "parser.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"

namespace spadec
{
    void Parser::fill_tokens_buffer(size_t n) {
        for (size_t i = 0; i < n; ++i) {
            const auto token = lexer->next_token();
            if (token->get_type() == TokenType::END_OF_FILE && !tokens.empty() && tokens.back()->get_type() == TokenType::END_OF_FILE)
                break;    // Already EOF occured
            tokens.push_back(token);
        }
    }

    std::shared_ptr<Token> Parser::current() {
        if (index == 0)
            return null;
        if (index >= tokens.size()) {
            fill_tokens_buffer(1);
            if (index >= tokens.size())
                return tokens.back();    // EOF
        }
        return tokens[index - 1];
    }

    std::shared_ptr<Token> Parser::peek(size_t i) {
        const auto idx = index + i;
        if (idx >= tokens.size()) {
            fill_tokens_buffer(i + 1);
            if (idx >= tokens.size())
                return tokens.back();    // EOF
        }
        return tokens[idx];
    }

    std::shared_ptr<Token> Parser::advance() {
        if (index >= tokens.size()) {
            fill_tokens_buffer(1);
            if (index >= tokens.size())
                return tokens.back();    // EOF
        }
        return tokens[index++];
    }

    ParserError Parser::error(const string &msg, const std::shared_ptr<Token> &token) {
        return {msg, file_path, token->get_line_start(), token->get_col_start(), token->get_line_end(), token->get_col_end()};
    }

    ParserError Parser::error(const string &msg) {
        return error(msg, peek());
    }

    std::shared_ptr<ast::Module> Parser::module() {
        std::vector<std::shared_ptr<ast::Import>> imports;
        while (peek()->get_type() == TokenType::IMPORT) imports.push_back(import());
        std::vector<std::shared_ptr<ast::Declaration>> members;
        while (peek()->get_type() != TokenType::END_OF_FILE) members.push_back(declaration());
        if (!imports.empty())
            return std::make_shared<ast::Module>(imports.front(), current(), imports, members, file_path);
        if (!members.empty())
            return std::make_shared<ast::Module>(members.front(), current(), imports, members, file_path);
        return std::make_shared<ast::Module>(peek(), peek(), imports, members, file_path);
    }

    std::shared_ptr<ast::Import> Parser::import() {
        std::vector<string> elements;
        const auto start = expect(TokenType::IMPORT);
        // Check for relative path
        if (match(TokenType::DOT)) {
            if (match(TokenType::DOT))
                elements.push_back("..");
            else
                elements.push_back(".");
        }
        const auto ref = reference();
        // Add all the tokens in reference to elements
        for (const auto &tok: ref->get_path()) elements.push_back(tok->get_text());
        // Check for alias or opened import
        std::shared_ptr<Token> alias;
        if (match(TokenType::AS))
            alias = expect(TokenType::IDENTIFIER);
        else if (match(TokenType::DOT)) {
            expect(TokenType::STAR);
            elements.push_back("*");
        }
        return std::make_shared<ast::Import>(start, current(), elements, ref->get_path().back(), alias);
    }

    std::shared_ptr<ast::Reference> Parser::reference() {
        std::vector<std::shared_ptr<Token>> path;
        path.push_back(expect(TokenType::IDENTIFIER));
        while (peek()->get_type() == TokenType::DOT && peek(1)->get_type() == TokenType::IDENTIFIER) {
            advance();
            path.push_back(advance());
        }
        return std::make_shared<ast::Reference>(path);
    }

    std::shared_ptr<ast::Declaration> Parser::declaration() {
        const auto mods = modifiers();
        std::shared_ptr<ast::Declaration> decl;
        switch (peek()->get_type()) {
        case TokenType::VAR:
        case TokenType::CONST:
            decl = variable_decl();
            break;
        case TokenType::FUN:
            decl = function_decl();
            break;
        case TokenType::CLASS:
        case TokenType::INTERFACE:
        case TokenType::ENUM:
        case TokenType::ANNOTATION:
            decl = compound_decl();
            break;
        default:
            throw error(std::format("expected {}", make_expected_string(TokenType::VAR, TokenType::CONST, TokenType::FUN, TokenType::CLASS,
                                                                        TokenType::INTERFACE, TokenType::ENUM, TokenType::ANNOTATION)));
        }
        decl->set_modifiers(mods);
        return decl;
    }

    std::shared_ptr<ast::Declaration> Parser::compound_decl() {
        const auto token = expect(TokenType::CLASS, TokenType::ENUM, TokenType::INTERFACE, TokenType::ANNOTATION);
        const auto name = expect(TokenType::IDENTIFIER);
        std::vector<std::shared_ptr<ast::decl::TypeParam>> type_params;
        std::vector<std::shared_ptr<ast::decl::Constraint>> constraints;
        bool context_generics = false;
        if (match(TokenType::LBRACKET)) {
            type_params = type_param_list();
            expect(TokenType::RBRACKET);
            context_generics = true;
        }
        std::vector<std::shared_ptr<ast::decl::Parent>> parents;
        if (match(TokenType::COLON)) {
            parents = parent_list();
        }
        if (context_generics && match("where")) {
            constraints = constraint_list();
        }
        std::vector<std::shared_ptr<ast::decl::Enumerator>> enumerators;
        std::vector<std::shared_ptr<ast::Declaration>> members;
        if (match(TokenType::LBRACE)) {
            while (peek()->get_type() == TokenType::IDENTIFIER) enumerators = enumerator_list();
            while (peek()->get_type() != TokenType::RBRACE) members.push_back(member_decl());
            expect(TokenType::RBRACE);
        }
        return std::make_shared<ast::decl::Compound>(token, current(), name, type_params, constraints, parents, enumerators, members);
    }

    std::shared_ptr<ast::Declaration> Parser::member_decl() {
        const auto mods = modifiers();
        std::shared_ptr<ast::Declaration> decl;
        switch (peek()->get_type()) {
        case TokenType::VAR:
        case TokenType::CONST:
            decl = variable_decl();
            break;
        case TokenType::FUN:
            decl = function_decl();
            break;
        case TokenType::INIT:
            decl = init_decl();
            break;
        case TokenType::CLASS:
        case TokenType::INTERFACE:
        case TokenType::ENUM:
        case TokenType::ANNOTATION:
            decl = compound_decl();
            break;
        default:
            throw error(
                    std::format("expected {}", make_expected_string(TokenType::VAR, TokenType::CONST, TokenType::FUN, TokenType::INIT,
                                                                    TokenType::CLASS, TokenType::INTERFACE, TokenType::ENUM, TokenType::ANNOTATION)));
        }
        decl->set_modifiers(mods);
        return decl;
    }

#define DEFINITION()                                                                                                                                 \
    (match(TokenType::EQUAL) ? spade::cast<ast::Statement>(std::make_shared<ast::stmt::Block>(std::make_shared<ast::stmt::Return>(expression())))    \
     : peek()->get_type() == TokenType::LBRACE ? block()                                                                                             \
                                               : throw error(std::format("expected {}", make_expected_string(TokenType::EQUAL, TokenType::LBRACE))))

    std::shared_ptr<ast::Declaration> Parser::init_decl() {
        const auto name = expect(TokenType::INIT);
        expect(TokenType::LPAREN);
        std::shared_ptr<ast::decl::Params> init_params;
        if (peek()->get_type() != TokenType::RPAREN)
            init_params = params();
        expect(TokenType::RPAREN);
        std::shared_ptr<ast::Statement> def = block();
        return std::make_shared<ast::decl::Function>(name, current(), name, std::vector<std::shared_ptr<ast::decl::TypeParam>>{},
                                                     std::vector<std::shared_ptr<ast::decl::Constraint>>{}, init_params, null, def);
    }

    std::shared_ptr<ast::Declaration> Parser::variable_decl() {
        const auto token = expect(TokenType::VAR, TokenType::CONST);
        const auto name = expect(TokenType::IDENTIFIER);
        std::shared_ptr<ast::Type> var_type;
        if (match(TokenType::COLON))
            var_type = type();
        std::shared_ptr<ast::Expression> expr;
        if (match(TokenType::EQUAL))
            expr = expression();
        return std::make_shared<ast::decl::Variable>(token, current(), name, var_type, expr);
    }

    std::shared_ptr<ast::Declaration> Parser::function_decl() {
        const auto token = expect(TokenType::FUN);
        const auto name = expect(TokenType::IDENTIFIER);
        std::vector<std::shared_ptr<ast::decl::TypeParam>> type_params;
        std::vector<std::shared_ptr<ast::decl::Constraint>> constraints;
        bool context_generics = false;
        if (match(TokenType::LBRACKET)) {
            if (peek()->get_type() != TokenType::RBRACKET)
                type_params = type_param_list();
            expect(TokenType::RBRACKET);
            context_generics = true;
        }
        expect(TokenType::LPAREN);
        std::shared_ptr<ast::decl::Params> fun_params;
        if (peek()->get_type() != TokenType::RPAREN)
            fun_params = params();
        expect(TokenType::RPAREN);
        std::shared_ptr<ast::Type> ret_type;
        if (match(TokenType::ARROW))
            ret_type = type();
        if (context_generics && match("where")) {
            constraints = constraint_list();
        }
        std::shared_ptr<ast::Statement> def;
        if (peek()->get_type() == TokenType::EQUAL || peek()->get_type() == TokenType::LBRACE)
            def = DEFINITION();
        return std::make_shared<ast::decl::Function>(token, current(), name, type_params, constraints, fun_params, ret_type, def);
    }

#undef DEFINITION

    std::vector<std::shared_ptr<Token>> Parser::modifiers() {
        std::vector<std::shared_ptr<Token>> mods;
        while (match(TokenType::ABSTRACT, TokenType::FINAL, TokenType::STATIC, TokenType::OVERRIDE, TokenType::PRIVATE, TokenType::INTERNAL,
                     TokenType::PROTECTED, TokenType::PUBLIC)) {
            mods.push_back(current());
        }
        return mods;
    }

    std::shared_ptr<ast::decl::TypeParam> Parser::type_param() {
        std::shared_ptr<Token> variance;
        if (match("out") || match(TokenType::IN))
            variance = current();
        // else
        //     throw error("expected 'out', 'in");
        const auto name = expect(TokenType::IDENTIFIER);
        std::shared_ptr<ast::Type> default_type;
        if (match(TokenType::EQUAL))
            default_type = type();
        return std::make_shared<ast::decl::TypeParam>(variance, current(), name, default_type);
    }

    std::shared_ptr<ast::decl::Constraint> Parser::constraint() {
        const auto arg = expect(TokenType::IDENTIFIER);
        expect(TokenType::COLON);
        const auto c_type = type();
        return std::make_shared<ast::decl::Constraint>(arg, c_type);
    }

    std::shared_ptr<ast::decl::Parent> Parser::parent() {
        const auto ref = reference();
        std::vector<std::shared_ptr<ast::Type>> type_args;
        if (match(TokenType::LBRACKET)) {
            type_args = type_list();
            expect(TokenType::RBRACKET);
        }
        return std::make_shared<ast::decl::Parent>(current(), ref, type_args);
    }

    std::shared_ptr<ast::decl::Enumerator> Parser::enumerator() {
        const auto name = expect(TokenType::IDENTIFIER);
        if (match(TokenType::EQUAL)) {
            const auto expr = expression();
            return std::make_shared<ast::decl::Enumerator>(name, expr);
        } else if (match(TokenType::LPAREN)) {
            std::vector<std::shared_ptr<ast::expr::Argument>> args;
            if (peek()->get_type() != TokenType::RPAREN)
                args = argument_list();
            expect(TokenType::RPAREN);
            return std::make_shared<ast::decl::Enumerator>(current(), name, args);
        }
        return std::make_shared<ast::decl::Enumerator>(name);
    }

    std::shared_ptr<ast::decl::Params> Parser::params() {
        const static std::vector<std::shared_ptr<ast::decl::Param>> EMPTY;
        std::vector<std::shared_ptr<ast::decl::Param>> param_list1 = param_list();
        std::vector<std::shared_ptr<ast::decl::Param>> param_list2;
        std::vector<std::shared_ptr<ast::decl::Param>> param_list3;
        bool got_param_list_1 = !param_list1.empty(), got_param_list_2 = false, got_param_list_3 = false;

        if (!got_param_list_1) {
            if (peek()->get_type() == TokenType::STAR && peek(1)->get_type() == TokenType::COMMA) {
                expect(TokenType::STAR);
                expect(TokenType::COMMA);
                got_param_list_2 = true;
                param_list2 = param_list();
            }
        } else {
            if (current()->get_type() == TokenType::COMMA && peek()->get_type() == TokenType::STAR && peek(1)->get_type() == TokenType::COMMA) {
                expect(TokenType::STAR);
                expect(TokenType::COMMA);
                got_param_list_2 = true;
                param_list2 = param_list();
            }
        }

        if (!got_param_list_1 && !got_param_list_2) {
            if (peek()->get_type() == TokenType::SLASH && peek(1)->get_type() == TokenType::COMMA) {
                expect(TokenType::SLASH);
                expect(TokenType::COMMA);
                got_param_list_3 = true;
                param_list3 = param_list();
            }
        } else {
            if (current()->get_type() == TokenType::COMMA && peek()->get_type() == TokenType::SLASH && peek(1)->get_type() == TokenType::COMMA) {
                expect(TokenType::SLASH);
                expect(TokenType::COMMA);
                got_param_list_3 = true;
                param_list3 = param_list();
            }
        }

        if (!got_param_list_2 && !got_param_list_3)
            return std::make_shared<ast::decl::Params>(got_param_list_1 ? param_list1.front() : null, current(), EMPTY, param_list1, EMPTY);
        if (!got_param_list_2)
            return std::make_shared<ast::decl::Params>(got_param_list_1 ? param_list1.front() : null, current(), EMPTY, param_list1, param_list3);
        if (!got_param_list_3)
            return std::make_shared<ast::decl::Params>(got_param_list_1 ? param_list1.front() : null, current(), param_list1, param_list2, EMPTY);
        return std::make_shared<ast::decl::Params>(got_param_list_1 ? param_list1.front() : null, current(), param_list1, param_list2, param_list3);
    }

    std::shared_ptr<ast::decl::Param> Parser::param() {
        const auto start = peek();

        const auto is_const = match(TokenType::CONST);
        const auto variadic = match(TokenType::STAR);
        const auto name = expect(TokenType::IDENTIFIER);
        std::shared_ptr<ast::Type> param_type;
        std::shared_ptr<ast::Expression> expr;
        if (match(TokenType::COLON))
            param_type = type();
        if (match(TokenType::EQUAL))
            expr = lambda();

        const auto end = current();

        return std::make_shared<ast::decl::Param>(start, end, is_const, variadic, name, param_type, expr);
    }

    std::shared_ptr<ast::Statement> Parser::statements() {
        if (peek()->get_type() == TokenType::LBRACE)
            return block();
        return statement();
    }

    std::shared_ptr<ast::stmt::Block> Parser::block() {
        const auto start = expect(TokenType::LBRACE);
        std::vector<std::shared_ptr<ast::Statement>> stmts;
        while (peek()->get_type() != TokenType::RBRACE) {
            switch (peek()->get_type()) {
            case TokenType::LBRACE:
                stmts.push_back(block());
                break;
            case TokenType::VAR:
            case TokenType::CONST:
            case TokenType::FUN:
            case TokenType::CLASS:
            case TokenType::INTERFACE:
            case TokenType::ENUM:
            case TokenType::ANNOTATION:
                stmts.push_back(std::make_shared<ast::stmt::Declaration>(declaration()));
                break;
            default:
                stmts.push_back(statement());
                break;
            }
        }
        const auto end = expect(TokenType::RBRACE);
        return std::make_shared<ast::stmt::Block>(start, end, stmts);
    }

    std::shared_ptr<ast::Statement> Parser::statement() {
        switch (peek()->get_type()) {
        case TokenType::IF:
            return if_stmt();
        case TokenType::WHILE:
            return while_stmt();
        case TokenType::DO:
            return do_while_stmt();
        case TokenType::TRY:
            return try_stmt();
        case TokenType::CONTINUE:
            return std::make_shared<ast::stmt::Continue>(advance());
        case TokenType::BREAK:
            return std::make_shared<ast::stmt::Break>(advance());
        case TokenType::THROW: {
            const auto token = advance();
            const auto expr = expression();
            return std::make_shared<ast::stmt::Throw>(token, expr);
        }
        case TokenType::RETURN: {
            const auto token = advance();
            const auto expr = rule_optional(&Parser::expression);
            return expr ? std::make_shared<ast::stmt::Return>(token, expr) : std::make_shared<ast::stmt::Return>(token);
        }
        case TokenType::YIELD: {
            const auto token = advance();
            const auto expr = expression();
            return std::make_shared<ast::stmt::Yield>(token, expr);
        }
        default: {
            int tok_idx = index;
            try {
                return std::make_shared<ast::stmt::Expr>(expression());
            } catch (const ParserError &) {
                index = tok_idx;
                throw error("expected a statement or expression");
            }
        }
        }
    }

#define BODY()                                                                                                                                       \
    (match(TokenType::COLON)                   ? statement()                                                                                         \
     : peek()->get_type() == TokenType::LBRACE ? block()                                                                                             \
                                               : throw error(std::format("expected {}", make_expected_string(TokenType::COLON, TokenType::LBRACE))))

    std::shared_ptr<ast::Statement> Parser::if_stmt() {
        const auto token = expect(TokenType::IF);
        const auto expr = expression();
        const auto body = BODY();
        if (match(TokenType::ELSE)) {
            if (peek()->get_type() == TokenType::IF) {
                const auto else_body = if_stmt();
                return std::make_shared<ast::stmt::If>(token, expr, body, else_body);
            } else if (match(TokenType::COLON)) {
                const auto else_body = statement();
                return std::make_shared<ast::stmt::If>(token, expr, body, else_body);
            } else if (peek()->get_type() == TokenType::LBRACE) {
                const auto else_body = block();
                return std::make_shared<ast::stmt::If>(token, expr, body, else_body);
            } else
                throw error(std::format("expected {}", make_expected_string(TokenType::COLON, TokenType::LBRACE, TokenType::IF)));
        }
        return std::make_shared<ast::stmt::If>(token, expr, body, null);
    }

    std::shared_ptr<ast::Statement> Parser::while_stmt() {
        const auto token = expect(TokenType::WHILE);
        const auto expr = expression();
        const auto body = BODY();
        const auto else_body = match(TokenType::ELSE) ? BODY() : null;
        return std::make_shared<ast::stmt::While>(token, expr, body, else_body);
    }

    std::shared_ptr<ast::Statement> Parser::do_while_stmt() {
        const auto token = expect(TokenType::DO);
        const auto body = block();
        expect(TokenType::WHILE);
        const auto expr = expression();
        const auto else_body = match(TokenType::ELSE) ? BODY() : null;
        return std::make_shared<ast::stmt::DoWhile>(token, body, expr, else_body);
    }

    std::shared_ptr<ast::Statement> Parser::try_stmt() {
        const auto token = expect(TokenType::TRY);
        const auto body = BODY();
        std::vector<std::shared_ptr<ast::Statement>> catches;
        std::shared_ptr<Token> finally_token;
        std::shared_ptr<ast::Statement> finally;
        if (match(TokenType::FINALLY)) {
            finally_token = current();
            finally = BODY();
        } else {
            do {
                catches.push_back(catch_stmt());
            } while (peek()->get_type() == TokenType::CATCH);
            if (match(TokenType::FINALLY)) {
                finally_token = current();
                finally = BODY();
            }
        }
        return std::make_shared<ast::stmt::Try>(token, body, catches, finally_token, finally);
    }

    std::shared_ptr<ast::Statement> Parser::catch_stmt() {
        const auto token = expect(TokenType::CATCH);
        const auto refs = reference_list();
        std::shared_ptr<ast::decl::Variable> symbol;
        if (match(TokenType::AS)) {
            const auto symbol_token = expect(TokenType::IDENTIFIER);
            const auto var_tok = std::make_shared<Token>(TokenType::CONST, "const", symbol_token->get_line(), symbol_token->get_col());
            symbol = std::make_shared<ast::decl::Variable>(var_tok, symbol_token, symbol_token, null, null);
        }
        const auto body = BODY();
        return std::make_shared<ast::stmt::Catch>(token, refs, symbol, body);
    }

#undef BODY

    std::shared_ptr<ast::Expression> Parser::expression() {
        return rule_or(&Parser::assignment, &Parser::lambda);
    }

    std::shared_ptr<ast::Expression> Parser::assignment() {
        const auto assignees = assignee_list();
        std::shared_ptr<Token> op1;
        switch (peek()->get_type()) {
        case TokenType::PLUS:
        case TokenType::DASH:
        case TokenType::STAR:
        case TokenType::SLASH:
        case TokenType::PERCENT:
        case TokenType::STAR_STAR:
        case TokenType::LSHIFT:
        case TokenType::RSHIFT:
        case TokenType::URSHIFT:
        case TokenType::AMPERSAND:
        case TokenType::PIPE:
        case TokenType::CARET:
        case TokenType::ELVIS:
            op1 = advance();
            [[fallthrough]];
        case TokenType::EQUAL: {
            std::shared_ptr<Token> op2;
            if (op1) {
                op2 = expect(TokenType::EQUAL);
            } else
                op1 = advance();
            const auto exprs = expr_list();
            return std::make_shared<ast::expr::Assignment>(assignees, op1, op2, exprs);
        }
        default:
            throw error(
                    std::format("expected one of {}",
                                make_expected_string(TokenType::PLUS, TokenType::DASH, TokenType::STAR, TokenType::SLASH, TokenType::PERCENT,
                                                     TokenType::STAR_STAR, TokenType::LSHIFT, TokenType::RSHIFT, TokenType::URSHIFT,
                                                     TokenType::AMPERSAND, TokenType::PIPE, TokenType::CARET, TokenType::ELVIS, TokenType::EQUAL)));
        }
    }

    std::shared_ptr<ast::Expression> Parser::lambda() {
        if (match(TokenType::FUN)) {
            const auto token = current();

            std::shared_ptr<ast::decl::Params> lm_params;
            if (match(TokenType::LPAREN)) {
                if (peek()->get_type() != TokenType::RPAREN)
                    lm_params = params();
                expect(TokenType::RPAREN);
            }

            std::shared_ptr<ast::Type> ret_type;
            if (match(TokenType::ARROW))
                ret_type = type();

            std::shared_ptr<ast::stmt::Block> def;
            switch (peek()->get_type()) {
            case TokenType::COLON: {
                advance();
                const auto start = peek();
                const auto expr = ternary();
                return std::make_shared<ast::expr::Lambda>(token, current(), lm_params, ret_type, expr);
            }
            case TokenType::LBRACE: {
                const auto body = block();
                return std::make_shared<ast::expr::Lambda>(token, current(), lm_params, ret_type, body);
            }
            default:
                throw error(std::format("expected {}", make_expected_string(TokenType::COLON, TokenType::LBRACE)));
            }
        }
        return ternary();
    }

    std::shared_ptr<ast::Expression> Parser::ternary() {
        const auto expr1 = logic_or();
        if (match(TokenType::IF)) {
            const auto expr2 = logic_or();
            expect(TokenType::ELSE);
            const auto expr3 = logic_or();
            return std::make_shared<ast::expr::Ternary>(expr2, expr1, expr3);
        }
        return expr1;
    }

    std::shared_ptr<ast::Expression> Parser::logic_or() {
        auto left = logic_and();
        while (match(TokenType::OR)) {
            const auto op = current();
            const auto right = logic_and();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::logic_and() {
        auto left = logic_not();
        while (match(TokenType::AND)) {
            const auto op = current();
            const auto right = logic_not();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::logic_not() {
        if (match(TokenType::NOT)) {
            const auto op = current();
            const auto expr = logic_not();
            return std::make_shared<ast::expr::Unary>(op, expr);
        }
        return conditional();
    }

    std::shared_ptr<ast::Expression> Parser::conditional() {
        auto left = relational();
        std::shared_ptr<Token> op;
        std::shared_ptr<Token> op_extra;
        switch (peek()->get_type()) {
        case TokenType::IS:
            op = advance();
            op_extra = match(TokenType::NOT);
            break;
        case TokenType::NOT:
            op = advance();
            op_extra = expect(TokenType::IN);
            break;
        case TokenType::IN:
            op = advance();
            break;
        default:
            break;
        }
        if (op) {
            const auto right = relational();
            left = std::make_shared<ast::expr::Binary>(left, op, op_extra, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::relational() {
        std::vector<std::shared_ptr<ast::Expression>> exprs;
        std::vector<std::shared_ptr<Token>> ops;
        const auto expr = bit_or();
        while (true) {
            switch (peek()->get_type()) {
            case TokenType::LT:
            case TokenType::LE:
            case TokenType::EQ:
            case TokenType::NE:
            case TokenType::GE:
            case TokenType::GT:
                ops.push_back(advance());
                if (exprs.empty())
                    exprs.push_back(expr);
                exprs.push_back(bit_or());
                break;
            default:
                if (exprs.empty())
                    return expr;
                return std::make_shared<ast::expr::ChainBinary>(exprs, ops);
            }
        }
    }

    std::shared_ptr<ast::Expression> Parser::bit_or() {
        auto left = bit_xor();
        while (match(TokenType::PIPE)) {
            const auto op = current();
            const auto right = bit_xor();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::bit_xor() {
        auto left = bit_and();
        while (match(TokenType::CARET)) {
            const auto op = current();
            const auto right = bit_and();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::bit_and() {
        auto left = shift();
        while (match(TokenType::AMPERSAND)) {
            const auto op = current();
            const auto right = shift();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::shift() {
        auto left = term();
        while (match(TokenType::LSHIFT) || match(TokenType::RSHIFT) || match(TokenType::URSHIFT)) {
            const auto op = current();
            const auto right = term();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::term() {
        auto left = factor();
        while (match(TokenType::PLUS) || match(TokenType::DASH)) {
            const auto op = current();
            const auto right = factor();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::factor() {
        auto left = power();
        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
            const auto op = current();
            const auto right = power();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::power() {
        std::vector<std::shared_ptr<Token>> ops;
        std::vector<std::shared_ptr<ast::Expression>> exprs;
        exprs.push_back(cast());
        while (match(TokenType::STAR_STAR)) {
            ops.push_back(current());
            exprs.push_back(cast());
        }
        if (exprs.size() == 1)
            return exprs.back();
        std::shared_ptr<ast::Expression> expr = exprs.back();
        for (size_t i = ops.size() - 1; i >= 0; i--) {
            expr = std::make_shared<ast::expr::Binary>(exprs[i], ops[i], expr);
            if (i == 0)
                break;
        }
        return expr;
    }

    std::shared_ptr<ast::Expression> Parser::cast() {
        auto expr = elvis();
        while (match(TokenType::AS)) {
            const auto safe = match(TokenType::HOOK);
            const auto cast_type = type();
            expr = std::make_shared<ast::expr::Cast>(expr, safe, cast_type);
        }
        return expr;
    }

    std::shared_ptr<ast::Expression> Parser::elvis() {
        auto left = unary();
        while (match(TokenType::ELVIS)) {
            const auto op = current();
            const auto right = unary();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::unary() {
        switch (peek()->get_type()) {
        case TokenType::TILDE:
        case TokenType::DASH:
        case TokenType::PLUS: {
            const auto op = advance();
            const auto expr = unary();
            return std::make_shared<ast::expr::Unary>(op, expr);
        }
        default:
            return postfix();
        }
    }

    std::shared_ptr<ast::Expression> Parser::postfix() {
        auto caller = primary();
        std::shared_ptr<Token> safe;
        while (true) {
            size_t parse_point = index;
            safe = match(TokenType::HOOK) ? current() : null;
            switch (peek()->get_type()) {
            case TokenType::DOT: {
                advance();
                const auto member = expect(TokenType::IDENTIFIER, TokenType::INIT);
                caller = std::make_shared<ast::expr::DotAccess>(caller, safe, member);
                break;
            }
            case TokenType::LPAREN: {
                advance();
                std::vector<std::shared_ptr<ast::expr::Argument>> args;
                auto end = match(TokenType::RPAREN);
                if (!end) {
                    args = argument_list();
                    end = expect(TokenType::RPAREN);
                }
                caller = std::make_shared<ast::expr::Call>(end, caller, safe, args);
                break;
            }
            case TokenType::LBRACKET: {
                advance();
                caller = rule_or<ast::Expression, ast::expr::Index, ast::expr::Reify>(
                        [&] {
                            const auto slices = slice_list();
                            const auto end = expect(TokenType::RBRACKET);
                            return std::make_shared<ast::expr::Index>(end, caller, safe, slices);
                        },
                        [&] {
                            const auto type_args = type_list();
                            const auto end = expect(TokenType::RBRACKET);
                            return std::make_shared<ast::expr::Reify>(end, caller, safe, type_args);
                        });
                break;
            }
            default:
                index = parse_point;
                return caller;
            }
        }
    }

    std::shared_ptr<ast::expr::Argument> Parser::argument() {
        std::shared_ptr<Token> name;
        if (peek()->get_type() == TokenType::IDENTIFIER && peek(1)->get_type() == TokenType::COLON) {
            name = advance();
            advance();
        }
        const auto expr = expression();
        return name ? std::make_shared<ast::expr::Argument>(name, expr) : std::make_shared<ast::expr::Argument>(expr);
    }

    std::shared_ptr<ast::expr::Slice> Parser::slice() {
        std::shared_ptr<ast::Expression> from = null;
        std::shared_ptr<ast::Expression> to = null;
        std::shared_ptr<ast::Expression> step = null;

        if (peek()->get_type() != TokenType::COLON)
            from = expression();
        std::shared_ptr<Token> c1 = match(TokenType::COLON);
        if (c1 && peek()->get_type() != TokenType::COLON)
            to = rule_optional(&Parser::expression);
        std::shared_ptr<Token> c2 = match(TokenType::COLON);
        if (c2)
            step = rule_optional(&Parser::expression);

        auto kind = ast::expr::Slice::Kind::SLICE;
        if (from && !c1 && !to && !c2 && !step)
            kind = ast::expr::Slice::Kind::INDEX;

        int line_start, col_start;
        int line_end, col_end;
        // Determine line, col starting
        if (from) {
            line_start = from->get_line_start();
            col_start = from->get_col_start();
        } else if (c1) {
            line_start = c1->get_line();
            col_start = c1->get_col();
        } else if (to) {
            line_start = to->get_line_start();
            col_start = to->get_col_start();
        } else if (c2) {
            line_start = c2->get_line();
            col_start = c2->get_col();
        } else if (step) {
            line_start = step->get_line_start();
            col_start = step->get_col_start();
        } else
            throw error("expected ':', <expression>");
        // Determine line, col ending
        if (step) {
            line_end = step->get_line_end();
            col_end = step->get_col_end();
        } else if (c2) {
            line_end = c2->get_line();
            col_end = c2->get_col();
        } else if (to) {
            line_end = to->get_line_end();
            col_end = to->get_col_end();
        } else if (c1) {
            line_end = c1->get_line();
            col_end = c1->get_col();
        } else if (from) {
            line_end = from->get_line_end();
            col_end = from->get_col_end();
        } else
            throw error("expected ':', <expression>");
        return std::make_shared<ast::expr::Slice>(line_start, line_end, col_start, col_end, kind, from, to, step);
    }

    std::shared_ptr<ast::Expression> Parser::primary() {
        switch (peek()->get_type()) {
        case TokenType::TRUE:
        case TokenType::FALSE:
        case TokenType::NULL_:
        case TokenType::INTEGER:
        case TokenType::FLOAT:
        case TokenType::STRING:
        case TokenType::IDENTIFIER:
        case TokenType::INIT:
            return std::make_shared<ast::expr::Constant>(advance());

        case TokenType::SUPER: {
            std::shared_ptr<Token> start = advance();
            if (match(TokenType::LBRACKET)) {
                const auto ref = reference();
                expect(TokenType::RBRACKET);
                return std::make_shared<ast::expr::Super>(start, current(), ref);
            }
            return std::make_shared<ast::expr::Super>(start, current(), null);
        }
        case TokenType::SELF:
            return std::make_shared<ast::expr::Self>(advance());
        case TokenType::LPAREN: {
            advance();
            const auto expr = expression();
            expect(TokenType::RPAREN);
            return expr;
        }
        default:
            throw error(std::format("expected {}", make_expected_string(TokenType::TRUE, TokenType::FALSE, TokenType::NULL_, TokenType::INTEGER,
                                                                        TokenType::FLOAT, TokenType::STRING, TokenType::IDENTIFIER, TokenType::INIT,
                                                                        TokenType::SUPER, TokenType::SELF, TokenType::LPAREN)));
        }
    }

    std::shared_ptr<ast::Type> Parser::type() {
        return nullable_type();
    }

    std::shared_ptr<ast::Type> Parser::nullable_type() {
        const auto type = primary_type();
        if (match(TokenType::HOOK))
            return std::make_shared<ast::type::Nullable>(current(), type);
        return type;
    }

    std::shared_ptr<ast::Type> Parser::primary_type() {
        switch (peek()->get_type()) {
        case TokenType::IDENTIFIER: {
            const auto ref = reference();
            if (match(TokenType::LBRACKET)) {
                const auto list = type_list();
                const auto end = expect(TokenType::RBRACKET);
                return std::make_shared<ast::type::Reference>(end, ref, list);
            }
            return std::make_shared<ast::type::Reference>(ref);
        }
        case TokenType::TYPE:
            return std::make_shared<ast::type::TypeLiteral>(advance());
        case TokenType::LPAREN: {
            const auto start = advance();
            return rule_or<ast::Type, ast::type::Function, ast::Type>(
                    [&] {
                        std::vector<std::shared_ptr<ast::Type>> params;
                        if (!match(TokenType::RPAREN)) {
                            params = type_list();
                            expect(TokenType::RPAREN);
                        }
                        expect(TokenType::ARROW);
                        const auto ret_type = type();
                        return std::make_shared<ast::type::Function>(start, params, ret_type);
                    },
                    [&] {
                        const auto grouped = type();
                        expect(TokenType::RPAREN);
                        return grouped;
                    });
        }
        case TokenType::OBJECT: {
            const auto start = advance();
            std::vector<std::shared_ptr<ast::type::TypeBuilderMember>> members;
            if (match(TokenType::LBRACE)) {
                if (peek()->get_type() != TokenType::RBRACE)
                    members = type_builder_member_list();
                expect(TokenType::RBRACE);
            }
            return std::make_shared<ast::type::TypeBuilder>(start, current(), members);
        }
        default:
            throw error(
                    std::format("expected {}", make_expected_string(TokenType::IDENTIFIER, TokenType::TYPE, TokenType::LPAREN, TokenType::OBJECT)));
        }
    }

    std::shared_ptr<ast::type::TypeBuilderMember> Parser::type_builder_member() {
        const auto name = expect(TokenType::IDENTIFIER, TokenType::INIT);
        std::shared_ptr<ast::Type> m_type;
        if (match(TokenType::COLON))
            m_type = type();
        return std::make_shared<ast::type::TypeBuilderMember>(name, m_type);
    }

    std::vector<std::shared_ptr<ast::Type>> Parser::type_list() {
        std::vector<std::shared_ptr<ast::Type>> list;
        list.push_back(type());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::type)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::Expression>> Parser::assignee_list() {
        std::vector<std::shared_ptr<ast::Expression>> list;
        list.push_back(postfix());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::postfix)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::Expression>> Parser::expr_list() {
        std::vector<std::shared_ptr<ast::Expression>> list;
        list.push_back(expression());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::expression)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::expr::Argument>> Parser::argument_list() {
        std::vector<std::shared_ptr<ast::expr::Argument>> list;
        list.push_back(argument());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::argument)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::expr::Slice>> Parser::slice_list() {
        std::vector<std::shared_ptr<ast::expr::Slice>> list;
        list.push_back(slice());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::slice)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::Reference>> Parser::reference_list() {
        std::vector<std::shared_ptr<ast::Reference>> list;
        list.push_back(reference());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::reference)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::decl::Param>> Parser::param_list() {
        std::vector<std::shared_ptr<ast::decl::Param>> list;
        list.push_back(rule_optional(&Parser::param));
        if (list.front() == null)
            return {};
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::param)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::decl::TypeParam>> Parser::type_param_list() {
        std::vector<std::shared_ptr<ast::decl::TypeParam>> list;
        list.push_back(type_param());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::type_param)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::decl::Constraint>> Parser::constraint_list() {
        std::vector<std::shared_ptr<ast::decl::Constraint>> list;
        list.push_back(constraint());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::constraint)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::decl::Parent>> Parser::parent_list() {
        std::vector<std::shared_ptr<ast::decl::Parent>> list;
        list.push_back(parent());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::parent)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::decl::Enumerator>> Parser::enumerator_list() {
        std::vector<std::shared_ptr<ast::decl::Enumerator>> list;
        list.push_back(enumerator());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::enumerator)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::type::TypeBuilderMember>> Parser::type_builder_member_list() {
        std::vector<std::shared_ptr<ast::type::TypeBuilderMember>> list;
        list.push_back(type_builder_member());
        while (match(TokenType::COMMA)) {
            if (const auto item = rule_optional(&Parser::type_builder_member)) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::shared_ptr<ast::Module> Parser::parse() {
        const auto mod = module();
        index = 0;
        return mod;
    }
}    // namespace spadec