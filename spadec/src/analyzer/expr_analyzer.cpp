#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "spimp/error.hpp"
#include "utils/error.hpp"
#include <vector>

namespace spade
{
    // Names of functions that represent overloaded operators

    // Binary ops
    // `a + b` is same as `a.__add__(b)` if `a` has defined `__add__` function
    static const string OV_OP_POW = "__pow__";
    static const string OV_OP_MUL = "__mul__";
    static const string OV_OP_DIV = "__div__";
    static const string OV_OP_MOD = "__mod__";
    static const string OV_OP_ADD = "__add__";
    static const string OV_OP_SUB = "__sub__";
    static const string OV_OP_LSHIFT = "__lshift__";
    static const string OV_OP_RSHIFT = "__rshift__";
    static const string OV_OP_URSHIFT = "__urshift__";
    static const string OV_OP_AND = "__and__";
    static const string OV_OP_XOR = "__xor__";
    static const string OV_OP_OR = "__or__";
    // `a + b` is same as `b.__rev_add__(a)` if `a` has not defined `__add__` function
    static const string OV_OP_REV_POW = "__rev_pow__";
    static const string OV_OP_REV_MUL = "__rev_mul__";
    static const string OV_OP_REV_DIV = "__rev_div__";
    static const string OV_OP_REV_MOD = "__rev_mod__";
    static const string OV_OP_REV_ADD = "__rev_add__";
    static const string OV_OP_REV_SUB = "__rev_sub__";
    static const string OV_OP_REV_LSHIFT = "__rev_lshift__";
    static const string OV_OP_REV_RSHIFT = "__rev_rshift__";
    static const string OV_OP_REV_URSHIFT = "__rev_urshift__";
    static const string OV_OP_REV_AND = "__rev_and__";
    static const string OV_OP_REV_XOR = "__rev_xor__";
    static const string OV_OP_REV_OR = "__rev_or__";
    // `a += b` is same as `b.__aug_add__(a)`
    static const string OV_OP_AUG_POW = "__aug_pow__";
    static const string OV_OP_AUG_MUL = "__aug_mul__";
    static const string OV_OP_AUG_DIV = "__aug_div__";
    static const string OV_OP_AUG_MOD = "__aug_mod__";
    static const string OV_OP_AUG_ADD = "__aug_add__";
    static const string OV_OP_AUG_SUB = "__aug_sub__";
    static const string OV_OP_AUG_LSHIFT = "__aug_lshift__";
    static const string OV_OP_AUG_RSHIFT = "__aug_rshift__";
    static const string OV_OP_AUG_URSHIFT = "__aug_urshift__";
    static const string OV_OP_AUG_AND = "__aug_and__";
    static const string OV_OP_AUG_XOR = "__aug_xor__";
    static const string OV_OP_AUG_OR = "__aug_or__";
    // Comparison operators
    static const string OV_OP_LT = "__lt__";
    static const string OV_OP_LE = "__le__";
    static const string OV_OP_EQ = "__eq__";
    static const string OV_OP_NE = "__ne__";
    static const string OV_OP_GE = "__ge__";
    static const string OV_OP_GT = "__gt__";

    // Postfix operators
    // `a(arg1, arg2, ...)` is same as `a.__call__(arg1, arg2, ...)`
    static const string OV_OP_CALL = "__call__";
    // `a[arg1, arg2, ...]` is same as `a.__get_item__(arg1, arg2, ...)`
    static const string OV_OP_GET_ITEM = "__get_item__";
    // `a[arg1, arg2, ...] = value` is same as `a.__set_item__(arg1, arg2, ..., value)`
    static const string OV_OP_SET_ITEM = "__set_item__";
    // `a in b` is same as `b.__contains__(a)`
    static const string OV_OP_CONTAINS = "__contains__";

    // Unary operators
    // `~a` is same as `a.__inv__()`
    static const string OV_OP_INV = "__inv__";
    // `-a` is same as `a.__neg__()`
    static const string OV_OP_NEG = "__neg__";
    // `+a` is same as `a.__pos__()`
    static const string OV_OP_POS = "__pos__";

    // This function overloads the truthiness of an object
    static const string OV_OP_BOOL = "__bool__";

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
                _res_expr_info.value_info.b_null = true;
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
                for (const auto &parent: klass->get_supers()) {
                    if (parent->get_compound_node()->get_token()->get_type() == TokenType::CLASS) {
                        _res_expr_info.type_info.type = parent;
                        break;
                    }
                }
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
        string member_name = node.get_member()->get_text();
        _res_expr_info.reset();
        switch (caller_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (caller_info.is_null())
                    throw error("cannot access 'null'", &node);
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot access member of nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe dot access operator '?.'", &node));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe dot access operator on non-nullable '{}'",
                                                     caller_info.to_string()),
                                         &node))
                            .note(error("remove the safe dot access operator '?.'", &node));
                }
                if (caller_info.type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    if (!caller_info.type_info.type->has_variable(member_name))
                        throw error(std::format("cannot access member: '{}'", member_name), &node);
                    auto member_scope = caller_info.type_info.type->get_variable(member_name);
                    if (!member_scope) {
                        // Provision of super fields and functions
                        if (auto compound = get_current_scope()->get_enclosing_compound()) {
                            if (compound->get_super_fields().contains(member_name)) {
                                member_scope = compound->get_super_fields().at(member_name);
                            } else if (compound->get_super_functions().contains(member_name)) {
                                _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                                _res_expr_info.value_info.b_const = true;
                                _res_expr_info.value_info.b_lvalue = true;
                                _res_expr_info.functions = compound->get_super_functions().at(member_name);
                                break;
                            }
                        }
                    }
                    if (!member_scope)
                        throw error(std::format("'{}' has no member named '{}'", caller_info.to_string(), member_name), &node);
                    resolve_context(member_scope, node);
                    switch (member_scope->get_type()) {
                        case scope::ScopeType::COMPOUND:
                            _res_expr_info.tag = ExprInfo::Type::STATIC;
                            _res_expr_info.value_info.b_const = true;
                            _res_expr_info.value_info.b_lvalue = true;
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*member_scope);
                            break;
                        case scope::ScopeType::FUNCTION:
                            throw Unreachable();    // surely some symbol tree builder error
                        case scope::ScopeType::FUNCTION_SET:
                            _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                            _res_expr_info.value_info.b_const = true;
                            _res_expr_info.value_info.b_lvalue = true;
                            _res_expr_info.functions = cast<scope::FunctionSet>(&*member_scope);
                            break;
                        case scope::ScopeType::VARIABLE:
                            _res_expr_info = get_var_expr_info(cast<scope::Variable>(member_scope), node);
                            break;
                        case scope::ScopeType::ENUMERATOR:
                            throw ErrorGroup<AnalyzerError>()
                                    .error(error("cannot access enumerator from an object (you should use the type)", &node))
                                    .note(error(
                                            std::format("use {}.{}", caller_info.type_info.type->to_string(false), member_name),
                                            &node));
                            break;
                        default:
                            throw Unreachable();    // surely some parser error
                    }
                }
                break;
            }
            case ExprInfo::Type::STATIC: {
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot access member of nullable '{}'", caller_info.to_string()), &node))
                            .note(error("use safe dot access operator '?.'", &node));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot use safe dot access operator on non-nullable '{}'",
                                                     caller_info.to_string()),
                                         &node))
                            .note(error("remove the safe dot access operator '?.'", &node));
                }
                if (caller_info.type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    if (!caller_info.type_info.type->has_variable(member_name))
                        throw error(std::format("cannot access member: '{}'", member_name), &node);
                    auto member_scope = caller_info.type_info.type->get_variable(member_name);
                    if (!member_scope)
                        throw error(std::format("'{}' has no member named '{}'", caller_info.to_string(), member_name), &node);
                    resolve_context(member_scope, node);
                    switch (member_scope->get_type()) {
                        case scope::ScopeType::COMPOUND:
                            _res_expr_info.tag = ExprInfo::Type::STATIC;
                            _res_expr_info.value_info.b_const = true;
                            _res_expr_info.value_info.b_lvalue = true;
                            _res_expr_info.type_info.type = cast<scope::Compound>(&*member_scope);
                            break;
                        case scope::ScopeType::FUNCTION:
                            throw Unreachable();    // surely some symbol tree builder error
                        case scope::ScopeType::FUNCTION_SET:
                            _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                            _res_expr_info.value_info.b_const = true;
                            _res_expr_info.value_info.b_lvalue = true;
                            _res_expr_info.functions = cast<scope::FunctionSet>(&*member_scope);
                            _res_expr_info.functions.remove_if(
                                    [](const std::pair<const SymbolPath &, const scope::Function *> &item) {
                                        return !item.second->is_static();
                                    });
                            if (_res_expr_info.functions.empty()) {
                                throw error(std::format("cannot access non-static '{}' of '{}'", member_scope->to_string(),
                                                        caller_info.to_string()),
                                            &node);
                            }
                            break;
                        case scope::ScopeType::VARIABLE: {
                            auto var_scope = cast<scope::Variable>(member_scope);
                            if (!var_scope->is_static())
                                throw error(std::format("cannot access non-static '{}' of '{}'", var_scope->to_string(),
                                                        caller_info.to_string()),
                                            &node);
                            _res_expr_info = get_var_expr_info(var_scope, node);
                            break;
                        }
                        case scope::ScopeType::ENUMERATOR:
                            _res_expr_info.type_info.type = caller_info.type_info.type;
                            _res_expr_info.value_info.b_const = true;
                            _res_expr_info.value_info.b_lvalue = true;
                            _res_expr_info.tag = ExprInfo::Type::NORMAL;
                            break;
                        default:
                            throw Unreachable();    // surely some parser error
                    }
                }
                break;
            }
            case ExprInfo::Type::MODULE: {
                if (node.get_safe())
                    throw error("cannot use safe dot access operator on a module", &node);
                if (!caller_info.module->has_variable(member_name))
                    throw error(std::format("cannot access member: '{}'", member_name), &node);
                auto member_scope = caller_info.module->get_variable(member_name);
                if (!member_scope)
                    throw error(std::format("'{}' has no member named '{}'", caller_info.to_string(), member_name), &node);
                resolve_context(member_scope, node);
                switch (member_scope->get_type()) {
                    case scope::ScopeType::FOLDER_MODULE:
                    case scope::ScopeType::MODULE:
                        _res_expr_info.tag = ExprInfo::Type::MODULE;
                        _res_expr_info.value_info.b_const = true;
                        _res_expr_info.value_info.b_lvalue = true;
                        _res_expr_info.module = cast<scope::Module>(&*member_scope);
                        break;
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        _res_expr_info.value_info.b_const = true;
                        _res_expr_info.value_info.b_lvalue = true;
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*member_scope);
                        break;
                    case scope::ScopeType::FUNCTION:
                        throw Unreachable();    // surely some symbol tree builder error
                    case scope::ScopeType::FUNCTION_SET:
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                        _res_expr_info.value_info.b_const = true;
                        _res_expr_info.value_info.b_lvalue = true;
                        _res_expr_info.functions = cast<scope::FunctionSet>(&*member_scope);
                        break;
                    case scope::ScopeType::VARIABLE:
                        _res_expr_info = get_var_expr_info(cast<scope::Variable>(member_scope), node);
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
        _res_expr_info.value_info.b_lvalue = true;
        // Fix for `self.a` const error bcz `self.a` is not constant if it is declared non-const
        if (!_res_expr_info.value_info.b_const)
            _res_expr_info.value_info.b_const = caller_info.value_info.b_const && !caller_info.value_info.b_self;
        _res_expr_info.value_info.b_self = false;
    }

    void Analyzer::visit(ast::expr::Call &node) {
        node.get_caller()->accept(this);
        auto caller_info = _res_expr_info;

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
                            .error(error(
                                    std::format("cannot use safe call operator on non-nullable '{}'", caller_info.to_string()),
                                    &node))
                            .note(error("remove the safe call operator '?()'", &node));
                }
                if (caller_info.type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    // also supports self(...) syntax
                    // check for call operator
                    if (auto fun_set = caller_info.type_info.type->get_variable(OV_OP_CALL)) {
                        _res_expr_info = resolve_call(&*cast<scope::FunctionSet>(fun_set), arg_infos, node);
                    } else
                        throw error(std::format("object of '{}' is not callable", caller_info.to_string()), &node);
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
                            .error(error(
                                    std::format("cannot use safe call operator on non-nullable '{}'", caller_info.to_string()),
                                    &node))
                            .note(error("remove the safe call operator '?()'", &node));
                }
                if (caller_info.type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    // check for constructor
                    if (auto fun_set = caller_info.type_info.type->get_variable("init")) {
                        _res_expr_info.reset();
                        _res_expr_info = resolve_call(&*cast<scope::FunctionSet>(fun_set), arg_infos, node);
                    } else {
                        throw ErrorGroup<AnalyzerError>()
                                .error(error(std::format("'{}' does not provide a constructor", caller_info.to_string()),
                                             &node))
                                .note(error("declared here", caller_info.type_info.type));
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
                            .error(error(
                                    std::format("cannot use safe call operator on non-nullable '{}'", caller_info.to_string()),
                                    &node))
                            .note(error("remove the safe call operator '?()'", &node));
                }
                // this is the actual thing: FUNCTION RESOLUTION
                _res_expr_info.reset();
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
        _res_expr_info.reset();
    }

    void Analyzer::visit(ast::expr::Slice &node) {
        // TODO: implement slices
    }

    void Analyzer::visit(ast::expr::Unary &node) {
        node.get_expr()->accept(this);
        auto expr_info = _res_expr_info;
        if (expr_info.is_null())
            throw error(std::format("cannot apply unary operator '{}' on 'null'", node.get_op()->get_text()), &node);
        switch (expr_info.tag) {
            case ExprInfo::Type::NORMAL: {
                auto type_info = expr_info.type_info;
                if (type_info.is_type_literal()) {
                    warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                    _res_expr_info.tag = ExprInfo::Type::NORMAL;
                    _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                    _res_expr_info.type_info.b_nullable = true;
                } else {
                    if (type_info.b_nullable) {
                        throw error(std::format("cannot apply unary operator '{}' on nullable type '{}'",
                                                node.get_op()->get_text(), type_info.type->to_string()),
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
                                // TODO: Check for overloaded operator ~
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
                                // TODO: Check for overloaded operator -
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
                                // TODO: Check for overloaded operator +
                                throw error(std::format("cannot apply unary operator '+' on '{}'", type_info.type->to_string()),
                                            &node);
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
                throw error(std::format("cannot apply unary operator '{}' on '{}'", node.get_op()->get_text(),
                                        expr_info.to_string()),
                            &node);
        }
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
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

        if (expr_info.is_null()) {
            if (node.get_safe()) {
                warning("expression is always 'null'", &node);
                _res_expr_info.value_info.b_null = true;
            } else
                throw error("cannot cast 'null'", &node);
        }

        if (type_cast_info.is_type_literal()) {
            warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
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

    void Analyzer::visit(ast::expr::Binary &node) {
        string op_str = (node.get_op1() ? node.get_op1()->get_text() : "") + (node.get_op2() ? node.get_op2()->get_text() : "");

        node.get_left()->accept(this);
        auto left_expr_info = _res_expr_info;
        node.get_right()->accept(this);
        auto right_expr_info = _res_expr_info;

        if (left_expr_info.is_null() || right_expr_info.is_null())
            throw error(std::format("cannot apply binary operator '{}' on 'null'", op_str), &node);
        if (((left_expr_info.tag == ExprInfo::Type::NORMAL || left_expr_info.tag == ExprInfo::Type::STATIC) &&
             left_expr_info.type_info.is_type_literal()) ||
            ((right_expr_info.tag == ExprInfo::Type::NORMAL || right_expr_info.tag == ExprInfo::Type::STATIC) &&
             right_expr_info.type_info.is_type_literal())) {
            warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
            _res_expr_info.tag = ExprInfo::Type::NORMAL;
            _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
            _res_expr_info.type_info.b_nullable = true;
        }
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
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
    }

    void Analyzer::visit(ast::expr::ChainBinary &node) {
        std::optional<ExprInfo> prev_expr_opt;
        size_t i = 0;
        for (const auto &cur_expr: node.get_exprs()) {
            cur_expr->accept(this);
            auto right_expr_info = _res_expr_info;
            if (prev_expr_opt) {
                auto left_expr_info = *prev_expr_opt;
                if (((left_expr_info.tag == ExprInfo::Type::NORMAL || left_expr_info.tag == ExprInfo::Type::STATIC) &&
                     left_expr_info.type_info.is_type_literal()) ||
                    ((right_expr_info.tag == ExprInfo::Type::NORMAL || right_expr_info.tag == ExprInfo::Type::STATIC) &&
                     right_expr_info.type_info.is_type_literal())) {
                    warning("'type' causes dynamic resolution", &node);
                } else {
                    string op_str = node.get_ops()[i - 1]->get_text();
                    string ov_op_str;
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
                            if (left_expr_info.tag != ExprInfo::Type::NORMAL || right_expr_info.tag != ExprInfo::Type::NORMAL)
                                throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str,
                                                        left_expr_info.to_string(), right_expr_info.to_string()),
                                            &node);
                            if (left_expr_info.type_info.b_nullable || right_expr_info.type_info.b_nullable)
                                throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str,
                                                        left_expr_info.to_string(), right_expr_info.to_string()),
                                            &node);
                            if ((left_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                                 left_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT]) &&
                                (right_expr_info.type_info.type == &*internals[Internal::SPADE_INT] ||
                                 right_expr_info.type_info.type == &*internals[Internal::SPADE_FLOAT])) {
                                // plain int|float <, <=, >=, > int|float
                            } else if (left_expr_info.type_info.type == &*internals[Internal::SPADE_STRING] &&
                                       right_expr_info.type_info.type == &*internals[Internal::SPADE_STRING]) {
                                // plain string <, <=, >=, > string
                            } else {
                                // TODO: check for overloaded operator `ov_op_str`
                                throw error(std::format("cannot apply binary operator '{}' on '{}' and '{}'", op_str,
                                                        left_expr_info.to_string(), right_expr_info.to_string()),
                                            &node);
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
            }
            prev_expr_opt = right_expr_info;
            i++;
        }
        _res_expr_info.tag = ExprInfo::Type::NORMAL;
        _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_BOOL]);
        _res_expr_info.value_info.b_lvalue = false;
        _res_expr_info.value_info.b_const = false;
    }

    void Analyzer::visit(ast::expr::Ternary &node) {
        node.get_condition()->accept(this);
        node.get_on_true()->accept(this);
        auto expr_info1 = _res_expr_info;
        node.get_on_false()->accept(this);
        auto expr_info2 = _res_expr_info;
        if (expr_info1.tag != expr_info2.tag)
            throw error("cannot infer type of the expression", &node);
        _res_expr_info.reset();
        switch (expr_info1.tag) {
            case ExprInfo::Type::NORMAL: {
                if (expr_info1.type_info.type != expr_info2.type_info.type)
                    if (!expr_info1.is_null() && expr_info2.is_null())
                        throw error("cannot infer type of the expression", &node);
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type =    // should get optimized out!!
                        expr_info1.is_null() ? (expr_info2.is_null() ? expr_info1.type_info.type : expr_info2.type_info.type)
                                             : (expr_info2.is_null() ? expr_info1.type_info.type : expr_info2.type_info.type);
                // TODO: check type args for covariance and contravariance
                // _res_expr_info.type_info.type_args = expr_info1.type_info.type_args;
                if (expr_info1.type_info.b_nullable || expr_info2.type_info.b_nullable)
                    _res_expr_info.type_info.b_nullable = true;
                break;
            }
            case ExprInfo::Type::STATIC:
                // expr returns 'type'
                _res_expr_info.type_info.type = null;
                _res_expr_info.type_info.type_args = {};
                if (expr_info1.type_info.b_nullable || expr_info2.type_info.b_nullable)
                    _res_expr_info.type_info.b_nullable = true;
                break;
            case ExprInfo::Type::MODULE:
                throw error("cannot infer type of the expression", &node);
            case ExprInfo::Type::FUNCTION_SET:
                // TODO: check if they have the same signature
                break;
        }
        _res_expr_info.value_info.b_lvalue = expr_info1.value_info.b_lvalue && expr_info2.value_info.b_lvalue;
        _res_expr_info.value_info.b_const = expr_info1.value_info.b_const || expr_info2.value_info.b_const;
    }

    void Analyzer::visit(ast::expr::Assignment &node) {
        std::vector<ExprInfo> assignees;
        std::vector<ExprInfo> exprs;

        for (const auto &assignee: node.get_assignees()) {
            assignee->accept(this);
            assignees.push_back(_res_expr_info);
            // avoid assigning `void`
            if ((_res_expr_info.tag == ExprInfo::Type::NORMAL || _res_expr_info.tag == ExprInfo::Type::STATIC) &&
                _res_expr_info.type_info.type == &*internals[Internal::SPADE_VOID])
                throw error(std::format("cannot assign to '{}'", _res_expr_info.to_string()), assignee);
        }

        for (const auto &expr: node.get_exprs()) {
            expr->accept(this);
            assignees.push_back(_res_expr_info);
            // avoid assigning `void`
            if ((_res_expr_info.tag == ExprInfo::Type::NORMAL || _res_expr_info.tag == ExprInfo::Type::STATIC) &&
                _res_expr_info.type_info.type == &*internals[Internal::SPADE_VOID])
                throw error(std::format("cannot assign '{}' to an object", _res_expr_info.to_string()), expr);
        }

        if (assignees.size() != exprs.size())
            throw error(std::format("expected {} values but got {}", assignees.size(), exprs.size()), &node);

        for (size_t i = 0; i < assignees.size(); i++) {
            auto left_expr_info = assignees[i];
            auto right_expr_info = exprs[i];
            if (node.get_op1()->get_type() == TokenType::EQUAL) {
                // Plain vanilla assignment
                if (!left_expr_info.value_info.b_lvalue)
                    throw error("cannot assign to a non-lvalue expression", node.get_assignees()[i]);
                if (left_expr_info.value_info.b_const)
                    throw error("cannot assign to a constant", node.get_assignees()[i]);
                if (left_expr_info.tag == ExprInfo::Type::NORMAL)
                    resolve_assign(left_expr_info.type_info, right_expr_info, node);
            } else {
                switch (node.get_op2()->get_type()) {
                    case TokenType::PLUS:
                        break;
                    case TokenType::DASH:
                        break;
                    case TokenType::STAR:
                        break;
                    case TokenType::SLASH:
                        break;
                    case TokenType::PERCENT:
                        break;
                    case TokenType::STAR_STAR:
                        break;
                    case TokenType::LSHIFT:
                        break;
                    case TokenType::RSHIFT:
                        break;
                    case TokenType::URSHIFT:
                        break;
                    case TokenType::AMPERSAND:
                        break;
                    case TokenType::PIPE:
                        break;
                    case TokenType::CARET:
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
        }
        // return the value of the last expression
        _res_expr_info = assignees.back();
    }
}    // namespace spade