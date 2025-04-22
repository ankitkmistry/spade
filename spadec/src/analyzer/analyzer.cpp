#include <algorithm>
#include <mutex>
#include <boost/functional/hash.hpp>

#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "spimp/error.hpp"
#include "spimp/utils.hpp"
#include "symbol_path.hpp"
#include "utils/error.hpp"

// TODO: implement generics

namespace spade
{
    scope::Scope *Analyzer::get_parent_scope() const {
        return cur_scope->get_parent();
    }

    scope::Scope *Analyzer::get_current_scope() const {
        return cur_scope;
    }

    scope::Function *Analyzer::get_current_function() const {
        return get_current_scope()->get_type() == scope::ScopeType::FUNCTION ? cast<scope::Function>(get_current_scope())
                                                                             : get_current_scope()->get_enclosing_function();
    }

    void Analyzer::load_internal_modules() {
        // module spade
        auto module = std::make_shared<scope::Module>(null);
        module->set_path(SymbolPath("spade"));

        const auto declare_class = [&](const string &name, Internal tag,
                                       const std::shared_ptr<scope::Compound> &super) -> std::shared_ptr<scope::Compound> {
            auto klass = std::make_shared<scope::Compound>(name);
            klass->set_path(module->get_path() / name);
            if (super)
                klass->inherit_from(&*super);
            module->new_variable(name, null, klass);
            internals[tag] = klass;
            return klass;
        };

        auto any_class = declare_class("any", Internal::SPADE_ANY, null);
        declare_class("Enum", Internal::SPADE_ENUM, any_class);
        declare_class("Annotation", Internal::SPADE_ANNOTATION, any_class);
        declare_class("Throwable", Internal::SPADE_THROWABLE, any_class);

        declare_class("int", Internal::SPADE_INT, any_class);
        declare_class("float", Internal::SPADE_FLOAT, any_class);
        declare_class("bool", Internal::SPADE_BOOL, any_class);
        declare_class("string", Internal::SPADE_STRING, any_class);
        declare_class("void", Internal::SPADE_VOID, any_class);

        module_scopes.emplace(null, ScopeInfo(module));
    }

    ExprInfo Analyzer::resolve_name(const string &name, const ast::AstNode &node) {
        ExprInfo expr_info;
        auto parent_compound = get_current_scope()->get_enclosing_compound();

        std::shared_ptr<scope::Scope> scope;
        for (auto itr = get_current_scope(); itr; itr = itr->get_parent()) {
            if (itr->get_type() == scope::ScopeType::COMPOUND) {
                auto compound = cast<scope::Compound>(itr);
                if (compound->get_eval() == scope::Compound::Eval::NOT_STARTED) {
                    auto old_cur_scope = get_current_scope();
                    cur_scope = compound->get_parent();
                    compound->get_node()->accept(this);
                    cur_scope = old_cur_scope;
                }
            }
            if (itr->has_variable(name)) {
                scope = itr->get_variable(name);
                break;
            } else if (parent_compound && itr == parent_compound) {
                if (parent_compound->get_super_fields().contains(name)) {
                    scope = parent_compound->get_super_fields().at(name);
                } else if (parent_compound->get_super_functions().contains(name)) {
                    expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                    expr_info.value_info.b_const = true;
                    expr_info.value_info.b_lvalue = true;
                    expr_info.functions = parent_compound->get_super_functions().at(name);
                    return expr_info;
                }
            }
        }
        // Check for null module
        if (!scope && module_scopes.at(null).get_scope()->has_variable(name)) {
            scope = module_scopes.at(null).get_scope()->get_variable(name);
        }
        // Yell if the scope cannot be located
        if (!scope)
            throw error("undefined reference", &node);
        // Resolve the context
        resolve_context(scope, node);
        switch (scope->get_type()) {
            case scope::ScopeType::FOLDER_MODULE:
            case scope::ScopeType::MODULE:
                expr_info.tag = ExprInfo::Type::MODULE;
                expr_info.value_info.b_const = true;
                expr_info.module = cast<scope::Module>(&*scope);
                break;
            case scope::ScopeType::COMPOUND:
                expr_info.tag = ExprInfo::Type::STATIC;
                expr_info.value_info.b_const = true;
                expr_info.type_info.type = cast<scope::Compound>(&*scope);
                break;
            case scope::ScopeType::FUNCTION:
                throw Unreachable();    // surely some scope tree builder error
            case scope::ScopeType::FUNCTION_SET:
                expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                expr_info.value_info.b_const = true;
                expr_info.functions = cast<scope::FunctionSet>(&*scope);
                break;
            case scope::ScopeType::BLOCK:
                throw Unreachable();    // surely some parser error
            case scope::ScopeType::VARIABLE:
                expr_info = get_var_expr_info(cast<scope::Variable>(scope), node);
                break;
            case scope::ScopeType::ENUMERATOR:
                expr_info.tag = ExprInfo::Type::NORMAL;
                expr_info.value_info.b_const = true;
                expr_info.type_info.type = scope->get_enclosing_compound();
                break;
        }
        expr_info.value_info.b_lvalue = true;
        return expr_info;
    }

    // This function check whether `scope` is accessible from `cur_scope`
    // It uses the accessor rules to determine accessibilty, the accessor rules are given as follows:
    //
    // +=======================================================================================================================+
    // |                                                   ACCESSORS                                                           |
    // +===================+===================================================================================================+
    // |   private         | same class                                                                                        |
    // |   internal        | same class, same module subclass                                                                  |
    // |   module private  | same class, same module subclass, same module                                                     |
    // |   protected       | same class, same module subclass, same module, other module subclass                              |
    // |   public          | same class, same module subclass, same module, other module subclass, other module non-subclass   |
    // +===================+===================================================================================================+
    //
    // If no accessor is provided then the default accessor is taken to be `module private`
    void Analyzer::resolve_context(const scope::Scope *from_scope, const scope::Scope *to_scope, const ast::AstNode &node,
                                   ErrorGroup<AnalyzerError> &errors) const {
        auto cur_mod = from_scope->get_enclosing_module();
        auto scope_mod = to_scope->get_enclosing_module();

        if (to_scope->get_type() == scope::ScopeType::FUNCTION_SET)
            return;    // spare function sets

        {    // static context code
            bool static_context = false;
            if (auto fun = from_scope->get_enclosing_function()) {
                static_context = fun->is_static();
            } else if (from_scope->get_type() == scope::ScopeType::VARIABLE) {
                static_context = cast<const scope::Variable>(from_scope)->is_static();
            }

            if (static_context) {
                switch (to_scope->get_type()) {
                    case scope::ScopeType::FOLDER_MODULE:    // modules can be referenced from static ctx
                    case scope::ScopeType::MODULE:           // modules can be referenced from static ctx
                    case scope::ScopeType::COMPOUND:         // compounds can be referenced from static ctx
                        break;
                    case scope::ScopeType::FUNCTION:    // only static functions can be referenced from static ctx
                        if (!cast<const scope::Function>(to_scope)->is_static()) {
                            errors.error(error(std::format("cannot access non-static '{}' from static context",
                                                           to_scope->to_string()),
                                               &node))
                                    .note(error("declared here", to_scope));
                            return;
                        }
                        break;
                    case scope::ScopeType::VARIABLE:    // only static variables can be referenced from static ctx
                        if (!cast<const scope::Variable>(to_scope)->is_static()) {
                            errors.error(error(std::format("cannot access non-static '{}' from static context",
                                                           to_scope->to_string()),
                                               &node))
                                    .note(error("declared here", to_scope));
                            return;
                        }
                        break;
                    case scope::ScopeType::ENUMERATOR:    // enumerators can be referenced from static ctx
                        break;
                    case scope::ScopeType::FUNCTION_SET:    // spare function sets
                    case scope::ScopeType::BLOCK:
                        throw Unreachable();
                }
            }
        }

        std::vector<std::shared_ptr<Token>> modifiers;
        switch (to_scope->get_type()) {
            case scope::ScopeType::FOLDER_MODULE:
            case scope::ScopeType::MODULE:
                return;
            case scope::ScopeType::COMPOUND:
            case scope::ScopeType::FUNCTION:
            case scope::ScopeType::VARIABLE:
            case scope::ScopeType::ENUMERATOR: {
                using namespace std::string_literals;
                if (!to_scope->get_node() && to_scope->get_enclosing_module()->get_path() == "spade"s) {
                    // if this belongs to internal module then do no context resolution
                    return;
                }
                modifiers = cast<ast::Declaration>(to_scope->get_node())->get_modifiers();
                break;
            }
            case scope::ScopeType::FUNCTION_SET:
            case scope::ScopeType::BLOCK:
                throw Unreachable();    // surely some parser error
        }
        for (const auto &modifier: modifiers) {
            switch (modifier->get_type()) {
                case TokenType::PRIVATE: {
                    // private here
                    auto cur_class = from_scope->get_enclosing_compound();
                    auto scope_class = to_scope->get_enclosing_compound();
                    if (!cur_class || cur_class != scope_class)
                        errors.error(error("cannot access 'private' member", &node)).note(error("declared here", to_scope));
                    return;
                }
                case TokenType::INTERNAL: {
                    // internal here
                    if (cur_mod != scope_mod) {
                        errors.error(error("cannot access 'internal' member", &node)).note(error("declared here", to_scope));
                        return;
                    }
                    auto cur_class = from_scope->get_enclosing_compound();
                    auto scope_class = to_scope->get_enclosing_compound();
                    if (!cur_class || cur_class != scope_class || !cur_class->has_super(scope_class))
                        errors.error(error("cannot access 'internal' member", &node)).note(error("declared here", to_scope));
                    return;
                }
                case TokenType::PROTECTED: {
                    auto cur_class = from_scope->get_enclosing_compound();
                    auto scope_class = to_scope->get_enclosing_compound();
                    if (cur_mod != scope_mod && (!cur_class || !cur_class->has_super(scope_class))) {
                        errors.error(error("cannot access 'protected' member", &node)).note(error("declared here", to_scope));
                        return;
                    }
                    // protected here
                    return;
                }
                case TokenType::PUBLIC:
                    // public here
                    // eat 5 star, do nothing
                    return;
                default:
                    break;
            }
        }
        // module private here
        if (cur_mod != scope_mod) {
            errors.error(error("cannot access 'module private' member", &node)).note(error("declared here", to_scope));
        }
    }

    void Analyzer::resolve_context(const scope::Scope *scope, const ast::AstNode &node) const {
        ErrorGroup<AnalyzerError> errors;
        resolve_context(get_current_scope(), scope, node, errors);
        if (!errors.get_errors().empty())
            throw errors;
    }

    void Analyzer::resolve_context(const std::shared_ptr<scope::Scope> scope, const ast::AstNode &node) const {
        ErrorGroup<AnalyzerError> errors;
        resolve_context(get_current_scope(), &*scope, node, errors);
        if (!errors.get_errors().empty())
            throw errors;
    }

    void Analyzer::check_cast(scope::Compound *from, scope::Compound *to, const ast::AstNode &node, bool safe) {
        if (from == null || to == null) {
            LOGGER.log_warn("check_cast: one of the scope::Compound is null, casting cannot be done");
            LOGGER.log_debug(
                    std::format("check_cast: from = {}, to = {}", (from ? "non-null" : "null"), (to ? "non-null" : "null")));
            return;
        }
        // take advantage of super classes
        if (from->has_super(to))
            return;
        // setup error state
        bool error_state = false;
        ErrorGroup<AnalyzerError> err_grp;
        if (safe)
            err_grp.warning(error("expression is always 'null'", &node));
        else
            err_grp.error(error(std::format("cannot cast '{}' to '{}'", from->to_string(), to->to_string()), &node));
        // duck typing
        // check if the members of 'to' is subset of members of 'from'
        for (const auto &[to_member_name, to_member]: to->get_members()) {
            const auto &[_, to_member_scope] = to_member;
            if (from->has_variable(to_member_name)) {
                auto from_member_scope = from->get_variable(to_member_name);
                // check if the scope type is same
                if (from_member_scope->get_type() == to_member_scope->get_type()) {
                    if (from_member_scope->get_type() == scope::ScopeType::COMPOUND) {
                        // check if they are the same type of compound (class, interface, enum, annotation)
                        if (cast<ast::decl::Compound>(from_member_scope->get_node())->get_token()->get_type() !=
                            cast<ast::decl::Compound>(to_member_scope->get_node())->get_token()->get_type()) {
                            if (!error_state)
                                error_state = true;
                            err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()),
                                               to_member_scope))
                                    .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(),
                                                            from->to_string()),
                                                from_member_scope));
                        }
                    } else if (from_member_scope->get_type() == scope::ScopeType::VARIABLE) {
                        // check if they are the same type of variable (var, const)
                        if (cast<ast::decl::Variable>(from_member_scope->get_node())->get_token()->get_type() !=
                            cast<ast::decl::Variable>(to_member_scope->get_node())->get_token()->get_type()) {
                            if (!error_state)
                                error_state = true;
                            err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()),
                                               to_member_scope))
                                    .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(),
                                                            from->to_string()),
                                                from_member_scope));
                        }
                    }
                } else {
                    if (!error_state)
                        error_state = true;
                    err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()),
                                       to_member_scope))
                            .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(), from->to_string()),
                                        from_member_scope));
                }
            } else {
                if (!error_state)
                    error_state = true;
                err_grp.note(error(std::format("'{}' does not have similar member like '{}'", from->to_string(),
                                               to_member_scope->to_string()),
                                   to_member_scope));
            }
        }
        if (error_state) {
            if (safe)
                printer.print(err_grp);
            else
                throw err_grp;
        }
        return;
    }

    TypeInfo Analyzer::resolve_assign(const TypeInfo &type_info, const ExprInfo &expr_info, const ast::AstNode &node) {
        TypeInfo result;
        // Check type inference
        switch (expr_info.tag) {
            case ExprInfo::Type::NORMAL:
                result = type_info;
                if (!expr_info.is_null() && type_info.type != expr_info.type_info.type &&
                    !expr_info.type_info.type->has_super(type_info.type))
                    throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info.to_string(),
                                            type_info.to_string()),
                                &node);
                if (!type_info.b_nullable && expr_info.type_info.b_nullable) {
                    expr_info.is_null()
                            ? throw error(std::format("cannot assign 'null' to type '{}'", type_info.to_string()), &node)
                            : throw error(std::format("cannot assign value of type '{}' to type '{}'",
                                                      expr_info.type_info.to_string(), type_info.to_string()),
                                          &node);
                }
                if (type_info.type_args.empty() && expr_info.type_info.type_args.empty()) {
                    // no type args, plain vanilla
                } else if (/* type_info.type_args.empty() &&  */ !expr_info.type_info.type_args.empty()) {
                    // deduce from type_info
                    result.type_args = expr_info.type_info.type_args;
                } else if (!type_info.type_args.empty() /*  && expr_info.type_info.type_args.empty() */) {
                    // deduce from expr_info
                    // TODO: check type args
                } else /* if(!type_info.type_args.empty() && !expr_info.type_info.type_args.empty()) */ {
                    if (type_info.type_args.size() != expr_info.type_info.type_args.size())
                        throw error(std::format("failed to deduce type arguments"), &node);    // failed deducing
                    // now both type_info and expr_info have typeargs (same size)
                    // check if both of them are same
                    // TODO: implement covariance and contravariance
                }
                break;
            case ExprInfo::Type::STATIC:
                result = type_info;
                if (!type_info.is_type_literal())
                    throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info.to_string(),
                                            type_info.to_string()),
                                &node);
                if (!type_info.b_nullable && expr_info.type_info.b_nullable)
                    throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info.to_string(),
                                            type_info.to_string()),
                                &node);
                break;
            case ExprInfo::Type::MODULE:
                throw error(std::format("cannot assign a module to type '{}'", type_info.to_string()), &node);
            case ExprInfo::Type::FUNCTION_SET:
                // TODO: implement function resolution
                break;
        }
        return result;
    }

    TypeInfo Analyzer::resolve_assign(std::shared_ptr<ast::Type> type, std::shared_ptr<ast::Expression> expr,
                                      const ast::AstNode &node) {
        TypeInfo type_info;
        if (type && expr) {
            type->accept(this);
            type_info = _res_type_info;

            if (auto scope = dynamic_cast<scope::Variable *>(get_current_scope())) {
                scope->set_type_info(type_info);
                scope->set_eval(scope::Variable::Eval::DONE);    // mimic as if type resolution is completed
            }

            expr->accept(this);
            auto expr_info = _res_expr_info;
            type_info = resolve_assign(type_info, expr_info, node);
        } else if (type) {
            type->accept(this);
            type_info = _res_type_info;
        } else if (expr) {
            expr->accept(this);
            switch (_res_expr_info.tag) {
                case ExprInfo::Type::NORMAL:
                    type_info = _res_expr_info.type_info;
                    break;
                case ExprInfo::Type::STATIC:
                    type_info.reset();    // `type` literal
                    break;
                case ExprInfo::Type::MODULE:
                    throw error("cannot assign a module to a variable", &node);
                case ExprInfo::Type::FUNCTION_SET:
                    // TODO: implement function types
                    throw Unreachable();
            }
        } else {
            // type_info.reset();
            type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
            // nullable by default
            type_info.b_nullable = true;
        }
        // Assigning to a variable, so set the type info
        if (auto scope = dynamic_cast<scope::Variable *>(get_current_scope())) {
            scope->set_type_info(type_info);
            scope->set_eval(scope::Variable::Eval::DONE);
        }
        return type_info;
    }

    bool Analyzer::check_fun_call(scope::Function *function, const std::vector<ArgInfo> &arg_infos, const ast::AstNode &node,
                                  ErrorGroup<AnalyzerError> &errors) {
        if (!function->is_variadic() && !function->is_default() && function->param_count() != arg_infos.size()) {
            errors.error(error(std::format("expected {} arguments but got {}", function->param_count(), arg_infos.size()),
                               &node))
                    .note(error("declared here", function));
            return false;
        }
        if (arg_infos.size() < function->min_param_count()) {
            errors.error(error(std::format("expected at least {} arguments but got {}", function->min_param_count(),
                                           arg_infos.size()),
                               &node))
                    .note(error("declared here", function));
            return false;
        }

        ErrorGroup<AnalyzerError> err_grp;

        // Separate out the value and keyword arguments
        std::vector<ArgInfo> value_args;
        std::unordered_map<string, ArgInfo> kwargs;
        for (const auto &arg_info: arg_infos) {
            if (arg_info.b_kwd)
                kwargs[arg_info.name] = arg_info;
            else
                value_args.emplace_back(arg_info);
        }

        size_t arg_id = 0;
        // Check positional only parameters
        if (value_args.size() < function->get_pos_only_params().size()) {
            err_grp.error(error(std::format("expected {} positional arguments but got {}",
                                            function->get_pos_only_params().size(), arg_infos.size()),
                                &node));
        } else
            for (const auto &param: function->get_pos_only_params()) {
                auto arg_info = value_args[arg_id];
                try {
                    resolve_assign(param.type_info, arg_info.expr_info, node);
                } catch (const AnalyzerError &err) {
                    err_grp.error(err);
                }
                arg_id++;
            }

        size_t min_kw_arg_count = 0;
        std::unordered_map<string, ParamInfo> kwd_params;
        std::optional<ParamInfo> kwd_only_variadic;

        // Consume pos_kwd parameters and then build kwd_params
        for (const auto &param: function->get_pos_kwd_params()) {
            if (arg_id >= value_args.size()) {
                if (!param.b_default && !param.b_variadic)
                    min_kw_arg_count++;
                if (!param.b_variadic)
                    kwd_params[param.name] = param;
            } else {
                do {
                    auto arg_info = value_args[arg_id];
                    try {
                        resolve_assign(param.type_info, arg_info.expr_info, node);
                    } catch (const AnalyzerError &err) {
                        err_grp.error(err);
                    }
                    arg_id++;
                } while (param.b_variadic && arg_id < value_args.size());
            }
        }
        // All value arguments should get consumed
        if (arg_id < value_args.size())
            err_grp.error(error(std::format("expected at most {} value arguments but got {} value arguments", arg_id,
                                            value_args.size()),
                                &node))
                    .note(error("declared here", function));
        // Add kwd_only_params to kwd_params map
        for (const auto &param: function->get_kwd_only_params()) {
            if (!param.b_default && !param.b_variadic)
                min_kw_arg_count++;
            if (param.b_variadic)
                kwd_only_variadic = param;
            else
                kwd_params[param.name] = param;
        }
        // Minimum keyword arguments should be present
        if (kwargs.size() < min_kw_arg_count)
            err_grp.error(error(std::format("expected at least {} keyword arguments but got {} keyword arguments",
                                            min_kw_arg_count, kwargs.size()),
                                &node))
                    .note(error("declared here", function));
        // Collect all keyword arguments
        for (const auto &[name, kwarg]: kwargs) {
            if (kwd_params.contains(name)) {
                try {
                    resolve_assign(kwd_params[name].type_info, kwarg.expr_info, node);
                } catch (const AnalyzerError &err) {
                    err_grp.error(err);
                }
                kwd_params.erase(name);
            } else if (kwd_only_variadic) {
                try {
                    resolve_assign(kwd_only_variadic->type_info, kwarg.expr_info, node);
                } catch (const AnalyzerError &err) {
                    err_grp.error(err);
                }
            } else
                err_grp.error(error(std::format("unexpected keyword argument '{}'", name), kwarg.node));
        }
        // give error for remaining keyword arguments if they are not default
        for (const auto &[name, param]: kwd_params) {
            if (!param.b_default) {
                if (param.b_kwd_only)
                    err_grp.error(error(std::format("missing required keyword argument '{}'", param.name), &node));
                else
                    err_grp.error(error(std::format("missing required argument '{}'", param.name), &node));
            }
        }
        if (!err_grp.get_errors().empty()) {
            err_grp.note(error("declared here", function));
            errors.extend(err_grp);
            return false;
        }
        return true;
    }

    ExprInfo Analyzer::resolve_call(const FunctionInfo &funs, const std::vector<ArgInfo> &arg_infos, const ast::AstNode &node) {
        // Check for redeclarations if any
        for (const auto &[_, fun_set]: funs.get_function_sets()) {
            if (!fun_set->is_redecl_check()) {
                // fun_set can never be empty (according to scope tree builder)
                auto old_cur_scope = get_current_scope();
                cur_scope = fun_set->get_parent();
                fun_set->get_members().begin()->second.second->get_node()->accept(this);
                cur_scope = old_cur_scope;
            }
        }

        ErrorGroup<AnalyzerError> err_grp;
        err_grp.error(error("call candidate cannot be resolved", &node));
        std::vector<scope::Function *> candidates;
        scope::Function *candidate = null;

        for (const auto &[_, fun]: funs.get_functions()) {
            if (check_fun_call(&*fun, arg_infos, node, err_grp))
                candidates.push_back(fun);
        }

        if (candidates.size() == 0)
            throw err_grp;
        else if (candidates.size() == 1) {
            candidate = candidates[0];
        } else if (candidates.size() > 1) {
            std::map<size_t, std::vector<scope::Function *>, std::less<size_t>> candidate_table;
            for (const auto &fun: candidates) {
                size_t priority = 0;
                if (fun->is_variadic())
                    priority = 1;
                else if (fun->is_default())
                    priority = 2;
                else {
                    size_t i = 0;
                    for (auto param: fun->get_pos_only_params()) {
                        if (priority == 3)
                            break;
                        if (param.type_info.type != arg_infos[i].expr_info.type_info.type)
                            priority = 3;
                        i++;
                    }
                    for (auto param: fun->get_pos_kwd_params()) {
                        if (priority == 3)
                            break;
                        if (param.type_info.type != arg_infos[i].expr_info.type_info.type)
                            priority = 3;
                        i++;
                    }
                    for (auto param: fun->get_kwd_only_params()) {
                        if (priority == 3)
                            break;
                        if (param.type_info.type != arg_infos[i].expr_info.type_info.type)
                            priority = 3;
                        i++;
                    }
                    priority = priority == 0 ? 4 : 3;
                }
                candidate_table[priority].push_back(fun);
            }
            candidates = candidate_table.rbegin()->second;
            if (candidates.size() > 1) {
                ErrorGroup<AnalyzerError> err_grp;
                err_grp.error(error(std::format("ambiguous call to '{}'", funs.to_string()), &node));
                for (const auto &fun: candidates)
                    err_grp.note(error(std::format("possible candidate declared here: '{}'", fun->to_string()),
                                       fun->get_node()))
                            .note(error("this error should not have occurred, please raise a github issue"));
                throw err_grp;
            }
            candidate = candidates[0];
        }

        resolve_context(candidate, node);

        LOGGER.log_debug(std::format("resolved call candidate: {}", candidate->to_string()));

        ExprInfo expr_info;
        expr_info.tag = ExprInfo::Type::NORMAL;
        expr_info.type_info = candidate->get_ret_type();
        return expr_info;
    }

    ExprInfo Analyzer::get_var_expr_info(std::shared_ptr<scope::Variable> var_scope, const ast::AstNode &node) {
        ExprInfo expr_info;
        expr_info.tag = ExprInfo::Type::NORMAL;
        switch (var_scope->get_eval()) {
            case scope::Variable::Eval::NOT_STARTED: {
                auto old_cur_scope = get_current_scope();    // save context
                cur_scope = var_scope->get_parent();         // change context
                var_scope->get_node()->accept(this);         // visit variable
                cur_scope = old_cur_scope;                   // reset context
                expr_info.type_info = var_scope->get_type_info();
                break;
            }
            case scope::Variable::Eval::PROGRESS:
                if (get_current_scope()->get_type() == scope::ScopeType::VARIABLE) {
                    auto cur_var_scope = cast<scope::Variable>(get_current_scope());
                    if (cur_var_scope->get_eval() == scope::Variable::Eval::DONE) {
                        expr_info.type_info = cur_var_scope->get_type_info();    // sense correct
                        break;
                    }
                }
                expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                expr_info.type_info.b_nullable = true;
                warning(std::format("type inference is ambiguous, defaulting to '{}'", expr_info.type_info.to_string()), &node);
                note("declared here", var_scope);
                break;
            case scope::Variable::Eval::DONE:
                expr_info.type_info = var_scope->get_type_info();
                break;
        }
        if (var_scope->get_variable_node()->get_token()->get_type() == TokenType::CONST)
            expr_info.value_info.b_const = true;
        return expr_info;
    }

    std::shared_ptr<scope::Variable> Analyzer::declare_variable(const std::shared_ptr<Token> &name) {
        std::shared_ptr<scope::Variable> scope;
        if (auto fun = get_current_function()) {
            // Check if the variable is not overshadowing parameters
            if (fun->has_param(name->get_text()))
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("function parameters cannot be overshadowed '{}'", name->get_text()), name))
                        .note(error("already declared here", fun->get_param(name->get_text()).node));
            // Add the variable to the parent scope
            if (!get_parent_scope()->new_variable(name, scope))
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("redeclaration of '{}'", name->get_text()), name))
                        .note(error("already declared here", scope));
        }
        return scope;
    }

    static bool check_fun_kwd_params(const scope::Function *fun1, const std::vector<ParamInfo> &fun1_pos_kwd,
                                     const std::vector<ParamInfo> &fun1_pos_kwd_default, const scope::Function *fun2,
                                     const std::vector<ParamInfo> &fun2_pos_kwd,
                                     const std::vector<ParamInfo> &fun2_pos_kwd_default);

    // In functions, there are three kinds of parameters:
    // - Positional only        `pos_only`
    // - Keyword or positional  `pos_kwd`
    // - Keyword only           `kwd_only`
    //
    // `pos_only` parameters cannot be variadic or have default values.
    // `pos_kwd` parameters can be variadic or have default values.
    // `kwd_only` parameters can be variadic or have default values.
    //
    // In any parameter list, variadic parameter is present in the last index (if any)
    // and default parameters are always the last few items in the list (if any).
    //
    // Here for every kind of parameter, we separate out the default parameters and
    // variadic ones from the required ones. After this, the following are formed for both the functions:
    // - [0] vector      pos_only           (required)
    // - [1] vector      pos_kwd            (required)
    // - [2] vector      pos_kwd_default
    // - [3] optional    pos_kwd_variadic
    // - [4] vector      kwd_only           (required)
    // - [5] vector      kwd_only_default
    // - [6] optional    kwd_only_variadic
    //
    // Then, each of the items in the above list is evaluated accordingly.
    void Analyzer::check_funs(const scope::Function *fun1, const scope::Function *fun2,
                              ErrorGroup<AnalyzerError> &errors) const {
#define AMBIGUOUS()                                                                                                            \
    do {                                                                                                                       \
        errors.error(error(std::format("ambiguous declaration of '{}'", fun1->to_string()), fun1))                             \
                .note(error(std::format("check another declaration here: '{}'", fun2->to_string()), fun2));                    \
        return;                                                                                                                \
    } while (false)

        if (fun1->get_function_node()->get_name()->get_text() != fun2->get_function_node()->get_name()->get_text())
            return;
        if (fun1->min_param_count() == 0 && fun2->min_param_count() == 0)
            AMBIGUOUS();
        if (!fun1->is_default() && !fun1->is_variadic() && !fun2->is_default() && !fun2->is_variadic())
            if (fun1->param_count() != fun2->param_count())
                return;

        auto fun1_pos_only = fun1->get_pos_only_params();
        std::vector<ParamInfo> fun1_pos_kwd;
        std::vector<ParamInfo> fun1_pos_kwd_default;
        std::optional<ParamInfo> fun1_pos_kwd_variadic;
        for (const auto &param: fun1->get_pos_kwd_params()) {
            if (param.b_variadic)
                fun1_pos_kwd_variadic = param;
            else if (param.b_default)
                fun1_pos_kwd_default.push_back(param);
            else
                fun1_pos_kwd.push_back(param);
        }

        auto fun2_pos_only = fun2->get_pos_only_params();
        std::vector<ParamInfo> fun2_pos_kwd;
        std::vector<ParamInfo> fun2_pos_kwd_default;
        std::optional<ParamInfo> fun2_pos_kwd_variadic;
        for (const auto &param: fun2->get_pos_kwd_params()) {
            if (param.b_variadic)
                fun2_pos_kwd_variadic = param;
            else if (param.b_default)
                fun2_pos_kwd_default.push_back(param);
            else
                fun2_pos_kwd.push_back(param);
        }

        {    // Check positional only parameters (with also overlapping pos-kwd parameters)
            if (!fun1_pos_only.empty() && !fun2_pos_only.empty()) {
                for (size_t i = 0; i < fun1_pos_only.size() && i < fun2_pos_only.size(); i++) {
                    if (fun1_pos_only[i].type_info.type != fun2_pos_only[i].type_info.type)
                        return;
                }
                size_t min_size = std::min(fun1_pos_only.size(), fun2_pos_only.size());
                fun1_pos_only.erase(fun1_pos_only.begin(), fun1_pos_only.begin() + min_size);
                fun2_pos_only.erase(fun2_pos_only.begin(), fun2_pos_only.begin() + min_size);
            }
            if (!fun1_pos_only.empty() && !fun2_pos_kwd.empty()) {
                for (size_t i = 0; i < fun1_pos_only.size() && i < fun2_pos_kwd.size(); i++) {
                    if (fun1_pos_only[i].type_info.type != fun2_pos_kwd[i].type_info.type)
                        return;
                }
                size_t min_size = std::min(fun1_pos_only.size(), fun2_pos_kwd.size());
                fun1_pos_only.erase(fun1_pos_only.begin(), fun1_pos_only.begin() + min_size);
                fun2_pos_kwd.erase(fun2_pos_kwd.begin(), fun2_pos_kwd.begin() + min_size);
            }
            if (!fun1_pos_kwd.empty() && !fun2_pos_only.empty()) {
                for (size_t i = 0; i < fun1_pos_kwd.size() && i < fun2_pos_only.size(); i++) {
                    if (fun1_pos_kwd[i].type_info.type != fun2_pos_only[i].type_info.type)
                        return;
                }
                size_t min_size = std::min(fun1_pos_kwd.size(), fun2_pos_only.size());
                fun1_pos_kwd.erase(fun1_pos_kwd.begin(), fun1_pos_kwd.begin() + min_size);
                fun2_pos_only.erase(fun2_pos_only.begin(), fun2_pos_only.begin() + min_size);
            }
            if (!fun1_pos_only.empty() || !fun2_pos_only.empty())
                return;
        }

        {    // Check pos-kwd parameters
            if (!fun1_pos_kwd.empty() && !fun2_pos_kwd.empty()) {
                for (size_t i = 0; i < fun1_pos_kwd.size() && i < fun2_pos_kwd.size(); i++) {
                    if (fun1_pos_kwd[i].type_info.type != fun2_pos_kwd[i].type_info.type) {
                        if (check_fun_kwd_params(fun1, fun1_pos_kwd, fun1_pos_kwd_default, fun2, fun2_pos_kwd,
                                                 fun2_pos_kwd_default))
                            return;
                        else
                            AMBIGUOUS();
                    }
                }
                if (fun1_pos_kwd.size() == fun2_pos_kwd.size())
                    AMBIGUOUS();
            }
            if (!fun1_pos_kwd_default.empty() || !fun1_pos_kwd_default.empty()) {
                // treat default as normal parameters
                for (size_t i = std::min(fun1_pos_kwd.size(), fun2_pos_kwd.size());
                     i < fun1_pos_kwd.size() + fun1_pos_kwd_default.size() &&
                     i < fun2_pos_kwd.size() + fun2_pos_kwd_default.size();
                     i++) {
                    auto param1 = i < fun1_pos_kwd.size() ? fun1_pos_kwd[i] : fun1_pos_kwd_default[i - fun1_pos_kwd.size()];
                    auto param2 = i < fun2_pos_kwd.size() ? fun2_pos_kwd[i] : fun2_pos_kwd_default[i - fun2_pos_kwd.size()];
                    if (i >= fun1_pos_kwd.size() && i >= fun2_pos_kwd.size()) {
                        // if both are default then types must be different
                        if (param1.type_info.type == param2.type_info.type)
                            AMBIGUOUS();
                    } else if (param1.type_info.type != param2.type_info.type) {
                        if (check_fun_kwd_params(fun1, fun1_pos_kwd, fun1_pos_kwd_default, fun2, fun2_pos_kwd,
                                                 fun2_pos_kwd_default))
                            return;
                        else
                            AMBIGUOUS();
                    }
                }
                AMBIGUOUS();
            }
            if (fun1_pos_kwd_variadic && fun2_pos_kwd_variadic)
                AMBIGUOUS();
        }

        // Check keyword arguments
        if (!check_fun_kwd_params(fun1, fun1_pos_kwd, fun1_pos_kwd_default, fun2, fun2_pos_kwd, fun2_pos_kwd_default))
            AMBIGUOUS();

            // AMBIGUOUS();
#undef AMBIGUOUS
    }

    static bool check_fun_kwd_params(const scope::Function *fun1, const std::vector<ParamInfo> &fun1_pos_kwd,
                                     const std::vector<ParamInfo> &fun1_pos_kwd_default, const scope::Function *fun2,
                                     const std::vector<ParamInfo> &fun2_pos_kwd,
                                     const std::vector<ParamInfo> &fun2_pos_kwd_default) {
        std::optional<ParamInfo> fun1_kwd_only_variadic;
        std::unordered_map<string, ParamInfo> fun1_kwd;
        for (const auto &param: fun1_pos_kwd) fun1_kwd[param.name] = param;
        for (const auto &param: fun1_pos_kwd_default) fun1_kwd[param.name] = param;
        for (const auto &param: fun1->get_kwd_only_params()) {
            if (param.b_variadic)
                fun1_kwd_only_variadic = param;
            else
                fun1_kwd[param.name] = param;
        }
        std::unordered_map<string, ParamInfo> fun2_kwd;
        std::optional<ParamInfo> fun2_kwd_only_variadic;
        for (const auto &param: fun2_pos_kwd) fun2_kwd[param.name] = param;
        for (const auto &param: fun2_pos_kwd_default) fun2_kwd[param.name] = param;
        for (const auto &param: fun2->get_kwd_only_params()) {
            if (param.b_variadic)
                fun2_kwd_only_variadic = param;
            else
                fun2_kwd[param.name] = param;
        }
        // Check keyword parameters
        for (const auto &[name, param]: fun1_kwd) {
            if (fun2_kwd.contains(name)) {
                if (param.b_default) {
                    if (param.type_info.type == fun2_kwd[name].type_info.type)
                        return false;
                } else if (param.type_info.type != fun2_kwd[name].type_info.type)
                    return true;
                fun2_kwd.erase(name);
            } else if (param.b_default)
                continue;
            else if (!fun2_kwd_only_variadic || param.type_info.type != fun2_kwd_only_variadic->type_info.type)
                return true;
        }
        for (const auto &[name, param]: fun2_kwd) {
            if (param.b_default)
                continue;
            else if (!fun2_kwd_only_variadic || param.type_info.type != fun2_kwd_only_variadic->type_info.type)
                return true;
        }
        return false;
    }

    void Analyzer::check_fun_set(const std::shared_ptr<scope::FunctionSet> &fun_set) {
        auto old_cur_scope = get_current_scope();
        cur_scope = &*fun_set;

        ErrorGroup<AnalyzerError> err_grp;

        if (fun_set->get_members().size() < 5) {
            // sequential algorithm
            for (auto it1 = fun_set->get_members().begin(); it1 != fun_set->get_members().end(); ++it1) {
                auto fun1 = cast<scope::Function>(it1->second.second);
                for (auto it2 = std::next(it1); it2 != fun_set->get_members().end(); ++it2) {
                    auto fun2 = cast<scope::Function>(it2->second.second);
                    check_funs(&*fun1, &*fun2, err_grp);
                }
            }
        } else {
            // parallel algorithm
            using FunOperand = std::pair<std::shared_ptr<scope::Function>, std::shared_ptr<scope::Function>>;

            std::vector<FunOperand> functions;
            // Reserve space for the number of combinations
            // Number of combinations = nC2 = n(n-1)/2
            // where n is the number of functions in the set
            functions.reserve(fun_set->get_members().size() * (fun_set->get_members().size() - 1) / 2);
            for (auto it1 = fun_set->get_members().begin(); it1 != fun_set->get_members().end(); ++it1) {
                auto fun1 = cast<scope::Function>(it1->second.second);
                for (auto it2 = std::next(it1); it2 != fun_set->get_members().end(); ++it2) {
                    auto fun2 = cast<scope::Function>(it2->second.second);
                    functions.emplace_back(fun1, fun2);
                }
            }

            std::mutex err_grp_mutex;
            std::for_each(std::execution::par, functions.begin(), functions.end(), [&](const FunOperand &item) {
                ErrorGroup<AnalyzerError> errors;
                check_funs(&*item.first, &*item.second, errors);
                if (!errors.get_errors().empty()) {
                    std::lock_guard lg(err_grp_mutex);
                    err_grp.extend(errors);
                }
            });
        }

        // Set qualified names
        std::unordered_map<string, scope::Scope::Member> new_members;
        for (const auto &[_1, member]: fun_set->get_members()) {
            const auto &[_2, scope] = member;
            string full_name = scope->to_string(false);
            string name = full_name.substr(0, full_name.find_first_of('('));
            auto final_path = SymbolPath(name) + full_name.substr(full_name.find_first_of('('));
            scope->set_path(final_path);
            cast<scope::Function>(scope)->get_function_node()->set_qualified_name(final_path.get_name());
            new_members[final_path.get_name()] = member;
        }

        fun_set->set_members(new_members);

        if (!err_grp.get_errors().empty())
            throw err_grp;
        cur_scope = old_cur_scope;
    }

    void Analyzer::analyze() {
        load_internal_modules();

        // Print scope tree
        // for (auto [_, module_scope_info]: module_scopes) {
        //     if (module_scope_info.is_original()) {
        //         module_scope_info.get_scope()->print();
        //     }
        // }
        // Start analysis
        mode = Mode::DECLARATION;
        for (auto [_, module_scope_info]: module_scopes) {
            if (module_scope_info.is_original())
                if (auto node = module_scope_info.get_scope()->get_node()) {
                    cur_scope = null;
                    node->accept(this);
                }
        }
        mode = Mode::DEFINITION;
        for (auto function: function_scopes) {
            cur_scope = function->get_parent()->get_parent();
            function->get_node()->accept(this);
        }
    }

    void Analyzer::visit(ast::Reference &node) {
        // Find the scope where name is located
        scope::Scope *scope = null;
        auto expr_info = resolve_name(node.get_path()[0]->get_text(), node);
        switch (expr_info.tag) {
            case ExprInfo::Type::NORMAL:
                scope = expr_info.type_info.type;
                break;
            case ExprInfo::Type::STATIC:
                scope = expr_info.type_info.type;
                break;
            case ExprInfo::Type::MODULE:
                scope = expr_info.module;
                break;
            case ExprInfo::Type::FUNCTION_SET:
                break;
        }
        if (!node.get_path().empty() && !scope)
            throw error("functions do not have members", &node);
        // Now check for references inside the scope
        for (size_t i = 1; i < node.get_path().size(); i++) {
            auto path_element = node.get_path()[i]->get_text();
            if (!scope->has_variable(path_element))
                throw error("undefined reference", &node);
            scope = &*scope->get_variable(path_element);
        }
        _res_expr_info.reset();
        _res_expr_info.value_info = expr_info.value_info;
        switch (scope->get_type()) {
            case scope::ScopeType::FOLDER_MODULE:
            case scope::ScopeType::MODULE:
                _res_expr_info.tag = ExprInfo::Type::MODULE;
                _res_expr_info.module = cast<scope::Module>(scope);
                break;
            case scope::ScopeType::COMPOUND:
                _res_expr_info.tag = ExprInfo::Type::STATIC;
                _res_expr_info.type_info.type = cast<scope::Compound>(scope);
                break;
            case scope::ScopeType::FUNCTION:
                throw Unreachable();
            case scope::ScopeType::FUNCTION_SET:
                _res_expr_info.tag = ExprInfo::Type::FUNCTION_SET;
                _res_expr_info.functions = expr_info.functions;
                break;
            case scope::ScopeType::BLOCK:
                throw Unreachable();
            case scope::ScopeType::VARIABLE:
            case scope::ScopeType::ENUMERATOR:
                _res_expr_info.tag = ExprInfo::Type::NORMAL;
                _res_expr_info.type_info.type = cast<scope::Compound>(scope);
                break;
        }
    }

    void Analyzer::visit(ast::type::Reference &node) {
        // Find the type scope
        node.get_reference()->accept(this);
        // Check if the reference is a type
        if (_res_expr_info.tag != ExprInfo::Type::STATIC)
            throw error("reference is not a type", &node);
        auto type_scope = _res_expr_info.type_info.type;
        // Check for type arguments
        std::vector<TypeInfo> type_args;
        for (auto type_arg: node.get_type_args()) {
            type_arg->accept(this);
            type_args.push_back(_res_type_info);
        }
        _res_type_info.reset();
        _res_type_info.type = type_scope;
        _res_type_info.type_args = type_args;
    }

    void Analyzer::visit(ast::type::Function &node) {
        _res_type_info.reset();
    }

    void Analyzer::visit(ast::type::TypeLiteral &node) {
        _res_type_info.reset();
    }

    void Analyzer::visit(ast::type::BinaryOp &node) {
        _res_type_info.reset();
    }

    void Analyzer::visit(ast::type::Nullable &node) {
        _res_type_info.reset();
        node.get_type()->accept(this);
        _res_type_info.b_nullable = true;
    }

    void Analyzer::visit(ast::type::TypeBuilder &node) {
        _res_type_info.reset();
    }

    void Analyzer::visit(ast::type::TypeBuilderMember &node) {
        // _res_type_info.reset();
    }
}    // namespace spade