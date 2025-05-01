#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "scope.hpp"
#include "spimp/error.hpp"
#include "utils/error.hpp"

namespace spade
{
    void Analyzer::visit(ast::expr::Constant &node) {
        _res_expr_info.reset();
        switch (node.get_token()->get_type()) {
            case TokenType::TRUE:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
                break;
            case TokenType::FALSE:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
                break;
            case TokenType::NULL_:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                _res_expr_info.type_info.b_nullable = true;
                _res_expr_info.value_info.b_null = true;
                break;
            case TokenType::INTEGER:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                break;
            case TokenType::FLOAT:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                break;
            case TokenType::STRING:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_STRING);
                break;
            case TokenType::INIT:
            case TokenType::IDENTIFIER:
                _res_expr_info = resolve_name(node.get_token()->get_text(), node);
                break;
            default:
                throw Unreachable();    // surely some parser error
        }
    }

    void Analyzer::visit(ast::expr::Super &node) {
        _res_expr_info.reset();

        if (auto klass = get_current_scope()->get_enclosing_compound()) {
            if (auto reference = node.get_reference()) {
                reference->accept(this);
                if (!klass->has_super(_res_type_info.type))
                    throw error("invalid super class", &node);
                _res_expr_info.type_info.type = _res_type_info.type;
            } else {
                bool found = false;
                for (const auto &parent: klass->get_supers()) {
                    if (parent->get_compound_node()->get_token()->get_type() == TokenType::CLASS) {
                        _res_expr_info.type_info.type = parent;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    throw error("cannot deduce super class", &node);
            }
        } else
            throw error("super is only allowed in class level functions and constructors only", &node);

        _res_expr_info.value_info.b_lvalue = true;
        _res_expr_info.value_info.b_const = true;
    }

    void Analyzer::visit(ast::expr::Self &node) {
        _res_expr_info.reset();

        if (auto klass = get_current_scope()->get_enclosing_compound())
            _res_expr_info.type_info.type = cast<scope::Compound>(klass);
        else
            throw error("self is only allowed in class level declarations only", &node);

        _res_expr_info.value_info.b_lvalue = true;
        _res_expr_info.value_info.b_const = true;
        _res_expr_info.value_info.b_self = true;
    }

    void Analyzer::visit(ast::expr::DotAccess &node) {
        node.get_caller()->accept(this);
        auto caller_info = _res_expr_info;
        resolve_indexer(caller_info, true, node);
        string member_name = node.get_member()->get_text();
        _res_expr_info = get_member(caller_info, member_name, node.get_safe() != null, node);
    }

    void Analyzer::visit(ast::expr::Call &node) {
        node.get_caller()->accept(this);
        auto caller_info = _res_expr_info;
        resolve_indexer(caller_info, true, node);

        std::vector<ArgInfo> arg_infos;
        arg_infos.reserve(node.get_args().size());
        for (auto arg: node.get_args()) {
            arg->accept(this);
            if (!arg_infos.empty() && arg_infos.back().b_kwd && !_res_arg_info.b_kwd)
                throw error("mixing non-keyword and keyword arguments is not allowed", arg);
            arg_infos.push_back(_res_arg_info);
        }

        _res_expr_info.reset();
        switch (caller_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (caller_info.is_null())
                    throw error("null is not callable", &node);
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot call nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe call operator '?()'", &node));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe call operator on non-nullable '{}'", caller_info.to_string()), &node))
                            .note(error("remove the safe call operator '?()'", &node));
                }
                if (caller_info.type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    // also supports self(...) syntax
                    // check for call operator
                    auto member = get_member(caller_info, OV_OP_CALL, node.get_safe() != null, node);
                    switch (member.tag) {
                        case ExprInfo::Type::NORMAL:
                        case ExprInfo::Type::STATIC:
                        case ExprInfo::Type::MODULE:
                            throw error(std::format("object of '{}' is not callable", caller_info.to_string()), &node);
                        case ExprInfo::Type::FUNCTION_SET: {
                            _res_expr_info = resolve_call(member.functions, arg_infos, node);
                            break;
                        }
                    }
                }
                break;
            }
            case ExprInfo::Type::STATIC: {
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot call nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe call operator '?()'", &node));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe call operator on non-nullable '{}'", caller_info.to_string()), &node))
                            .note(error("remove the safe call operator '?()'", &node));
                }
                if (caller_info.type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    // check for constructor
                    auto member = get_member(caller_info, "init", node.get_safe() != null, node);
                    switch (member.tag) {
                        case ExprInfo::Type::NORMAL:
                        case ExprInfo::Type::STATIC:
                        case ExprInfo::Type::MODULE:
                            throw ErrorGroup<AnalyzerError>()
                                    .error(error(std::format("'{}' does not provide a constructor", caller_info.to_string()), &node))
                                    .note(error("declared here", caller_info.type_info.type));
                        case ExprInfo::Type::FUNCTION_SET: {
                            _res_expr_info = resolve_call(member.functions, arg_infos, node);
                            break;
                        }
                    }
                }
                break;
            }
            case ExprInfo::Type::MODULE:
                throw error("module is not callable", &node);
            case ExprInfo::Type::FUNCTION_SET:
                if (caller_info.functions.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot call nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe call operator '?()'", &node));
                }
                if (!caller_info.functions.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe call operator on non-nullable '{}'", caller_info.to_string()), &node))
                            .note(error("remove the safe call operator '?()'", &node));
                }
                // this is the actual thing: FUNCTION RESOLUTION
                _res_expr_info = resolve_call(caller_info.functions, arg_infos, node);
                break;
        }
        // This is the property of safe call operator
        // where 'a?(...)' returns 'a(...)' if 'a' is not null, else returns null
        if (node.get_safe()) {
            switch (_res_expr_info.tag) {
                case ExprInfo::Type::NORMAL:
                case ExprInfo::Type::STATIC:
                    _res_expr_info.type_info.b_nullable = true;
                    break;
                case ExprInfo::Type::MODULE:
                    break;
                case ExprInfo::Type::FUNCTION_SET:
                    _res_expr_info.functions.b_nullable = true;
                    break;
            }
        }
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
    }

    void Analyzer::visit(ast::expr::Argument &node) {
        ArgInfo arg_info;
        arg_info.b_kwd = node.get_name() != null;
        arg_info.name = arg_info.b_kwd ? node.get_name()->get_text() : "";
        node.get_expr()->accept(this);
        resolve_indexer(_res_expr_info, true, node);
        arg_info.expr_info = _res_expr_info;
        arg_info.node = &node;

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
        auto caller_info = _res_expr_info;
        resolve_indexer(caller_info, true, node);

        std::vector<ArgInfo> arg_infos;
        arg_infos.reserve(node.get_slices().size());
        for (auto slice: node.get_slices()) {
            slice->accept(this);
            arg_infos.push_back(_res_arg_info);
        }
        _res_expr_info.reset();

        switch (caller_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (caller_info.is_null())
                    throw error("null is not indexable", &node);
                if (caller_info.type_info.b_nullable && !node.get_safe())
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot index nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe index operator '?[]'", &node));
                if (!caller_info.type_info.b_nullable && node.get_safe())
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe index operator on non-nullable '{}'", caller_info.to_string()), &node))
                            .note(error("remove the safe index operator '?[]'", &node));
                indexer_info.reset();
                indexer_info.caller_info = caller_info;
                indexer_info.arg_infos = arg_infos;
                indexer_info.node = &node;
                _res_expr_info = caller_info;
                break;
            }
            case ExprInfo::Type::STATIC: {
                if (caller_info.is_null())
                    throw error("null is not indexable", &node);
                if (caller_info.type_info.b_nullable && !node.get_safe())
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot index nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe index operator '?[]'", &node));
                if (!caller_info.type_info.b_nullable && node.get_safe())
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe index operator on non-nullable '{}'", caller_info.to_string()), &node))
                            .note(error("remove the safe index operator '?[]'", &node));
                for (const auto &arg_info: arg_infos)
                    if (arg_info.expr_info.tag != ExprInfo::Type::STATIC)
                        throw error(std::format("invalid type argument: '{}'", arg_info.to_string()), arg_info.node);
                // TODO: implement reify
                break;
            }
            case ExprInfo::Type::MODULE:
            case ExprInfo::Type::FUNCTION_SET:
                throw error(std::format("'{}' is not indexable", caller_info.to_string()), &node);
        }
    }

    void Analyzer::visit(ast::expr::Slice &node) {
        switch (node.get_kind()) {
            case ast::expr::Slice::Kind::INDEX: {
                ArgInfo arg_info;
                arg_info.b_kwd = false;
                arg_info.name = "";
                node.get_from()->accept(this);
                resolve_indexer(_res_expr_info, true, node);
                arg_info.expr_info = _res_expr_info;
                arg_info.node = &node;

                _res_arg_info.reset();
                _res_arg_info = arg_info;
                break;
            }
            case ast::expr::Slice::Kind::SLICE:
                // TODO: implement this
                ExprInfo start_expr_info, end_expr_info, step_expr_info;
                if (auto ctx = node.get_from()) {
                    ctx->accept(this);
                    start_expr_info = _res_expr_info;
                    resolve_indexer(start_expr_info, true, node);
                } else {
                    start_expr_info.tag = ExprInfo::Type::NORMAL;
                    start_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                    start_expr_info.type_info.b_nullable = false;
                    start_expr_info.value_info.b_null = true;
                }
                if (auto ctx = node.get_to()) {
                    ctx->accept(this);
                    end_expr_info = _res_expr_info;
                    resolve_indexer(end_expr_info, true, node);
                } else {
                    end_expr_info.tag = ExprInfo::Type::NORMAL;
                    end_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                    end_expr_info.type_info.b_nullable = false;
                    end_expr_info.value_info.b_null = true;
                }
                if (auto ctx = node.get_step()) {
                    ctx->accept(this);
                    step_expr_info = _res_expr_info;
                    resolve_indexer(step_expr_info, true, node);
                } else {
                    step_expr_info.tag = ExprInfo::Type::NORMAL;
                    step_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                    step_expr_info.type_info.b_nullable = false;
                    step_expr_info.value_info.b_null = true;
                }

                // call `basic.Slice(start: from, end: to, step: step)`
                ExprInfo caller_info;
                caller_info.tag = ExprInfo::Type::NORMAL;
                caller_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_SLICE);
                caller_info.type_info.b_nullable = false;
                auto slice_ctor = get_member(caller_info, "init", node);
                switch (slice_ctor.tag) {
                    case ExprInfo::Type::NORMAL:
                    case ExprInfo::Type::STATIC:
                    case ExprInfo::Type::MODULE:
                        throw ErrorGroup<AnalyzerError>()
                                .error(error(std::format("'{}' is ill formed", caller_info.to_string()), &node))
                                .note(error("declared here", caller_info.type_info.type));
                    case ExprInfo::Type::FUNCTION_SET:
                        std::vector<ArgInfo> args(3);
                        args[0].reset();
                        args[0].b_kwd = true;
                        args[0].name = "start";
                        args[0].expr_info = start_expr_info;
                        args[0].node = &*node.get_from();

                        args[1].reset();
                        args[1].b_kwd = true;
                        args[1].name = "end";
                        args[1].expr_info = end_expr_info;
                        args[1].node = &*node.get_to();

                        args[2].reset();
                        args[2].b_kwd = true;
                        args[2].name = "step";
                        args[2].expr_info = step_expr_info;
                        args[2].node = &*node.get_step();

                        _res_expr_info = resolve_call(slice_ctor.functions, args, node);
                        break;
                }

                ArgInfo arg_info;
                arg_info.b_kwd = false;
                arg_info.name = "";
                arg_info.expr_info = _res_expr_info;
                arg_info.node = &node;
                _res_arg_info = arg_info;
                break;
        }
    }

    void Analyzer::visit(ast::expr::Unary &node) {
        node.get_expr()->accept(this);
        auto expr_info = _res_expr_info;
        resolve_indexer(expr_info, true, node);
        if (expr_info.is_null())
            throw error(std::format("cannot apply unary operator '{}' on 'null'", node.get_op()->get_text()), &node);
        switch (expr_info.tag) {
            case ExprInfo::Type::NORMAL: {
                auto type_info = expr_info.type_info;
                if (type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    if (type_info.b_nullable)
                        throw error(std::format("cannot apply unary operator '{}' on nullable type '{}'", node.get_op()->get_text(),
                                                type_info.to_string()),
                                    &node);
                    _res_expr_info.reset();
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    switch (node.get_op()->get_type()) {
                        case TokenType::NOT:
                            _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
                            break;
                        case TokenType::TILDE: {
                            if (type_info.type == &*internals[Internal::SPADE_INT]) {
                                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                            } else {
                                // Check for overloaded operator ~
                                auto member = get_member(expr_info, OV_OP_INV, node);
                                switch (member.tag) {
                                    case ExprInfo::Type::NORMAL:
                                    case ExprInfo::Type::STATIC:
                                    case ExprInfo::Type::MODULE:
                                        throw error(std::format("cannot apply unary operator '~' on '{}'", type_info.to_string()), &node);
                                    case ExprInfo::Type::FUNCTION_SET: {
                                        _res_expr_info = resolve_call(member.functions, {}, node);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case TokenType::DASH: {
                            if (type_info.type == &*internals[Internal::SPADE_INT]) {
                                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                            } else if (type_info.type == &*internals[Internal::SPADE_FLOAT]) {
                                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                            } else {
                                // Check for overloaded operator -
                                auto member = get_member(expr_info, OV_OP_SUB, node);
                                switch (member.tag) {
                                    case ExprInfo::Type::NORMAL:
                                    case ExprInfo::Type::STATIC:
                                    case ExprInfo::Type::MODULE:
                                        throw error(std::format("cannot apply unary operator '-' on '{}'", type_info.to_string()), &node);
                                    case ExprInfo::Type::FUNCTION_SET: {
                                        _res_expr_info = resolve_call(member.functions, {}, node);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case TokenType::PLUS: {
                            if (type_info.type == &*internals[Internal::SPADE_INT]) {
                                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                            } else if (type_info.type == &*internals[Internal::SPADE_FLOAT]) {
                                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                            } else {
                                // Check for overloaded operator +
                                auto member = get_member(expr_info, OV_OP_ADD, node);
                                switch (member.tag) {
                                    case ExprInfo::Type::NORMAL:
                                    case ExprInfo::Type::STATIC:
                                    case ExprInfo::Type::MODULE:
                                        throw error(std::format("cannot apply unary operator '+' on '{}'", type_info.to_string()), &node);
                                    case ExprInfo::Type::FUNCTION_SET: {
                                        _res_expr_info = resolve_call(member.functions, {}, node);
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        default:
                            throw Unreachable();    // surely some parser error
                    }
                }
                break;
            }
            case ExprInfo::Type::STATIC:
            case ExprInfo::Type::MODULE:
            case ExprInfo::Type::FUNCTION_SET:
                throw error(std::format("cannot apply unary operator '{}' on '{}'", node.get_op()->get_text(), expr_info.to_string()), &node);
        }
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
    }

    void Analyzer::visit(ast::expr::Cast &node) {
        node.get_expr()->accept(this);
        auto expr_info = _res_expr_info;
        resolve_indexer(expr_info, true, node);
        if (expr_info.tag != ExprInfo::Type::NORMAL)
            throw error(std::format("cannot cast '{}'", expr_info.to_string()), &node);
        node.get_type()->accept(this);
        auto type_cast_info = _res_type_info;
        if (type_cast_info.b_nullable)
            throw error("cast type cannot be nullable", &node);

        _res_expr_info.reset();
        _res_expr_info.tag = ExprInfo::Type::NORMAL;

        if (expr_info.is_null()) {
            if (node.get_safe()) {
                warning("expression is always 'null'", &node);
                _res_expr_info.value_info.b_null = true;
            } else
                throw error("cannot cast 'null'", &node);
        }

        if (type_cast_info.is_type_literal()) {
            warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
            _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
            _res_expr_info.type_info.b_nullable = true;
        } else {
            if (!expr_info.is_null())
                check_cast(expr_info.type_info.type, type_cast_info.type, node, node.get_safe() != null);
            _res_expr_info.type_info = type_cast_info;
            _res_expr_info.type_info.b_nullable = node.get_safe() != null;
        }

        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
    }

#define is_number_type(type_info) ((type_info).type == &*internals[Internal::SPADE_INT] || (type_info).type == &*internals[Internal::SPADE_FLOAT])
#define is_string_type(type_info) ((type_info).type == &*internals[Internal::SPADE_STRING])

    void Analyzer::visit(ast::expr::Binary &node) {
#define find_user_defined_op_no_rev(OPERATOR)                                                                                                        \
    do {                                                                                                                                             \
        ErrorGroup<AnalyzerError> errors;                                                                                                            \
        auto member = get_member(left_expr_info, OV_OP_##OPERATOR, node, errors);                                                                    \
        if (!errors.get_errors().empty())                                                                                                            \
            throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),                        \
                                    right_expr_info.to_string()),                                                                                    \
                        &node);                                                                                                                      \
        switch (member.tag) {                                                                                                                        \
            case ExprInfo::Type::NORMAL:                                                                                                             \
            case ExprInfo::Type::STATIC:                                                                                                             \
            case ExprInfo::Type::MODULE:                                                                                                             \
                throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),                    \
                                        right_expr_info.to_string()),                                                                                \
                            &node);                                                                                                                  \
                break;                                                                                                                               \
            case ExprInfo::Type::FUNCTION_SET: {                                                                                                     \
                std::vector<ArgInfo> args(1);                                                                                                        \
                args[0] = ArgInfo{false, "", right_expr_info, &node};                                                                                \
                _res_expr_info = resolve_call(member.functions, args, node);                                                                         \
                break;                                                                                                                               \
            }                                                                                                                                        \
        }                                                                                                                                            \
    } while (false)
#define find_user_defined_op(OPERATOR)                                                                                                               \
    do {                                                                                                                                             \
        ErrorGroup<AnalyzerError> errors;                                                                                                            \
        auto member = get_member(left_expr_info, OV_OP_##OPERATOR, node, errors);                                                                    \
        bool find_rev_op = left_expr_info.type_info.b_nullable || !errors.get_errors().empty();                                                      \
        if (!find_rev_op) {                                                                                                                          \
            switch (member.tag) {                                                                                                                    \
                case ExprInfo::Type::NORMAL:                                                                                                         \
                case ExprInfo::Type::STATIC:                                                                                                         \
                case ExprInfo::Type::MODULE:                                                                                                         \
                    find_rev_op = true;                                                                                                              \
                    break;                                                                                                                           \
                case ExprInfo::Type::FUNCTION_SET: {                                                                                                 \
                    std::vector<ArgInfo> args(1);                                                                                                    \
                    args[0] = ArgInfo{false, "", right_expr_info, &node};                                                                            \
                    _res_expr_info = resolve_call(member.functions, args, node);                                                                     \
                    break;                                                                                                                           \
                }                                                                                                                                    \
            }                                                                                                                                        \
        }                                                                                                                                            \
        if (find_rev_op) {                                                                                                                           \
            ErrorGroup<AnalyzerError> errors;                                                                                                        \
            auto member = get_member(right_expr_info, OV_OP_REV_##OPERATOR, node, errors);                                                           \
            if (right_expr_info.type_info.b_nullable || !errors.get_errors().empty())                                                                \
                throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),                    \
                                        right_expr_info.to_string()),                                                                                \
                            &node);                                                                                                                  \
            switch (member.tag) {                                                                                                                    \
                case ExprInfo::Type::NORMAL:                                                                                                         \
                case ExprInfo::Type::STATIC:                                                                                                         \
                case ExprInfo::Type::MODULE:                                                                                                         \
                    throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),                \
                                            right_expr_info.to_string()),                                                                            \
                                &node);                                                                                                              \
                case ExprInfo::Type::FUNCTION_SET: {                                                                                                 \
                    std::vector<ArgInfo> args(1);                                                                                                    \
                    args[0] = ArgInfo{false, "", left_expr_info, &node};                                                                             \
                    _res_expr_info = resolve_call(member.functions, args, node);                                                                     \
                    break;                                                                                                                           \
                }                                                                                                                                    \
            }                                                                                                                                        \
        }                                                                                                                                            \
    } while (false)
#define check_non_null()                                                                                                                             \
    do {                                                                                                                                             \
        if (left_expr_info.type_info.b_nullable || right_expr_info.type_info.b_nullable)                                                             \
            throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),                        \
                                    right_expr_info.to_string()),                                                                                    \
                        &node);                                                                                                                      \
    } while (false)
        string op_str = (node.get_op1() ? node.get_op1()->get_text() : "") + (node.get_op2() ? node.get_op2()->get_text() : "");

        node.get_left()->accept(this);
        auto left_expr_info = _res_expr_info;
        resolve_indexer(left_expr_info, true, node);

        node.get_right()->accept(this);
        auto right_expr_info = _res_expr_info;
        resolve_indexer(right_expr_info, true, node);

        switch (left_expr_info.tag) {
            case ExprInfo::Type::NORMAL: {
                auto type_info = left_expr_info.type_info;
                if (type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                    _res_expr_info.type_info.b_nullable = true;
                    return;
                }
                break;
            }
            case ExprInfo::Type::STATIC:
            case ExprInfo::Type::MODULE:
            case ExprInfo::Type::FUNCTION_SET:
                throw error(std::format("cannot apply binary operator '{}' on '{}'", op_str, left_expr_info.to_string()), &node);
        }
        switch (right_expr_info.tag) {
            case ExprInfo::Type::NORMAL: {
                auto type_info = right_expr_info.type_info;
                if (type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                    _res_expr_info.type_info.b_nullable = true;
                    return;
                }
                break;
            }
            case ExprInfo::Type::STATIC:
            case ExprInfo::Type::MODULE:
            case ExprInfo::Type::FUNCTION_SET:
                throw error(std::format("cannot apply binary operator '{}' on '{}'", op_str, right_expr_info.to_string()), &node);
        }

        _res_expr_info.reset();
        _res_expr_info.tag = ExprInfo::Type::NORMAL;
        switch (node.get_op1()->get_type()) {
            case TokenType::ELVIS: {
                if (left_expr_info.is_null())
                    warning(std::format("left hand expression of '{}' operator is never evaluated", op_str), node.get_left());
                if (!left_expr_info.type_info.b_nullable)
                    warning(std::format("right hand expression of '{}' operator is never evaluated", op_str), node.get_right());
                if (left_expr_info.is_null() && right_expr_info.is_null()) {
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                } else if (left_expr_info.is_null()) {
                    _res_expr_info.type_info.type = right_expr_info.type_info.type;
                } else if (right_expr_info.is_null()) {
                    _res_expr_info.type_info.type = left_expr_info.type_info.type;
                } else if (left_expr_info.type_info.type != right_expr_info.type_info.type) {
                    throw error("cannot infer type of the expression", &node);
                } else
                    _res_expr_info.type_info.type = left_expr_info.type_info.type;
                // TODO: check type args for covariance and contravariance
                // _res_expr_info.type_info.type_args = left_expr_info.type_info.type_args;
                _res_expr_info.type_info.b_nullable = right_expr_info.type_info.b_nullable;
                break;
            }
            case TokenType::STAR_STAR:
                if (is_number_type(left_expr_info.type_info) && is_number_type(right_expr_info.type_info)) {
                    check_non_null();
                    if (left_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT] ||
                        right_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT])
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                    else
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator **
                    find_user_defined_op(POW);
                break;
            case TokenType::STAR:
                if (is_number_type(left_expr_info.type_info) && is_number_type(right_expr_info.type_info)) {
                    check_non_null();
                    if (left_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT] ||
                        right_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT])
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                    else
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else if (is_string_type(left_expr_info.type_info) && right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    // `string` * `int` -> `string`
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_STRING);
                } else
                    // Check for overloaded operator *
                    find_user_defined_op(MUL);
                break;
            case TokenType::SLASH:
                if (is_number_type(left_expr_info.type_info) && is_number_type(right_expr_info.type_info)) {
                    check_non_null();
                    if (left_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT] ||
                        right_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT])
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                    else
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator /
                    find_user_defined_op(DIV);
                break;
            case TokenType::PERCENT:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator %
                    find_user_defined_op(MOD);
                break;
            case TokenType::PLUS:
                if (is_number_type(left_expr_info.type_info) && is_number_type(right_expr_info.type_info)) {
                    // `int` + `int` -> `int`
                    // `float` + `float` or `int` + `float` or `float` + `int` -> `float`
                    check_non_null();
                    if (left_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT] ||
                        right_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT])
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                    else
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else if (is_string_type(left_expr_info.type_info) || is_string_type(right_expr_info.type_info)) {
                    // `any` + `string` or `string` + `any` or `string` + `string` -> `string`
                    // check_non_null(); // E.g. `"val: " + val` can be "val: null"
                    if (left_expr_info.type_info.b_nullable && right_expr_info.type_info.b_nullable)
                        throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                right_expr_info.to_string()),
                                    &node);
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_STRING);
                } else
                    // Check for overloaded operator +
                    find_user_defined_op(ADD);
                break;
            case TokenType::DASH:
                if (is_number_type(left_expr_info.type_info) && is_number_type(right_expr_info.type_info)) {
                    check_non_null();
                    if (left_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT] ||
                        right_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT])
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_FLOAT);
                    else
                        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator -
                    find_user_defined_op(SUB);
                break;
            case TokenType::LSHIFT:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator <<
                    find_user_defined_op(LSHIFT);
                break;
            case TokenType::RSHIFT:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator >>
                    find_user_defined_op(RSHIFT);
                break;
            case TokenType::URSHIFT:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator >>>
                    find_user_defined_op(URSHIFT);
                break;
            case TokenType::AMPERSAND:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator &
                    find_user_defined_op(AND);
                break;
            case TokenType::CARET:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator ^
                    find_user_defined_op(XOR);
                break;
            case TokenType::PIPE:
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                    right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                    check_non_null();
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_INT);
                } else
                    // Check for overloaded operator |
                    find_user_defined_op(OR);
                break;
            case TokenType::IS:
                // Either `is` or `is not` operator
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
                // _res_expr_info.type_info.b_nullable = false; // by default it is false
                break;
            case TokenType::NOT:
            case TokenType::IN:
                // Either `in` or `not in` operator
                find_user_defined_op_no_rev(CONTAINS);
                break;
            case TokenType::AND:
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
                break;
            case TokenType::OR:
                _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
                break;
            default:
                throw Unreachable();    // surely some parser error
        }
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
#undef find_user_defined_op
#undef find_user_defined_op_no_rev
#undef check_non_null
    }

    void Analyzer::visit(ast::expr::ChainBinary &node) {
        std::optional<ExprInfo> prev_expr_opt;
        size_t i = 0;
        for (const auto &cur_expr: node.get_exprs()) {
            cur_expr->accept(this);
            resolve_indexer(_res_expr_info, true, node);
            auto right_expr_info = _res_expr_info;
            if (prev_expr_opt) {
                auto left_expr_info = *prev_expr_opt;
                string op_str = node.get_ops()[i - 1]->get_text();
                string ov_op_str;
                switch (left_expr_info.tag) {
                    case ExprInfo::Type::NORMAL: {
                        auto type_info = left_expr_info.type_info;
                        if (type_info.is_type_literal()) {
                            warning("'type' causes dynamic resolution", &node);
                            continue;
                        }
                        break;
                    }
                    case ExprInfo::Type::STATIC:
                    case ExprInfo::Type::MODULE:
                    case ExprInfo::Type::FUNCTION_SET:
                        throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                right_expr_info.to_string()),
                                    &node);
                }
                switch (right_expr_info.tag) {
                    case ExprInfo::Type::NORMAL: {
                        auto type_info = right_expr_info.type_info;
                        if (type_info.is_type_literal()) {
                            warning("'type' causes dynamic resolution", &node);
                            continue;
                        }
                        break;
                    }
                    case ExprInfo::Type::STATIC:
                    case ExprInfo::Type::MODULE:
                    case ExprInfo::Type::FUNCTION_SET:
                        throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                right_expr_info.to_string()),
                                    &node);
                }
                switch (node.get_ops()[i - 1]->get_type()) {
                    case TokenType::LT:
                        ov_op_str = OV_OP_LT;
                        goto lt_le_ge_gt_common;
                    case TokenType::LE:
                        ov_op_str = OV_OP_LE;
                        goto lt_le_ge_gt_common;
                    case TokenType::GE:
                        ov_op_str = OV_OP_GE;
                        goto lt_le_ge_gt_common;
                    case TokenType::GT:
                        ov_op_str = OV_OP_GT;
lt_le_ge_gt_common:
                        if (left_expr_info.type_info.b_nullable || right_expr_info.type_info.b_nullable)
                            throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                    right_expr_info.to_string()),
                                        &node);
                        if (left_expr_info.tag != ExprInfo::Type::NORMAL || right_expr_info.tag != ExprInfo::Type::NORMAL)
                            throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                    right_expr_info.to_string()),
                                        &node);
                        if (left_expr_info.type_info.b_nullable || right_expr_info.type_info.b_nullable)
                            throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                    right_expr_info.to_string()),
                                        &node);
                        if (is_number_type(left_expr_info.type_info) && is_number_type(right_expr_info.type_info)) {
                            // plain int|float <, <=, >=, > int|float
                        } else if (is_string_type(left_expr_info.type_info) && is_string_type(right_expr_info.type_info)) {
                            // plain string <, <=, >=, > string
                        } else {
                            // Check for overloaded operator <, <=, >=, >
                            auto member = get_member(left_expr_info, ov_op_str, node);
                            switch (member.tag) {
                                case ExprInfo::Type::NORMAL:
                                case ExprInfo::Type::STATIC:
                                case ExprInfo::Type::MODULE:
                                    throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(),
                                                            right_expr_info.to_string()),
                                                &node);
                                case ExprInfo::Type::FUNCTION_SET: {
                                    std::vector<ArgInfo> args(1);
                                    args[0] = ArgInfo{false, "", right_expr_info, &node};
                                    resolve_call(member.functions, args, node);
                                    break;
                                }
                            }
                        }
                        break;
                    case TokenType::EQ:
                        break;
                    case TokenType::NE:
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
            prev_expr_opt = right_expr_info;
            i++;
        }

        _res_expr_info.tag = ExprInfo::Type::NORMAL;
        _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_BOOL);
        _res_expr_info.type_info.b_nullable = false;
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
        _res_expr_info.value_info.b_null = false;
        _res_expr_info.value_info.b_self = false;
    }

    void Analyzer::visit(ast::expr::Ternary &node) {
        node.get_condition()->accept(this);
        node.get_on_true()->accept(this);
        auto expr_info1 = _res_expr_info;
        resolve_indexer(expr_info1, true, node);
        node.get_on_false()->accept(this);
        auto expr_info2 = _res_expr_info;
        resolve_indexer(expr_info2, true, node);
        if (expr_info1.tag != expr_info2.tag)
            throw error("cannot infer type of the expression", &node);
        _res_expr_info.reset();
        switch (expr_info1.tag) {
            case ExprInfo::Type::NORMAL: {
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                if (expr_info1.is_null() && expr_info2.is_null()) {
                    _res_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                } else if (expr_info1.is_null()) {
                    _res_expr_info.type_info.type = expr_info2.type_info.type;
                } else if (expr_info2.is_null()) {
                    _res_expr_info.type_info.type = expr_info1.type_info.type;
                } else if (expr_info1.type_info.type != expr_info2.type_info.type) {
                    throw error("cannot infer type of the expression", &node);
                } else
                    _res_expr_info.type_info.type = expr_info1.type_info.type;
                // TODO: check type args for covariance and contravariance
                // _res_expr_info.type_info.type_args = expr_info1.type_info.type_args;
                _res_expr_info.type_info.b_nullable = expr_info1.type_info.b_nullable || expr_info2.type_info.b_nullable;
                break;
            }
            case ExprInfo::Type::STATIC:
                // expr returns 'type'
                _res_expr_info.type_info.type = null;
                _res_expr_info.type_info.type_args = {};
                _res_expr_info.type_info.b_nullable = expr_info1.type_info.b_nullable || expr_info2.type_info.b_nullable;
                break;
            case ExprInfo::Type::MODULE:
                throw error("cannot infer type of the expression", &node);
            case ExprInfo::Type::FUNCTION_SET:
                // TODO: check if they have the same signature
                break;
        }
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = expr_info1.value_info.b_const || expr_info2.value_info.b_const;
    }

    void Analyzer::visit(ast::expr::Assignment &node) {
        if (node.get_assignees().size() != node.get_exprs().size())
            throw error(std::format("expected {} values but got {}", node.get_assignees().size(), node.get_exprs().size()), &node);

        ExprInfo last_expr_info;
        for (size_t i = 0; i < node.get_assignees().size(); i++) {
            auto expr_node = node.get_exprs()[i];
            expr_node->accept(this);
            auto right_expr_info = _res_expr_info;
            resolve_indexer(right_expr_info, true, node);

            auto assignee_node = node.get_assignees()[i];
            assignee_node->accept(this);
            auto left_expr_info = _res_expr_info;

            if (indexer_info) {
                if (node.get_op1()->get_type() != TokenType::EQUAL)
                    throw error("augmented assignment on an indexer is not allowed", &node);
                // Add the value as the last argument of the indexer
                ArgInfo value_arg;
                // value_arg.b_kwd = false;
                // value_arg.name = "";
                value_arg.expr_info = right_expr_info;
                value_arg.node = &*expr_node;
                indexer_info.arg_infos.push_back(value_arg);
                resolve_indexer(left_expr_info, false, node);
                if (left_expr_info.type_info.type == &*internals[Internal::SPADE_VOID]) {
                    last_expr_info.reset();
                    last_expr_info.tag = ExprInfo::Type::NORMAL;
                    last_expr_info.type_info.type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                    last_expr_info.type_info.b_nullable = true;
                    last_expr_info.value_info.b_null = true;
                } else
                    last_expr_info = left_expr_info;
                continue;
            }

            // avoid assigning `void` value
            if ((right_expr_info.tag == ExprInfo::Type::NORMAL || right_expr_info.tag == ExprInfo::Type::STATIC) &&
                right_expr_info.type_info.type == &*internals[Internal::SPADE_VOID])
                throw error(std::format("cannot assign '{}' to an object", right_expr_info.to_string()), expr_node);
            // avoid assigning to `void`
            if ((left_expr_info.tag == ExprInfo::Type::NORMAL || left_expr_info.tag == ExprInfo::Type::STATIC) &&
                left_expr_info.type_info.type == &*internals[Internal::SPADE_VOID])
                throw error(std::format("cannot assign to '{}'", left_expr_info.to_string()), assignee_node);

            // expression checks
            if (left_expr_info.tag != ExprInfo::Type::NORMAL)
                throw error(std::format("cannot assign to '{}'", left_expr_info.to_string()), assignee_node);
            if (!left_expr_info.value_info.b_lvalue)
                throw error("cannot assign to a non-lvalue expression", assignee_node);
            if (left_expr_info.value_info.b_const)
                throw error("cannot assign to a constant", assignee_node);
            if (!left_expr_info.type_info.b_nullable && right_expr_info.type_info.b_nullable)
                throw error(std::format("cannot assign nullable '{}' to non-nullable '{}'", right_expr_info.to_string(), left_expr_info.to_string()),
                            expr_node);
            // Plain vanilla assignment
            if (node.get_op1()->get_type() == TokenType::EQUAL) {
                last_expr_info.tag = ExprInfo::Type::NORMAL;
                last_expr_info.type_info = resolve_assign(left_expr_info.type_info, right_expr_info, node);
                last_expr_info.value_info = left_expr_info.value_info;
            } else if (node.get_op2()->get_type() == TokenType::EQUAL) {
                // Augmented assignment
                string op_str = node.get_op1()->get_text() + node.get_op2()->get_text();
#define find_user_defined_aug_op(OPERATOR)                                                                                                           \
    do {                                                                                                                                             \
        ErrorGroup<AnalyzerError> errors;                                                                                                            \
        auto member = get_member(left_expr_info, OV_OP_AUG_##OPERATOR, node, errors);                                                                \
        if (left_expr_info.type_info.b_nullable || !errors.get_errors().empty())                                                                     \
            throw error(std::format("cannot apply operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(), right_expr_info.to_string()), \
                        &node);                                                                                                                      \
        switch (member.tag) {                                                                                                                        \
            case ExprInfo::Type::NORMAL:                                                                                                             \
            case ExprInfo::Type::STATIC:                                                                                                             \
            case ExprInfo::Type::MODULE:                                                                                                             \
                throw error(                                                                                                                         \
                        std::format("cannot apply operator '{}' on '{}' and '{}'", op_str, left_expr_info.to_string(), right_expr_info.to_string()), \
                        &node);                                                                                                                      \
                break;                                                                                                                               \
            case ExprInfo::Type::FUNCTION_SET: {                                                                                                     \
                std::vector<ArgInfo> args(1);                                                                                                        \
                args[0] = ArgInfo{false, "", right_expr_info, &node};                                                                                \
                last_expr_info = resolve_call(member.functions, args, node);                                                                         \
                break;                                                                                                                               \
            }                                                                                                                                        \
        }                                                                                                                                            \
    } while (false)
                switch (node.get_op1()->get_type()) {
                    case TokenType::ELVIS:
                        if (!left_expr_info.type_info.b_nullable)
                            warning(std::format("right hand expression of '{}' operator is never evaluated", op_str), expr_node);
                        if (left_expr_info.type_info.type != right_expr_info.type_info.type) {
                            throw error("cannot infer type of the expression", &node);
                        } else
                            last_expr_info.type_info.type = left_expr_info.type_info.type;
                        last_expr_info.type_info.b_nullable = right_expr_info.type_info.b_nullable;
                        last_expr_info.value_info = left_expr_info.value_info;
                        break;
                    case TokenType::STAR_STAR:
                        if (is_number_type(left_expr_info.type_info) &&
                            (is_number_type(right_expr_info.type_info) || (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(POW);
                        break;
                    case TokenType::STAR:
                        if (is_number_type(left_expr_info.type_info) && (is_number_type(right_expr_info.type_info) ||
                                                                         (left_expr_info.type_info.b_nullable && right_expr_info.is_null())) ||
                            is_string_type(left_expr_info.type_info) && right_expr_info.type_info.type == &*internals[Internal::SPADE_INT]) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(MUL);
                        break;
                    case TokenType::SLASH:
                        if (is_number_type(left_expr_info.type_info) &&
                            (is_number_type(right_expr_info.type_info) || (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(DIV);
                        break;
                    case TokenType::PERCENT:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(MOD);
                        break;
                    case TokenType::PLUS: {
                        if (is_number_type(left_expr_info.type_info) &&
                            (is_number_type(right_expr_info.type_info) || (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else if (is_string_type(left_expr_info.type_info) || is_string_type(right_expr_info.type_info)) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(ADD);
                        break;
                    }
                    case TokenType::DASH:
                        if (is_number_type(left_expr_info.type_info) &&
                            (is_number_type(right_expr_info.type_info) || (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(SUB);
                        break;
                    case TokenType::LSHIFT:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(LSHIFT);
                        break;
                    case TokenType::RSHIFT:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(RSHIFT);
                        break;
                    case TokenType::URSHIFT:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(RSHIFT);
                        break;
                    case TokenType::AMPERSAND:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(AND);
                        break;
                    case TokenType::PIPE:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(OR);
                        break;
                    case TokenType::CARET:
                        if (left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] &&
                            (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                             (left_expr_info.type_info.b_nullable && right_expr_info.is_null()))) {
                            last_expr_info = left_expr_info;
                        } else
                            find_user_defined_aug_op(XOR);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
        }
        // return the value of the last expression
        _res_expr_info = last_expr_info;
#undef find_user_defined_aug_op
    }
}    // namespace spade