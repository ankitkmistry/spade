#include "analyzer.hpp"

namespace spade
{
    void Analyzer::visit(ast::expr::Constant &node) {
        _res_expr_info.reset();
        switch (node.get_token()->get_type()) {
            case TokenType::TRUE:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_BOOL]);
                break;
            case TokenType::FALSE:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_BOOL]);
                break;
            case TokenType::NULL_:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                _res_expr_info.type_info.b_nullable = true;
                _res_expr_info.type_info.b_null = true;
                break;
            case TokenType::INTEGER:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_INT]);
                break;
            case TokenType::FLOAT:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_FLOAT]);
                break;
            case TokenType::STRING:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_STRING]);
                break;
            case TokenType::IDENTIFIER: {
                // Find the scope where name is located
                auto scope = find_name(node.get_token()->get_text());
                // Yell if the scope cannot be located
                if (!scope)
                    throw error("undefined reference", &node);
                // Now set the expr info accordingly
                switch (scope->get_type()) {
                    case scope::ScopeType::FOLDER_MODULE:
                    case scope::ScopeType::MODULE:
                        _res_expr_info.tag = ExprInfo::Type::MODULE;
                        _res_expr_info.module = cast<scope::Module>(&*scope);
                        break;
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*scope);
                        break;
                    case scope::ScopeType::FUNCTION:
                        throw Unreachable();    // surely some symbol tree builder error
                    case scope::ScopeType::FUNCTION_SET:
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                        _res_expr_info.function_set = cast<scope::FunctionSet>(&*scope);
                        break;
                    case scope::ScopeType::BLOCK:
                        throw Unreachable();    // surely some parser error
                    case scope::ScopeType::VARIABLE: {
                        _res_expr_info = get_var_expr_info(cast<scope::Variable>(scope), node);
                        break;
                    }
                    case scope::ScopeType::ENUMERATOR:
                        _res_expr_info.tag = ExprInfo::Type::NORMAL;
                        _res_expr_info.type_info.type = scope->get_enclosing_compound();
                        break;
                }
                break;
            }
            default:
                throw Unreachable();    // surely some parser error
        }
    }

    void Analyzer::visit(ast::expr::Super &node) {
        _res_expr_info.reset();

        if (get_parent_scope()->get_type() == scope::ScopeType::COMPOUND &&
            (get_current_scope()->get_type() == scope::ScopeType::FUNCTION) /* ||
             get_current_scope()->get_type() == scope::ScopeType::VARIABLE ||
             get_current_scope()->get_type() == scope::ScopeType::ENUMERATOR) */) {
            auto klass = cast<scope::Compound>(get_parent_scope());
            if (auto reference = node.get_reference()) {
                reference->accept(this);
                if (!klass->has_super(_res_type_info.type))
                    throw error("invalid super class", &node);
                _res_expr_info.type_info.type = _res_type_info.type;
                return;
            } else {
                for (const auto &parent: klass->get_supers()) {
                    if (parent->get_compound_node()->get_token()->get_type() == TokenType::CLASS) {
                        _res_expr_info.type_info.type = parent;
                        return;
                    }
                }
                throw error("cannot deduce super class", &node);
            }
        } else
            throw error("super is only allowed in class level functions and constructors only", &node);
    }

    void Analyzer::visit(ast::expr::Self &node) {
        _res_expr_info.reset();

        if (get_parent_scope()->get_type() == scope::ScopeType::COMPOUND &&
            (get_current_scope()->get_type() == scope::ScopeType::FUNCTION ||
             get_current_scope()->get_type() == scope::ScopeType::VARIABLE ||
             get_current_scope()->get_type() == scope::ScopeType::ENUMERATOR)) {
            _res_expr_info.type_info.type = cast<scope::Compound>(&*get_parent_scope());
        } else {
            throw error("self is only allowed in class level declarations only", &node);
        }
    }

    void Analyzer::visit(ast::expr::DotAccess &node) {
        node.get_caller()->accept(this);
        auto caller_info = _res_expr_info;
        _res_expr_info.reset();
        switch (caller_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (caller_info.is_null())
                    throw error("cannot access 'null'", &node);
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("cannot access member of nullable type", &node))
                            .note(error("use safe dot access operator '?.'", &node));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("cannot use safe dot access operator on non-nullable type", &node))
                            .note(error("remove the safe dot access operator '?.'", &node));
                }
                if (!caller_info.type_info.type->has_variable(node.get_member()->get_text())) {
                    throw error(std::format("cannot access member: '{}'", node.get_member()->get_text()), &node);
                }
                auto member_scope = caller_info.type_info.type->get_variable(node.get_member()->get_text());
                resolve_context(member_scope, node);
                switch (member_scope->get_type()) {
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*member_scope);
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        break;
                    case scope::ScopeType::FUNCTION:
                        throw Unreachable();    // surely some symbol tree builder error
                    case scope::ScopeType::FUNCTION_SET:
                        _res_expr_info.function_set = cast<scope::FunctionSet>(&*member_scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                        break;
                    case scope::ScopeType::VARIABLE:
                        _res_expr_info = get_var_expr_info(cast<scope::Variable>(member_scope), node);
                        break;
                    case scope::ScopeType::ENUMERATOR:
                        throw error("cannot access enumerator from an object (you should use the type)", &node);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
                break;
            }
            case ExprInfo::Type::STATIC: {
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("cannot access member of nullable type", &node))
                            .note(error("use safe dot access operator '?.'", &node));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("cannot use safe dot access operator on non-nullable type", &node))
                            .note(error("remove the safe dot access operator '?.'", &node));
                }
                if (!caller_info.type_info.type->has_variable(node.get_member()->get_text()))
                    throw error(std::format("cannot access member: '{}'", node.get_member()->get_text()), &node);
                auto member_scope = caller_info.type_info.type->get_variable(node.get_member()->get_text());
                resolve_context(member_scope, node);
                switch (member_scope->get_type()) {
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*member_scope);
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        break;
                    case scope::ScopeType::FUNCTION:
                        throw Unreachable();    // surely some symbol tree builder error
                    case scope::ScopeType::FUNCTION_SET:
                        _res_expr_info.function_set = cast<scope::FunctionSet>(&*member_scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                        break;
                    case scope::ScopeType::VARIABLE: {
                        auto var_scope = cast<scope::Variable>(member_scope);
                        if (!var_scope->is_static()) {
                            throw ErrorGroup<AnalyzerError>()
                                    .error(error(std::format("cannot access non-static variable '{}' of '{}'",
                                                             var_scope->to_string(), caller_info.to_string()),
                                                 &node))
                                    .note(error("declared here", var_scope));
                        }
                        _res_expr_info = get_var_expr_info(var_scope, node);
                        break;
                    }
                    case scope::ScopeType::ENUMERATOR:
                        _res_expr_info.type_info.type = caller_info.type_info.type;
                        _res_expr_info.tag = ExprInfo::Type::NORMAL;
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
                break;
            }
            case ExprInfo::Type::MODULE: {
                if (!caller_info.module->has_variable(node.get_member()->get_text())) {
                    throw error(std::format("cannot access member: '{}'", node.get_member()->get_text()), &node);
                }
                auto scope = caller_info.module->get_variable(node.get_member()->get_text());
                switch (scope->get_type()) {
                    case scope::ScopeType::FOLDER_MODULE:
                    case scope::ScopeType::MODULE:
                        _res_expr_info.module = cast<scope::Module>(&*scope);
                        _res_expr_info.tag = ExprInfo::Type::MODULE;
                        break;
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*scope);
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        break;
                    case scope::ScopeType::FUNCTION:
                        throw Unreachable();    // surely some symbol tree builder error
                    case scope::ScopeType::FUNCTION_SET:
                        _res_expr_info.function_set = cast<scope::FunctionSet>(&*scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                        break;
                    case scope::ScopeType::VARIABLE:
                        _res_expr_info.type_info = cast<scope::Variable>(scope)->get_type_info();
                        _res_expr_info.tag = ExprInfo::Type::NORMAL;
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
                break;
            }
            case ExprInfo::Type::FUNCTION_SET:
                throw error("cannot access member of callable type", &node);
        }
        // This is the property of safe dot operator
        // where 'a?.b' returns 'a.b' if 'a' is not null, else returns null
        if (node.get_safe() && (_res_expr_info.tag == ExprInfo::Type::NORMAL || _res_expr_info.tag == ExprInfo::Type::STATIC))
            _res_expr_info.type_info.b_nullable = true;
    }

    void Analyzer::visit(ast::expr::Call &node) {
        node.get_caller()->accept(this);
        auto caller_info = _res_expr_info;
        _res_expr_info.reset();

        std::vector<ArgInfo> arg_infos;
        arg_infos.reserve(node.get_args().size());

        for (auto arg: node.get_args()) {
            arg->accept(this);
            if (!arg_infos.empty() && arg_infos.back().b_kwd && !_res_arg_info.b_kwd)
                throw error("mixing non-keyword and keyword arguments is not allowed", arg);
            arg_infos.push_back(_res_arg_info);
        }

        switch (caller_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (caller_info.is_null())
                    throw error("null is not callable", &node);
                // check for call operator
                auto fun_set = caller_info.type_info.type->get_variable("__call__");
                if (fun_set) {
                    _res_expr_info.reset();
                    _res_expr_info = resolve_call(&*cast<scope::FunctionSet>(fun_set), arg_infos, node);
                } else {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("'{}' does not provide a constructor", caller_info.to_string()), &node))
                            .note(error("declared here", caller_info.type_info.type));
                }
                break;
            }
            case ExprInfo::Type::STATIC: {
                // check for constructor
                auto fun_set = caller_info.type_info.type->get_variable("init");
                if (fun_set) {
                    _res_expr_info.reset();
                    _res_expr_info = resolve_call(&*cast<scope::FunctionSet>(fun_set), arg_infos, node);
                } else {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("'{}' does not provide a constructor", caller_info.to_string()), &node))
                            .note(error("declared here", caller_info.type_info.type));
                }
                break;
            }
            case ExprInfo::Type::MODULE:
                throw error("module is not callable", &node);
            case ExprInfo::Type::FUNCTION_SET:
                // this is the actual thing: FUNCTION RESOLUTION
                _res_expr_info.reset();
                _res_expr_info = resolve_call(caller_info.function_set, arg_infos, node);
                break;
        }
    }

    void Analyzer::visit(ast::expr::Argument &node) {
        ArgInfo arg_info;
        arg_info.b_kwd = node.get_name() != null;
        arg_info.name = arg_info.b_kwd ? node.get_name()->get_text() : "";
        node.get_expr()->accept(this);
        arg_info.expr_info = _res_expr_info;

        _res_arg_info.reset();
        _res_arg_info = arg_info;
    }

    void Analyzer::visit(ast::expr::Reify &node) {
        node.get_caller()->accept(this);
        _res_expr_info.reset();
        // TODO: implement reify
    }

    void Analyzer::visit(ast::expr::Index &node) {
        node.get_caller()->accept(this);
        _res_expr_info.reset();
    }

    void Analyzer::visit(ast::expr::Slice &node) {
        // TODO: implement slices
    }

    void Analyzer::visit(ast::expr::Unary &node) {
        node.get_expr()->accept(this);
        auto expr_info = _res_expr_info;
        switch (expr_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (expr_info.is_null())
                    throw error(std::format("cannot apply unary operator '{}' on 'null'", node.get_op()->get_text()), &node);
                auto type_info = expr_info.type_info;
                if (type_info.b_nullable) {
                    throw error(std::format("cannot apply unary operator '{}' on nullable type '{}'", node.get_op()->get_text(),
                                            type_info.type->to_string()),
                                &node);
                }
                _res_expr_info.reset();
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                switch (node.get_op()->get_type()) {
                    case TokenType::NOT:
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_BOOL]);
                        break;
                    case TokenType::TILDE: {
                        if (type_info.type == &*internals[Internal::SPADE_INT]) {
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_INT]);
                        } else {
                            // Check for overloaded operator ~
                            throw error(std::format("cannot apply unary operator '~' on '{}'", type_info.type->to_string()),
                                        &node);
                        }
                        break;
                    }
                    case TokenType::DASH: {
                        if (type_info.type == &*internals[Internal::SPADE_INT]) {
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_INT]);
                        } else if (type_info.type == &*internals[Internal::SPADE_FLOAT]) {
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_FLOAT]);
                        } else {
                            // Check for overloaded operator -
                            throw error(std::format("cannot apply unary operator '-' on '{}'", type_info.type->to_string()),
                                        &node);
                        }
                        break;
                    }
                    case TokenType::PLUS: {
                        if (type_info.type == &*internals[Internal::SPADE_INT]) {
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_INT]);
                        } else if (type_info.type == &*internals[Internal::SPADE_FLOAT]) {
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_FLOAT]);
                        } else {
                            // Check for overloaded operator +
                            throw error(std::format("cannot apply unary operator '+' on '{}'", type_info.type->to_string()),
                                        &node);
                        }
                        break;
                    }
                    default:
                        throw Unreachable();    // surely some parser error
                }
                break;
            }
            case ExprInfo::Type::STATIC:
            case ExprInfo::Type::MODULE:
            case ExprInfo::Type::FUNCTION_SET:
                throw error(std::format("cannot apply unary operator '{}' on '{}'", node.get_op()->get_text(),
                                        expr_info.to_string()),
                            &node);
        }
    }

    void Analyzer::visit(ast::expr::Cast &node) {
        node.get_expr()->accept(this);
        auto expr_info = _res_expr_info;
        if (expr_info.tag != ExprInfo::Type::NORMAL)
            throw error(std::format("cannot cast '{}'", expr_info.to_string()), &node);
        node.get_type()->accept(this);
        auto type_cast_info = _res_type_info;
        if (type_cast_info.b_nullable)
            throw error("cast type cannot be nullable", &node);

        _res_expr_info.reset();
        _res_expr_info.tag = ExprInfo::Type::NORMAL;
        if (node.get_safe()) {
            if (expr_info.is_null())
                warning("expression is always 'null'", &node);
            else {
                check_cast(expr_info.type_info.type, type_cast_info.type, node, true);
                type_cast_info.b_nullable = true;
                _res_expr_info.type_info = type_cast_info;
            }
        } else {
            if (expr_info.is_null())
                throw error("cannot cast 'null'", &node);
            check_cast(expr_info.type_info.type, type_cast_info.type, node, false);
            _res_expr_info.type_info = type_cast_info;
        }
    }

    void Analyzer::visit(ast::expr::Binary &node) {
        string op_str = (node.get_op1() ? node.get_op1()->get_text() : "") + (node.get_op2() ? node.get_op2()->get_text() : "");

        node.get_left()->accept(this);
        auto left_expr_info = _res_expr_info;
        node.get_right()->accept(this);
        auto right_expr_info = _res_expr_info;

        if (left_expr_info.is_null() || right_expr_info.is_null())
            throw error(std::format("cannot apply binary operator '{}' on 'null'", op_str), &node);
        switch (node.get_op1()->get_type()) {
            case TokenType::STAR_STAR:
                break;
            case TokenType::STAR:
                break;
            case TokenType::SLASH:
                break;
            case TokenType::PERCENT:
                break;
            case TokenType::PLUS:
                break;
            case TokenType::DASH:
                break;
            case TokenType::LSHIFT:
                break;
            case TokenType::RSHIFT:
                break;
            case TokenType::URSHIFT:
                break;
            case TokenType::AMPERSAND:
                break;
            case TokenType::CARET:
                break;
            case TokenType::PIPE:
                break;
            case TokenType::IS:
                if (node.get_op2() && node.get_op2()->get_type() == TokenType::NOT) {
                } else {
                }
                break;
            case TokenType::NOT:
                if (node.get_op2() && node.get_op2()->get_type() == TokenType::IN) {
                } else
                    throw Unreachable();    // surely some parser error
                break;
            case TokenType::IN:
                break;
            case TokenType::AND:
                break;
            case TokenType::OR:
                break;
            default:
                throw Unreachable();
        }
    }

    void Analyzer::visit(ast::expr::ChainBinary &node) {
        std::shared_ptr<ast::Expression> prev_expr;
        size_t i = 0;
        for (const auto &cur_expr: node.get_exprs()) {
            cur_expr->accept(this);
            if (prev_expr) {
                switch (node.get_ops()[i - 1]->get_type()) {
                    case TokenType::LT:
                        break;
                    case TokenType::LE:
                        break;
                    case TokenType::EQ:
                        break;
                    case TokenType::NE:
                        break;
                    case TokenType::GE:
                        break;
                    case TokenType::GT:
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
            prev_expr = cur_expr;
            i++;
        }
    }

    void Analyzer::visit(ast::expr::Ternary &node) {
        node.get_condition()->accept(this);
        node.get_on_true()->accept(this);
        auto expr_info1 = _res_expr_info;
        node.get_on_false()->accept(this);
        auto expr_info2 = _res_expr_info;
        // TODO: check for type equality
    }

    void Analyzer::visit(ast::expr::Assignment &node) {}
}    // namespace spade