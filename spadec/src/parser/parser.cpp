#include "parser.hpp"
#include "lexer/token.hpp"

namespace spade
{
    void Parser::fill_tokens_buffer(int n) {
        for (int i = 0; i < n; ++i) {
            auto token = lexer->next_token();
            if (token->get_type() == TokenType::END_OF_FILE && !tokens.empty() &&
                tokens.back()->get_type() == TokenType::END_OF_FILE)
                break;    // Already EOF occured
            tokens.push_back(token);
        }
    }

    std::shared_ptr<Token> Parser::current() {
        if (index == 0)
            return null;
        if (index >= tokens.size()) {
            fill_tokens_buffer(FILL_CONSTANT);
            if (index >= tokens.size())
                return tokens.back();    // EOF
        }
        return tokens[index - 1];
    }

    std::shared_ptr<Token> Parser::peek(int i) {
        auto idx = index + i;
        if (idx >= tokens.size()) {
            fill_tokens_buffer(FILL_CONSTANT);
            if (idx >= tokens.size())
                return tokens.back();    // EOF
        }
        return tokens[idx];
    }

    std::shared_ptr<Token> Parser::advance() {
        if (index >= tokens.size()) {
            fill_tokens_buffer(FILL_CONSTANT);
            if (index >= tokens.size())
                return tokens.back();    // EOF
        }
        return tokens[index++];
    }

    ParserError Parser::error(const string &msg, const std::shared_ptr<Token> &token) {
        int line_start = token->get_line();
        int col_start = token->get_col();
        int line_end = line_start;
        int col_end = col_start;
        for (auto c: token->get_text()) {
            if (c == '\n') {
                line_end++;
                col_end = 0;
            } else
                col_end++;
        }
        return {msg, file_path, line_start, col_start, line_end, col_end};
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
        auto start = expect(TokenType::IMPORT);
        std::stringstream path;
        if (match(TokenType::DOT)) {
            path << '.';
            if (match(TokenType::DOT))
                path << '.';
        }
        auto ref = reference();
        for (size_t i = 0; auto tok: ref->get_path()) {
            path << tok->get_text();
            if (i < ref->get_path().size() - 1)
                path << ".";
            i++;
        }
        std::shared_ptr<Token> alias;
        if (match(TokenType::AS))
            alias = expect(TokenType::IDENTIFIER);
        else if (match(TokenType::DOT)) {
            // TODO: implement this in analyzer
            expect(TokenType::STAR);
            path << ".*";
        }
        return std::make_shared<ast::Import>(start, current(), path.str(), ref->get_path().back(), alias);
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
        auto mods = modifiers();
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
                throw error(std::format("expected {}",
                                        make_expected_string(TokenType::VAR, TokenType::CONST, TokenType::FUN, TokenType::CLASS,
                                                             TokenType::INTERFACE, TokenType::ENUM, TokenType::ANNOTATION)));
        }
        decl->set_modifiers(mods);
        return decl;
    }

    std::shared_ptr<ast::Declaration> Parser::compound_decl() {
        auto token = expect(TokenType::CLASS, TokenType::ENUM, TokenType::INTERFACE, TokenType::ANNOTATION);
        auto name = expect(TokenType::IDENTIFIER);
        std::vector<std::shared_ptr<ast::decl::TypeParam>> type_params;
        std::vector<std::shared_ptr<ast::decl::Constraint>> constraints;
        if (match(TokenType::LBRACKET)) {
            if (peek()->get_type() != TokenType::RBRACKET)
                type_params = type_param_list();
            expect(TokenType::RBRACKET);
            if (match("where")) {
                constraints = constraint_list();
            }
        }
        std::vector<std::shared_ptr<ast::decl::Parent>> parents;
        if (match(TokenType::COLON)) {
            parents = parent_list();
        }
        std::vector<std::shared_ptr<ast::Declaration>> members;
        std::vector<std::shared_ptr<ast::decl::Enumerator>> enumerators;
        if (match(TokenType::LBRACE)) {
            while (peek()->get_type() == TokenType::IDENTIFIER) enumerators = enumerator_list();
            while (peek()->get_type() != TokenType::RBRACE) members.push_back(member_decl());
            expect(TokenType::RBRACE);
        }
        return std::make_shared<ast::decl::Compound>(token, current(), name, type_params, constraints, parents, enumerators,
                                                     members);
    }

    std::shared_ptr<ast::Declaration> Parser::member_decl() {
        auto mods = modifiers();
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
                decl = compound_decl();
                break;
            default:
                throw error(std::format("expected {}",
                                        make_expected_string(TokenType::VAR, TokenType::CONST, TokenType::FUN, TokenType::INIT,
                                                             TokenType::CLASS, TokenType::INTERFACE, TokenType::ENUM)));
        }
        decl->set_modifiers(mods);
        return decl;
    }

    std::shared_ptr<ast::Declaration> Parser::init_decl() {
        auto name = expect(TokenType::INIT);
        expect(TokenType::LPAREN);
        std::shared_ptr<ast::decl::Params> init_params;
        if (peek()->get_type() != TokenType::RPAREN)
            init_params = params();
        expect(TokenType::RPAREN);
        std::shared_ptr<ast::Statement> def;
        if (match(TokenType::EQUAL))
            def = statement();
        else if (peek()->get_type() == TokenType::LBRACE)
            def = block();
        else
            throw error(std::format("expected {}", make_expected_string(TokenType::EQUAL, TokenType::LBRACE)));
        return std::make_shared<ast::decl::Init>(name, init_params, def);
    }

    std::shared_ptr<ast::Declaration> Parser::variable_decl() {
        auto token = expect(TokenType::VAR, TokenType::CONST);
        auto name = expect(TokenType::IDENTIFIER);
        std::shared_ptr<ast::Type> var_type;
        if (match(TokenType::COLON))
            var_type = type();
        std::shared_ptr<ast::Expression> expr;
        if (match(TokenType::EQUAL))
            expr = expression();
        return std::make_shared<ast::decl::Variable>(token, current(), name, var_type, expr);
    }

    std::shared_ptr<ast::Declaration> Parser::function_decl() {
        auto token = expect(TokenType::FUN);
        auto name = expect(TokenType::IDENTIFIER, TokenType::UNDERSCORE);
        std::vector<std::shared_ptr<ast::decl::TypeParam>> type_params;
        std::vector<std::shared_ptr<ast::decl::Constraint>> constraints;
        if (match(TokenType::LBRACKET)) {
            if (peek()->get_type() != TokenType::RBRACKET)
                type_params = type_param_list();
            expect(TokenType::RBRACKET);
            if (match("where")) {
                constraints = constraint_list();
            }
        }
        expect(TokenType::LPAREN);
        std::shared_ptr<ast::decl::Params> fun_params;
        if (peek()->get_type() != TokenType::RPAREN)
            fun_params = params();
        expect(TokenType::RPAREN);
        std::shared_ptr<ast::Type> ret_type;
        if (match(TokenType::ARROW))
            ret_type = type();
        std::shared_ptr<ast::Statement> def;
        if (match(TokenType::EQUAL))
            def = statement();
        else if (peek()->get_type() == TokenType::LBRACE)
            def = block();
        return std::make_shared<ast::decl::Function>(token, current(), name, type_params, constraints, fun_params, ret_type,
                                                     def);
    }

    std::vector<std::shared_ptr<Token>> Parser::modifiers() {
        std::vector<std::shared_ptr<Token>> mods;
        while (match(TokenType::ABSTRACT, TokenType::FINAL, TokenType::STATIC, TokenType::OVERRIDE, TokenType::PRIVATE,
                     TokenType::INTERNAL, TokenType::PROTECTED, TokenType::PUBLIC)) {
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
        auto name = expect(TokenType::IDENTIFIER);
        std::shared_ptr<ast::Type> default_type;
        if (match(TokenType::EQUAL))
            default_type = type();
        return std::make_shared<ast::decl::TypeParam>(variance, current(), name, default_type);
    }

    std::shared_ptr<ast::decl::Constraint> Parser::constraint() {
        auto arg = expect(TokenType::IDENTIFIER);
        expect(TokenType::COLON);
        auto c_type = type();
        return std::make_shared<ast::decl::Constraint>(arg, c_type);
    }

    std::shared_ptr<ast::decl::Parent> Parser::parent() {
        auto ref = reference();
        std::vector<std::shared_ptr<ast::Type>> type_args;
        if (match(TokenType::LBRACKET)) {
            type_args = type_list();
            expect(TokenType::RBRACKET);
        }
        return std::make_shared<ast::decl::Parent>(current(), ref, type_args);
    }

    std::shared_ptr<ast::decl::Enumerator> Parser::enumerator() {
        auto name = expect(TokenType::IDENTIFIER);
        if (match(TokenType::EQUAL)) {
            auto expr = expression();
            return std::make_shared<ast::decl::Enumerator>(name, expr);
        }
        if (match(TokenType::LPAREN)) {
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
        if (current()->get_type() == TokenType::COMMA && match(TokenType::STAR)) {
            expect(TokenType::COMMA);
            param_list2 = param_list();
        }
        if (current()->get_type() == TokenType::COMMA && match(TokenType::SLASH)) {
            expect(TokenType::COMMA);
            param_list3 = param_list();
        }
        if (param_list2.empty() && param_list3.empty())
            return std::make_shared<ast::decl::Params>(param_list1.front(), current(), EMPTY, param_list1, EMPTY);
        if (param_list2.empty())
            return std::make_shared<ast::decl::Params>(param_list1.front(), current(), EMPTY, param_list1, param_list3);
        if (param_list3.empty())
            return std::make_shared<ast::decl::Params>(param_list1.front(), current(), param_list1, param_list2, EMPTY);
        return std::make_shared<ast::decl::Params>(param_list1.front(), current(), param_list1, param_list2, param_list3);
    }

    std::shared_ptr<ast::decl::Param> Parser::param() {
        auto is_const = match(TokenType::CONST);
        auto variadic = match(TokenType::STAR);
        auto name = expect(TokenType::IDENTIFIER);
        std::shared_ptr<ast::Type> param_type;
        std::shared_ptr<ast::Expression> expr;
        if (match(TokenType::COLON))
            param_type = type();
        if (match(TokenType::EQUAL))
            expr = expression();
        int line_start, col_start, line_end, col_end;
        if (is_const) {
            line_start = is_const->get_line_start();
            col_start = is_const->get_col_start();
        } else if (variadic) {
            line_start = variadic->get_line_start();
            col_start = variadic->get_col_start();
        } else {
            line_start = name->get_line_start();
            col_start = name->get_col_start();
        }
        if (expr) {
            line_end = expr->get_line_end();
            col_end = expr->get_col_end();
        } else if (param_type) {
            line_end = param_type->get_line_end();
            col_end = param_type->get_col_end();
        } else {
            line_end = name->get_line_end();
            col_end = name->get_col_end();
        }
        return std::make_shared<ast::decl::Param>(line_start, line_end, col_start, col_end, is_const, variadic, name,
                                                  param_type, expr);
    }

    std::shared_ptr<ast::Statement> Parser::statements() {
        if (peek()->get_type() == TokenType::LBRACE)
            return block();
        return statement();
    }

    std::shared_ptr<ast::Statement> Parser::block() {
        auto start = expect(TokenType::LBRACE);
        std::vector<std::shared_ptr<ast::Statement>> stmts;
        while (peek()->get_type() != TokenType::RBRACE) {
            switch (peek()->get_type()) {
                case TokenType::LBRACE:
                    stmts.push_back(block());
                    break;
                case TokenType::VAR:
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
        auto end = expect(TokenType::RBRACE);
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
                auto token = advance();
                auto expr = expression();
                return std::make_shared<ast::stmt::Throw>(token, expr);
            }
            case TokenType::RETURN: {
                auto token = advance();
                auto expr = expression();
                return std::make_shared<ast::stmt::Return>(token, expr);
            }
            case TokenType::YIELD: {
                auto token = advance();
                auto expr = expression();
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

#define BODY() (match(TokenType::COLON) ? statement() : block())

    std::shared_ptr<ast::Statement> Parser::if_stmt() {
        auto token = expect(TokenType::IF);
        auto expr = expression();
        auto body = BODY();
        auto else_body = match(TokenType::ELSE) ? BODY() : null;
        return std::make_shared<ast::stmt::If>(token, expr, body, else_body);
    }

    std::shared_ptr<ast::Statement> Parser::while_stmt() {
        auto token = expect(TokenType::WHILE);
        auto expr = expression();
        auto body = BODY();
        auto else_body = match(TokenType::ELSE) ? BODY() : null;
        return std::make_shared<ast::stmt::While>(token, expr, body, else_body);
    }

    std::shared_ptr<ast::Statement> Parser::do_while_stmt() {
        auto token = expect(TokenType::DO);
        auto body = block();
        expect(TokenType::WHILE);
        auto expr = expression();
        auto else_body = match(TokenType::ELSE) ? BODY() : null;
        return std::make_shared<ast::stmt::DoWhile>(token, body, expr, else_body);
    }

    std::shared_ptr<ast::Statement> Parser::try_stmt() {
        auto token = expect(TokenType::TRY);
        auto body = BODY();
        std::shared_ptr<ast::Statement> finally;
        std::vector<std::shared_ptr<ast::Statement>> catches;
        if (match(TokenType::FINALLY))
            finally = BODY();
        else {
            do {
                catches.push_back(catch_stmt());
            } while (peek()->get_type() == TokenType::CATCH);
            if (match(TokenType::FINALLY))
                finally = BODY();
        }
        return std::make_shared<ast::stmt::Try>(token, body, catches, finally);
    }

    std::shared_ptr<ast::Statement> Parser::catch_stmt() {
        auto token = expect(TokenType::CATCH);
        auto refs = reference_list();
        std::shared_ptr<Token> symbol;
        if (match(TokenType::AS))
            symbol = expect(TokenType::IDENTIFIER);
        auto body = BODY();
        return std::make_shared<ast::stmt::Catch>(token, refs, symbol, body);
    }

    std::shared_ptr<ast::Expression> Parser::expression() {
        return rule_or<ast::Expression>([&] { return assignment(); }, [&] { return ternary(); });
    }

    std::shared_ptr<ast::Expression> Parser::assignment() {
        auto assignees = assignee_list();
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
                auto exprs = expr_list();
                return std::make_shared<ast::expr::Assignment>(assignees, op1, op2, exprs);
            }
            default:
                throw error(std::format("expected {}",
                                        make_expected_string(TokenType::PLUS, TokenType::DASH, TokenType::STAR,
                                                             TokenType::SLASH, TokenType::PERCENT, TokenType::STAR_STAR,
                                                             TokenType::LSHIFT, TokenType::RSHIFT, TokenType::URSHIFT,
                                                             TokenType::AMPERSAND, TokenType::PIPE, TokenType::CARET,
                                                             TokenType::ELVIS, TokenType::EQUAL)));
        }
    }

    std::shared_ptr<ast::Expression> Parser::ternary() {
        auto expr1 = or_();
        if (match(TokenType::IF)) {
            auto expr2 = or_();
            expect(TokenType::ELSE);
            auto expr3 = or_();
            return std::make_shared<ast::expr::Ternary>(expr2, expr1, expr3);
        }
        return expr1;
    }

    std::shared_ptr<ast::Expression> Parser::or_() {
        auto left = and_();
        while (match(TokenType::OR)) {
            auto op = current();
            auto right = and_();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::and_() {
        auto left = not_();
        while (match(TokenType::AND)) {
            auto op = current();
            auto right = not_();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::not_() {
        if (match(TokenType::NOT)) {
            auto op = current();
            auto expr = not_();
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
            auto right = relational();
            left = std::make_shared<ast::expr::Binary>(left, op, op_extra, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::relational() {
        std::vector<std::shared_ptr<ast::Expression>> exprs;
        std::vector<std::shared_ptr<Token>> ops;
        auto expr = bit_or();
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
            auto op = current();
            auto right = bit_xor();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::bit_xor() {
        auto left = bit_and();
        while (match(TokenType::CARET)) {
            auto op = current();
            auto right = bit_and();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::bit_and() {
        auto left = shift();
        while (match(TokenType::AMPERSAND)) {
            auto op = current();
            auto right = shift();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::shift() {
        auto left = term();
        while (match(TokenType::LSHIFT) || match(TokenType::RSHIFT) || match(TokenType::URSHIFT)) {
            auto op = current();
            auto right = term();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::term() {
        auto left = factor();
        while (match(TokenType::PLUS) || match(TokenType::DASH)) {
            auto op = current();
            auto right = factor();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::factor() {
        auto left = power();
        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT)) {
            auto op = current();
            auto right = power();
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
        std::shared_ptr<ast::Expression> expr = exprs.back();
        for (int i = static_cast<int>(ops.size()) - 1; i >= 0; i--) {
            expr = std::make_shared<ast::expr::Binary>(exprs[i], ops[i], expr);
        }
        return expr;
    }

    std::shared_ptr<ast::Expression> Parser::cast() {
        auto expr = elvis();
        while (match(TokenType::AS)) {
            auto safe = match(TokenType::HOOK);
            auto cast_type = type();
            expr = std::make_shared<ast::expr::Cast>(expr, safe, cast_type);
        }
        return expr;
    }

    std::shared_ptr<ast::Expression> Parser::elvis() {
        auto left = unary();
        while (match(TokenType::ELVIS)) {
            auto op = current();
            auto right = unary();
            left = std::make_shared<ast::expr::Binary>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Expression> Parser::unary() {
        switch (peek()->get_type()) {
            case TokenType::TILDE:
            case TokenType::DASH:
            case TokenType::PLUS: {
                auto op = advance();
                auto expr = unary();
                return std::make_shared<ast::expr::Unary>(op, expr);
            }
            default:
                return postfix();
        }
    }

    std::shared_ptr<ast::Expression> Parser::postfix() {
        auto caller = primary();
        std::shared_ptr<Token> safe = null;
        while (true) {
            switch (peek()->get_type()) {
                case TokenType::HOOK:
                    safe = advance();
                    [[fallthrough]];
                case TokenType::DOT: {
                    expect(TokenType::DOT);
                    auto member = expect(TokenType::IDENTIFIER, TokenType::INIT);
                    caller = std::make_shared<ast::expr::DotAccess>(caller, member, safe);
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
                    caller = std::make_shared<ast::expr::Call>(end, caller, args);
                    break;
                }
                case TokenType::LBRACKET: {
                    advance();
                    caller = rule_or<ast::Expression, ast::expr::Index, ast::expr::Reify>(
                            [&] {
                                auto slices = slice_list();
                                auto end = expect(TokenType::RBRACKET);
                                return std::make_shared<ast::expr::Index>(end, caller, slices);
                            },
                            [&] {
                                auto type_args = type_list();
                                auto end = expect(TokenType::RBRACKET);
                                return std::make_shared<ast::expr::Reify>(end, caller, type_args);
                            });
                    break;
                }
                default:
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
        auto expr = expression();
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
            to = rule_optional<ast::Expression>([&] { return expression(); });
        std::shared_ptr<Token> c2 = match(TokenType::COLON);
        if (c2)
            step = rule_optional<ast::Expression>([&] { return expression(); });

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
                return std::make_shared<ast::expr::Constant>(advance());

            case TokenType::SUPER: {
                std::shared_ptr<Token> end;
                std::shared_ptr<Token> start = end = advance();
                if (match(TokenType::LBRACKET)) {
                    auto ref = reference();
                    end = expect(TokenType::RBRACKET);
                    return std::make_shared<ast::expr::Super>(start, end, ref);
                }
                return std::make_shared<ast::expr::Super>(start, end, null);
            }
            case TokenType::SELF:
                return std::make_shared<ast::expr::Self>(advance());
            case TokenType::LPAREN: {
                advance();
                auto expr = expression();
                expect(TokenType::RPAREN);
                return expr;
            }
            default:
                throw error(
                        std::format("expected {}", make_expected_string(TokenType::TRUE, TokenType::FALSE, TokenType::NULL_,
                                                                        TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING,
                                                                        TokenType::IDENTIFIER, TokenType::SUPER,
                                                                        TokenType::SELF, TokenType::LPAREN)));
        }
    }

    std::shared_ptr<ast::Type> Parser::type() {
        return union_type();
    }

    std::shared_ptr<ast::Type> Parser::union_type() {
        auto left = intersection_type();
        while (match(TokenType::PIPE)) {
            auto op = current();
            auto right = intersection_type();
            left = std::make_shared<ast::type::BinaryOp>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Type> Parser::intersection_type() {
        auto left = nullable_type();
        while (match(TokenType::AMPERSAND)) {
            auto op = current();
            auto right = nullable_type();
            left = std::make_shared<ast::type::BinaryOp>(left, op, right);
        }
        return left;
    }

    std::shared_ptr<ast::Type> Parser::nullable_type() {
        auto type = primary_type();
        if (match(TokenType::HOOK))
            return std::make_shared<ast::type::Nullable>(current(), type);
        return type;
    }

    std::shared_ptr<ast::Type> Parser::primary_type() {
        switch (peek()->get_type()) {
            case TokenType::IDENTIFIER: {
                auto ref = reference();
                if (match(TokenType::LBRACKET)) {
                    auto list = type_list();
                    auto end = expect(TokenType::RBRACKET);
                    return std::make_shared<ast::type::Reference>(end, ref, list);
                }
                return std::make_shared<ast::type::Reference>(ref);
            }
            case TokenType::TYPE:
                return std::make_shared<ast::type::TypeLiteral>(advance());
            case TokenType::LPAREN: {
                auto start = advance();
                return rule_or<ast::Type, ast::type::Function, ast::Type>(
                        [&] {
                            std::vector<std::shared_ptr<ast::Type>> params;
                            if (!match(TokenType::RPAREN)) {
                                params = type_list();
                                expect(TokenType::RPAREN);
                            }
                            expect(TokenType::ARROW);
                            auto ret_type = type();
                            return std::make_shared<ast::type::Function>(start, params, ret_type);
                        },
                        [&] {
                            auto grouped = type();
                            expect(TokenType::RPAREN);
                            return grouped;
                        });
            }
            case TokenType::OBJECT: {
                auto start = advance();
                std::vector<std::shared_ptr<ast::type::TypeBuilderMember>> members;
                if (match(TokenType::LBRACE)) {
                    if (peek()->get_type() != TokenType::RBRACE)
                        members = type_builder_member_list();
                    expect(TokenType::RBRACE);
                }
                return std::make_shared<ast::type::TypeBuilder>(start, current(), members);
            }
            default:
                throw error(std::format("expected {}", make_expected_string(TokenType::IDENTIFIER, TokenType::TYPE,
                                                                            TokenType::LPAREN, TokenType::OBJECT)));
        }
    }

    std::shared_ptr<ast::type::TypeBuilderMember> Parser::type_builder_member() {
        auto name = expect(TokenType::IDENTIFIER, TokenType::INIT);
        std::shared_ptr<ast::Type> m_type;
        if (match(TokenType::COLON))
            m_type = type();
        return std::make_shared<ast::type::TypeBuilderMember>(name, m_type);
    }

    std::vector<std::shared_ptr<ast::Type>> Parser::type_list() {
        std::vector<std::shared_ptr<ast::Type>> list;
        list.push_back(type());
        while (match(TokenType::COMMA)) {
            if (auto item = rule_optional<ast::Type>([this] { return type(); })) {
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
            if (auto item = rule_optional<ast::Expression>([this] { return postfix(); })) {
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
            if (auto item = rule_optional<ast::Expression>([this] { return expression(); })) {
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
            if (auto item = rule_optional<ast::expr::Argument>([this] { return argument(); })) {
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
            if (auto item = rule_optional<ast::expr::Slice>([this] { return slice(); })) {
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
            if (auto item = rule_optional<ast::Reference>([this] { return reference(); })) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::vector<std::shared_ptr<ast::decl::Param>> Parser::param_list() {
        std::vector<std::shared_ptr<ast::decl::Param>> list;
        list.push_back(param());
        while (match(TokenType::COMMA)) {
            if (auto item = rule_optional<ast::decl::Param>([this] { return param(); })) {
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
            if (auto item = rule_optional<ast::decl::TypeParam>([this] { return type_param(); })) {
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
            if (auto item = rule_optional<ast::decl::Constraint>([this] { return constraint(); })) {
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
            if (auto item = rule_optional<ast::decl::Parent>([this] { return parent(); })) {
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
            if (auto item = rule_optional<ast::decl::Enumerator>([this] { return enumerator(); })) {
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
            if (auto item = rule_optional<ast::type::TypeBuilderMember>([this] { return type_builder_member(); })) {
                list.push_back(item);
            } else
                break;
        }
        return list;
    }

    std::shared_ptr<ast::Module> Parser::parse() {
        auto mod = module();
        index = 0;
        return mod;
    }
}    // namespace spade