#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "scope_tree.hpp"
#include "spimp/error.hpp"
#include "symbol_path.hpp"
#include "utils/error.hpp"
#include <algorithm>
#include <memory>

// TODO: implement generics

namespace ranges = std::ranges;

namespace spade
{
    void Analyzer::load_internal_modules() {
        // module spade
        auto module = std::make_shared<scope::Module>(null);
        module->set_path(SymbolPath("spade"));

        // class any
        auto any_class = std::make_shared<scope::Compound>("any");
        any_class->set_path(SymbolPath("spade.any"));
        module->new_variable("any", null, any_class);
        internals[Internal::SPADE_ANY] = any_class;
        // class int
        auto int_class = std::make_shared<scope::Compound>("int");
        int_class->set_path(SymbolPath("spade.int"));
        int_class->inherit_from(any_class);
        module->new_variable("int", null, int_class);
        internals[Internal::SPADE_INT] = int_class;
        // class float
        auto float_class = std::make_shared<scope::Compound>("float");
        float_class->set_path(SymbolPath("spade.float"));
        float_class->inherit_from(any_class);
        module->new_variable("float", null, float_class);
        internals[Internal::SPADE_FLOAT] = float_class;
        // class bool
        auto bool_class = std::make_shared<scope::Compound>("bool");
        bool_class->set_path(SymbolPath("spade.bool"));
        bool_class->inherit_from(any_class);
        module->new_variable("bool", null, bool_class);
        internals[Internal::SPADE_BOOL] = bool_class;
        // class string
        auto string_class = std::make_shared<scope::Compound>("string");
        string_class->set_path(SymbolPath("spade.string"));
        string_class->inherit_from(any_class);
        module->new_variable("string", null, string_class);
        internals[Internal::SPADE_STRING] = string_class;
        // class void
        auto void_class = std::make_shared<scope::Compound>("void");
        void_class->set_path(SymbolPath("spade.void"));
        void_class->inherit_from(any_class);
        module->new_variable("void", null, void_class);
        internals[Internal::SPADE_VOID] = void_class;

        module_scopes.emplace(null, ScopeInfo(module));
    }

    scope::Scope *Analyzer::get_parent_scope() const {
        return cur_scope->get_parent();
    }

    scope::Scope *Analyzer::get_current_scope() const {
        return cur_scope;
    }

    std::shared_ptr<scope::Scope> Analyzer::find_name(const string &name) const {
        std::shared_ptr<scope::Scope> scope;
        for (auto itr = get_current_scope(); itr != null; itr = itr->get_parent()) {
            if (itr->has_variable(name)) {
                scope = itr->get_variable(name);
                break;
            }
        }
        // Check for null module
        if (!scope && module_scopes.at(null).get_scope()->has_variable(name)) {
            scope = module_scopes.at(null).get_scope()->get_variable(name);
        }
        return scope;
    }

    void Analyzer::resolve_context(const std::shared_ptr<scope::Scope> &scope, const ast::AstNode &node) {
        /*
            +=======================================================================================================================+
            |                                                   ACCESSORS                                                           |
            +===================+===================================================================================================+
            |   private         | same class                                                                                        | 
            |   internal        | same class, same module subclass                                                                  | 
            |   module private  | same class, same module subclass, same module                                                     | 
            |   protected       | same class, same module subclass, same module, other module subclass                              | 
            |   public          | same class, same module subclass, same module, other module subclass, other module non-subclass   |
            +===================+===================================================================================================+

            default acessor is module private
        */
        auto cur_mod = get_current_scope()->get_enclosing_module();
        auto scope_mod = scope->get_enclosing_module();
        if (!cur_mod || !scope_mod)
            throw Unreachable();    // this cannot happen

        std::vector<std::shared_ptr<Token>> modifiers;
        // scope is a variable of scope::Compound
        // scope.get_enclosing_compound() is never null
        switch (scope->get_type()) {
            case scope::ScopeType::COMPOUND:
            case scope::ScopeType::FUNCTION:
            case scope::ScopeType::VARIABLE:
            case scope::ScopeType::ENUMERATOR:
                modifiers = cast<ast::Declaration>(scope->get_node())->get_modifiers();
                break;
            default:
                throw Unreachable();    // surely some parser error
        }
        for (const auto &modifier: modifiers) {
            switch (modifier->get_type()) {
                case TokenType::PRIVATE: {
                    // private here
                    auto cur_class = get_current_scope()->get_enclosing_compound();
                    auto scope_class = scope->get_enclosing_compound();
                    if (!cur_class || cur_class != scope_class)
                        throw ErrorGroup<AnalyzerError>()
                                .error(error("cannot access 'private' member", &node))
                                .note(error("declared here", scope));
                    return;
                }
                case TokenType::INTERNAL: {
                    // internal here
                    if (cur_mod != scope_mod) {
                        throw ErrorGroup<AnalyzerError>()
                                .error(error("cannot access 'internal' member", &node))
                                .note(error("declared here", scope));
                    }
                    auto cur_class = get_current_scope()->get_enclosing_compound();
                    auto scope_class = scope->get_enclosing_compound();
                    if (!cur_class || cur_class != scope_class || !cur_class->has_super(scope_class))
                        throw ErrorGroup<AnalyzerError>()
                                .error(error("cannot access 'internal' member", &node))
                                .note(error("declared here", scope));
                    return;
                }
                case TokenType::PROTECTED: {
                    auto cur_class = get_current_scope()->get_enclosing_compound();
                    auto scope_class = scope->get_enclosing_compound();
                    if (cur_mod != scope_mod && (!cur_class || !cur_class->has_super(scope_class))) {
                        throw ErrorGroup<AnalyzerError>()
                                .error(error("cannot access 'protected' member", &node))
                                .note(error("declared here", scope));
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
            throw ErrorGroup<AnalyzerError>()
                    .error(error("cannot access 'module private' member", &node))
                    .note(error("declared here", scope));
        }
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
            const auto &[to_member_decl_site, to_member_scope] = to_member;
            if (from->has_variable(to_member_name)) {
                auto from_member_decl_site = from->get_decl_site(to_member_name);
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
                                               to_member_decl_site))
                                    .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(),
                                                            from->to_string()),
                                                from_member_decl_site));
                        }
                    } else if (from_member_scope->get_type() == scope::ScopeType::VARIABLE) {
                        // check if they are the same type of variable (var, const)
                        if (cast<ast::decl::Variable>(from_member_scope->get_node())->get_token()->get_type() !=
                            cast<ast::decl::Variable>(to_member_scope->get_node())->get_token()->get_type()) {
                            if (!error_state)
                                error_state = true;
                            err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()),
                                               to_member_decl_site))
                                    .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(),
                                                            from->to_string()),
                                                from_member_decl_site));
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

    TypeInfo Analyzer::resolve_assign(const TypeInfo *type_info, const ExprInfo *expr_info, const ast::AstNode &node) {
        TypeInfo result;
        // Check type inference
        switch (expr_info->tag) {
            case ExprInfo::Type::NORMAL:
                if (type_info) {
                    result = *type_info;
                    if (!expr_info->is_null() && type_info->type != expr_info->type_info.type &&
                        !expr_info->type_info.type->has_super(type_info->type))
                        throw error(std::format("cannot assign value of type '{}' to type '{}'",
                                                expr_info->type_info.to_string(), type_info->to_string()),
                                    &node);
                    if (!type_info->b_nullable && expr_info->type_info.b_nullable) {
                        expr_info->is_null()
                                ? throw error(std::format("cannot assign 'null' to type '{}'", type_info->to_string()), &node)
                                : throw error(std::format("cannot assign value of type '{}' to type '{}'",
                                                          expr_info->type_info.to_string(), type_info->to_string()),
                                              &node);
                    }
                    if (type_info->type_args.empty() && expr_info->type_info.type_args.empty()) {
                        // no type args, plain vanilla
                    } else if (/* type_info.type_args.empty() &&  */ !expr_info->type_info.type_args.empty()) {
                        // deduce from type_info
                        result.type_args = expr_info->type_info.type_args;
                    } else if (!type_info->type_args.empty() /*  && expr_info.type_info.type_args.empty() */) {
                        // deduce from expr_info
                        // TODO: check type args
                    } else /* if(!type_info.type_args.empty() && !expr_info.type_info.type_args.empty()) */ {
                        if (type_info->type_args.size() != expr_info->type_info.type_args.size())
                            throw error(std::format("failed to deduce type arguments"), &node);    // failed deducing
                        // now both type_info and expr_info have typeargs (same size)
                        // check if both of them are same
                        // TODO: implement covariance and contravariance
                    }
                } else {
                    // deduce variable type from expression
                    result = expr_info->type_info;
                }
                break;
            case ExprInfo::Type::STATIC:
                if (type_info) {
                    result = *type_info;
                    if (!type_info->is_type_literal())
                        throw error(std::format("cannot assign value of type '{}' to type '{}'",
                                                expr_info->type_info.to_string(), type_info->to_string()),
                                    &node);
                    if (!type_info->b_nullable && expr_info->type_info.b_nullable)
                        throw error(std::format("cannot assign value of type '{}' to type '{}'",
                                                expr_info->type_info.to_string(), type_info->to_string()),
                                    &node);
                } else {
                    // deduce variable type as type literal, also set nullable if any
                    result.reset();
                    result.b_nullable = expr_info->type_info.b_nullable;
                }
                break;
            case ExprInfo::Type::MODULE:
                if (type_info)
                    throw error(std::format("cannot assign a module to type '{}'", type_info->to_string()), &node);
                else
                    throw error(std::format("cannot assign a module", type_info->to_string()), &node);
            case ExprInfo::Type::FUNCTION_SET:
                // TODO: implement function resolution
                break;
        }
        return result;
    }

    TypeInfo Analyzer::resolve_assign(std::shared_ptr<ast::Type> type, std::shared_ptr<ast::Expression> expr,
                                      const ast::AstNode &node) {
        TypeInfo type_info;
        if (type) {
            type->accept(this);
            type_info = _res_type_info;

            if (auto scope = dynamic_cast<scope::Variable *>(get_current_scope())) {
                scope->set_type_info(type_info);
                scope->set_eval(scope::Variable::Eval::DONE);    // mimic as if type resolution is completed
            }
        }
        if (expr) {
            expr->accept(this);
            auto expr_info = _res_expr_info;
            type_info = resolve_assign(type ? &type_info : null, &expr_info, node);
        }
        if (!type && !expr) {
            // type_info.reset();
            type_info.type = cast<scope::Compound>(&*internals[Analyzer::Internal::SPADE_ANY]);
            // non nullable by default
        }
        // Assigning to a variable, so set the type info
        if (auto scope = dynamic_cast<scope::Variable *>(get_current_scope())) {
            if (scope->get_eval() != scope::Variable::Eval::DONE) {
                scope->set_type_info(type_info);
                scope->set_eval(scope::Variable::Eval::DONE);
            }
        }
        return type_info;
    }

    std::vector<std::shared_ptr<scope::Function>> Analyzer::resolve_call_candidates(scope::FunctionSet *fun_set,
                                                                                    std::vector<ArgInfo> arg_infos,
                                                                                    const ast::expr::Call &node,
                                                                                    ErrorGroup<AnalyzerError> *errors) {
        // Check for redeclarations if any
        if (!fun_set->is_redecl_check()) {
            // fun_set can never be empty (according to scope tree builder)
            fun_set->get_members().begin()->second.second->get_node()->accept(this);
        }

        ErrorGroup<AnalyzerError> err_grp;
        err_grp.error(error("call candidate cannot be resolved", &node));
        std::vector<std::shared_ptr<scope::Function>> candidates;

        for (const auto &[member_name, member]: fun_set->get_members()) {
            const auto &[decl_site, member_scope] = member;
            auto fun_scope = cast<scope::Function>(member_scope);

            bool next_fun = false;    // flag to skip to next function
            size_t arg_id = 0;        // index of arg_info in arg_infos

            // Positional only parameters
            for (const auto &param: fun_scope->get_pos_only_params()) {
                if (next_fun)
                    break;
                auto arg_info = arg_infos[arg_id];
                if (!param.b_variadic && arg_info.b_kwd) {
                    err_grp.note(
                            error(std::format("expected positional argument '{}' but got keyword argument '{}', declared here",
                                              param.name, arg_info.name),
                                  decl_site));
                    next_fun = true;
                    continue;
                }
                if (param.b_variadic) {
                    for (; arg_id < arg_infos.size(); arg_id++) {
                        arg_info = arg_infos[arg_id];
                        // check assignability
                        resolve_assign(&param.type_info, &arg_info.expr_info, node);
                        if (arg_info.b_kwd) {
                            arg_id--;
                            break;
                        }
                    }
                } else {
                    // check assignability
                    resolve_assign(&param.type_info, &arg_info.expr_info, node);
                }
                arg_id++;
            }
            // Positional or keyword parameters and keyword only parameters
            std::map<string, ParamInfo> params;
            // Build the param table
            for (const auto &param: fun_scope->get_pos_kwd_params()) params[param.name] = param;
            for (const auto &param: fun_scope->get_kwd_only_params()) params[param.name] = param;
            const ParamInfo *var_kwargs = null;
            if (fun_scope->is_variadic_kwd_only()) {
                var_kwargs = &fun_scope->get_kwd_only_params().back();
            }

            // Collect arguments
            while (arg_id < arg_infos.size() && !params.empty()) {
                if (next_fun)
                    break;
                auto arg_info = arg_infos[arg_id];
                if (arg_info.b_kwd) {
                    if (params.contains(arg_info.name)) {
                        auto param = params[arg_info.name];
                        if (param.b_variadic) {
                            if (!param.b_kwd_only) {
                                // if argument is kwd and the param is not kwd only but variadic
                                err_grp.note(error(std::format("variadic parameter '{}' cannot be used as keyword "
                                                               "argument, declared here",
                                                               arg_info.name),
                                                   decl_site));
                                next_fun = true;
                                continue;
                            } else {
                                // if argument is kwd and the param kwd only and variadic
                                // var_kwargs should not be null
                                if (!var_kwargs)
                                    throw Unreachable();
                                // check for assignabilty
                                resolve_assign(&param.type_info, &arg_info.expr_info, node);
                            }
                        } else {
                            // if argument is kwd and the param is not variadic
                            // check for assignabilty
                            resolve_assign(&param.type_info, &arg_info.expr_info, node);
                        }
                        params.erase(arg_info.name);    // NOTE: variadic kw param name gets deleted here
                    } else {
                        // if argument is kwd but the param is not found
                        // surely it must be variadic kwargs
                        if (!var_kwargs) {
                            err_grp.note(error(std::format("unknown keyword argument '{}', declared here", arg_info.name),
                                               decl_site));
                            next_fun = true;
                            continue;
                        }
                        // check for assignabilty
                        resolve_assign(&var_kwargs->type_info, &arg_info.expr_info, node);
                    }
                } else {
                    auto [param_name, param] = *params.begin();
                    if (param.b_kwd_only) {
                        // if argument is value but the param is kwd only
                        err_grp.note(
                                error(std::format("expected keyword argument '{}' but got non-keyword argument, declared here",
                                                  param_name),
                                      decl_site));
                        next_fun = true;
                        continue;
                    }
                    if (param.b_variadic) {
                        // if argument is value and the param is not kwd only but variadic
                        for (; arg_id < arg_infos.size(); arg_id++) {
                            arg_info = arg_infos[arg_id];
                            // check for assignabilty
                            resolve_assign(&param.type_info, &arg_info.expr_info, node);
                            if (arg_info.b_kwd) {
                                arg_id--;
                                break;
                            }
                        }
                    } else {
                        // if argument is value and the param is not kwd only
                        // check for assignabilty
                        resolve_assign(&param.type_info, &arg_info.expr_info, node);
                    }
                    params.erase(param_name);
                }
                arg_id++;
            }
            if (next_fun) {
                next_fun = false;
                continue;
            }
            if (arg_id >= arg_infos.size() && !params.empty()) {
                for (const auto &[param_name, param]: params) {
                    if (param.b_variadic)
                        continue;
                    if (!param.b_default) {
                        err_grp.note(
                                error(std::format("missing required argument '{}', declared here", param_name), decl_site));
                        next_fun = true;
                        continue;
                    }
                }
            }
            if (next_fun) {
                next_fun = false;
                continue;
            }
            if (arg_id < arg_infos.size() && params.empty()) {
                for (; arg_id < arg_infos.size(); arg_id++) {
                    auto arg_info = arg_infos[arg_id];
                    err_grp.note(error(std::format("unexpected argument"), node.get_args()[arg_id]));
                }
                err_grp.note(error("declared here", decl_site));
                continue;
            }

            candidates.push_back(fun_scope);
        }
        if (errors)
            *errors = err_grp;
        return candidates;
    }

    ExprInfo Analyzer::resolve_call(scope::FunctionSet *fun_set, std::vector<ArgInfo> arg_infos, const ast::expr::Call &node) {
        ErrorGroup<AnalyzerError> err_grp;
        auto candidates = resolve_call_candidates(fun_set, arg_infos, node, &err_grp);
        std::shared_ptr<scope::Function> candidate;
        if (candidates.size() == 0)
            throw err_grp;
        else if (candidates.size() == 1) {
            candidate = candidates[0];
        } else if (candidates.size() > 1) {
            // TODO: Check for most viable call candidate
            // auto COMP = [&](const std::shared_ptr<scope::Function> fun1, const std::shared_ptr<scope::Function> fun2) {
            //     return fun1->get_param_count() < fun2->get_param_count();
            // };

            // std::sort(candidates.begin(), candidates.end(), COMP);

            ErrorGroup<AnalyzerError> err_grp;
            err_grp.error(error(std::format("ambiguous call to '{}'", fun_set->to_string()), &node));
            for (const auto &candidate: candidates) {
                err_grp.note(error(std::format("possible candidate declared here: '{}'", candidate->to_string()),
                                   candidate->get_node()));
            }
            throw err_grp;
        }

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
                expr_info.type_info.type = cast<scope::Compound>(&*internals[Analyzer::Internal::SPADE_ANY]);
                expr_info.type_info.b_nullable = true;
                warning(std::format("type inference is ambiguous, defaulting to '{}'", expr_info.type_info.to_string()), &node);
                note("declared here", var_scope);
                break;
            case scope::Variable::Eval::DONE:
                expr_info.type_info = var_scope->get_type_info();
                break;
        }
        if (var_scope->get_variable_node()->get_token()->get_type() == TokenType::CONST)
            expr_info.b_const = true;
        return expr_info;
    }

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
    // Then, each of the items in the above list is evaluated accordingly
    // while also saving an `error_state` for each item in `error_states`.
    // If any error occurs then the messages are sent out accordingly
    void Analyzer::check_funs(std::shared_ptr<scope::Function> fun1, std::shared_ptr<scope::Function> fun2,
                              ErrorGroup<AnalyzerError> &errors) {
        enum class ErrorState { NONE, SAME_PARAMS, SAME_DEFAULT_PARAM, AMBIGUOUS };
        std::vector<ErrorState> error_states;
        ErrorState error_state = ErrorState::NONE;

        if (fun1->get_function_node()->get_name()->get_text() != fun2->get_function_node()->get_name()->get_text())
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

        std::vector<ParamInfo> fun1_kwd_only;
        std::vector<ParamInfo> fun1_kwd_only_default;
        std::optional<ParamInfo> fun1_kwd_only_variadic;
        for (const auto &param: fun1->get_kwd_only_params()) {
            if (param.b_variadic)
                fun1_kwd_only_variadic = param;
            else if (param.b_default)
                fun1_kwd_only_default.push_back(param);
            else
                fun1_kwd_only_default.push_back(param);
        }

        std::vector<ParamInfo> fun2_kwd_only;
        std::vector<ParamInfo> fun2_kwd_only_default;
        std::optional<ParamInfo> fun2_kwd_only_variadic;
        for (const auto &param: fun2->get_kwd_only_params()) {
            if (param.b_variadic)
                fun2_kwd_only_variadic = param;
            else if (param.b_default)
                fun2_kwd_only_default.push_back(param);
            else
                fun2_kwd_only_default.push_back(param);
        }

        const bool REQUIRED_PARAMS_AVAILABLE =
                !fun1_pos_only.empty() && !fun2_pos_only.empty() && !fun1_pos_kwd.empty() && !fun2_pos_kwd.empty();
        const bool REQUIRED_PARAMS_NOT_AVAILABLE =
                fun1_pos_only.empty() && fun2_pos_only.empty() && fun1_pos_kwd.empty() && fun2_pos_kwd.empty();
        const bool REQUIRED_KWD_PARAMS_AVAILABLE = !fun1_kwd_only.empty() && !fun2_kwd_only.empty();
        const bool REQUIRED_KWD_PARAMS_NOT_AVAILABLE = fun1_kwd_only.empty() && fun2_kwd_only.empty();

        // check for positional only parameters
        // these parameters are required during a function call
        error_state = ErrorState::NONE;
        if (fun1_pos_only.empty() && fun2_pos_only.empty())
            ;
        else if (fun1_pos_only.size() == fun2_pos_only.size()) {
            auto [_1, _2] = ranges::mismatch(fun1_pos_only, fun2_pos_only, [&](const auto &fun1_param, const auto &fun2_param) {
                return fun1_param.type_info == fun2_param.type_info;
            });
            if (_1 == ranges::end(fun1_pos_only) && _2 == ranges::end(fun2_pos_only)) {
                error_state = ErrorState::SAME_PARAMS;
            }
        } else
            return;
        error_states.push_back(error_state);

        // check for positional or keyword parameters which are not default
        // these parameters are required during a function call
        error_state = ErrorState::NONE;
        if (fun1_pos_kwd.empty() && fun2_pos_kwd.empty())
            ;
        else if (fun1_pos_kwd.size() == fun2_pos_kwd.size()) {
            auto [_1, _2] =
                    ranges::mismatch(fun1_pos_kwd, fun2_pos_kwd, [&](const ParamInfo &fun1_param, const ParamInfo &fun2_param) {
                        return fun1_param.type_info == fun2_param.type_info;
                    });
            if (_1 == ranges::end(fun1_pos_kwd) && _2 == ranges::end(fun2_pos_kwd)) {
                error_state = ErrorState::SAME_PARAMS;
            }
        } else
            return;
        error_states.push_back(error_state);

        {    // check for positional or keyword parameters which are default
            error_state = ErrorState::NONE;
            if (fun1_pos_kwd_default.empty() && fun2_pos_kwd_default.empty())
                ;
            else {
                if (error_states[0] == ErrorState::SAME_PARAMS && error_states[1] == ErrorState::SAME_PARAMS) {
                    auto [_1, _2] = ranges::mismatch(fun1_pos_kwd_default, fun2_pos_kwd_default,
                                                     [&](const ParamInfo &fun1_param, const ParamInfo &fun2_param) {
                                                         return fun1_param.type_info != fun2_param.type_info;
                                                     });
                    error_state = _1 == ranges::end(fun1_pos_kwd_default) || _2 == ranges::end(fun2_pos_kwd_default)
                                          ? ErrorState::NONE
                                          : ErrorState::SAME_DEFAULT_PARAM;
                }
                if (REQUIRED_PARAMS_NOT_AVAILABLE)
                    error_state = ErrorState::AMBIGUOUS;
            }
            error_states.push_back(error_state);
        }

        // check for variadic positional or keyword parameters
        error_state = ErrorState::NONE;
        if (fun1_pos_kwd_variadic || fun2_pos_kwd_variadic) {
            if (error_states[0] != ErrorState::NONE && error_states[1] != ErrorState::NONE) {
                error_state = ErrorState::AMBIGUOUS;
            }
        }
        error_states.push_back(error_state);

        // check for keyword only parameters which are not default
        // these parameters are required during a function call
        error_state = ErrorState::NONE;
        if (fun1_kwd_only.empty() && fun2_kwd_only.empty())
            ;
        else if (fun1_kwd_only.size() == fun2_kwd_only.size()) {
            auto [_1, _2] = ranges::mismatch(
                    fun1_kwd_only, fun2_kwd_only, [&](const ParamInfo &fun1_param, const ParamInfo &fun2_param) {
                        return fun1_param.name == fun2_param.name && fun1_param.type_info == fun2_param.type_info;
                    });
            if (_1 == ranges::end(fun1_kwd_only) && _2 == ranges::end(fun2_kwd_only)) {
                error_state = ErrorState::SAME_PARAMS;
            }
        }
        error_states.push_back(error_state);

        {    // check for keyword only parameters which are default
            error_state = ErrorState::NONE;
            if (fun1_kwd_only_default.empty() && fun2_kwd_only_default.empty())
                ;
            else {
                if (error_states[4] == ErrorState::SAME_PARAMS) {
                    auto [_1, _2] = ranges::mismatch(fun1_kwd_only_default, fun2_kwd_only_default,
                                                     [&](const ParamInfo &fun1_param, const ParamInfo &fun2_param) {
                                                         return !(fun1_param.name == fun2_param.name &&
                                                                  fun1_param.type_info == fun2_param.type_info);
                                                     });
                    error_state = _1 == ranges::end(fun1_kwd_only_default) || _2 == ranges::end(fun2_kwd_only_default)
                                          ? ErrorState::NONE
                                          : ErrorState::SAME_DEFAULT_PARAM;
                }
                if (REQUIRED_KWD_PARAMS_NOT_AVAILABLE)
                    error_state = ErrorState::AMBIGUOUS;
            }
            error_states.push_back(error_state);
        }

        // check for keyword only parameters
        error_state = ErrorState::NONE;
        if (fun1_kwd_only_variadic || fun2_kwd_only_variadic) {
            if (REQUIRED_KWD_PARAMS_NOT_AVAILABLE || error_states[4] == ErrorState::SAME_PARAMS) {
                error_state = ErrorState::AMBIGUOUS;
            }
        }
        error_states.push_back(error_state);

        for (const auto &error_state: error_states) {
            if (error_state != ErrorState::NONE) {
                errors.error(error(std::format("ambiguous declaration of '{}'", fun1->to_string()), fun1->get_decl_site()))
                        .note(error(std::format("check another declaration here: '{}'", fun2->to_string()),
                                    fun2->get_decl_site()));
                return;
            }
        }
    }

    void Analyzer::check_fun_set(std::shared_ptr<scope::FunctionSet> fun_set) {
        auto old_cur_scope = get_current_scope();
        cur_scope = &*fun_set;

        ErrorGroup<AnalyzerError> err_grp;
        bool error_state = false;

        for (auto it1 = fun_set->get_members().begin(); it1 != fun_set->get_members().end(); ++it1) {
            auto fun1 = cast<scope::Function>(it1->second.second);

            for (auto it2 = std::next(it1); it2 != fun_set->get_members().end(); ++it2) {
                auto fun2 = cast<scope::Function>(it2->second.second);

                check_funs(fun1, fun2, err_grp);
                if (!err_grp.get_errors().empty())
                    error_state = true;
            }
        }

        // Set qualified names
        std::unordered_map<string, scope::Scope::Member> new_members;
        for (auto &[_1, member]: fun_set->get_members()) {
            const auto &[_2, scope] = member;
            string full_name = scope->to_string(false);
            string name = full_name.substr(0, full_name.find_first_of('('));
            string final_name = SymbolPath(name).get_name() + full_name.substr(full_name.find_first_of('('));
            cast<scope::Function>(scope)->get_function_node()->set_qualified_name(final_name);
            new_members[final_name] = member;
        }

        fun_set->set_members(new_members);

        if (error_state)
            throw err_grp;
        cur_scope = old_cur_scope;
    }

    void Analyzer::analyze(const std::vector<std::shared_ptr<ast::Module>> &modules) {
        if (modules.empty())
            return;
        // Build scope tree
        ScopeTreeBuilder builder(modules);
        module_scopes = builder.build();

        load_internal_modules();

        // Print scope tree
        // for (auto [_, module_scope_info]: module_scopes) {
        //     if (module_scope_info.is_original()) {
        //         module_scope_info.get_scope()->print();
        //     }
        // }
        // Start analysis
        for (auto [_, module_scope_info]: module_scopes) {
            if (module_scope_info.is_original())
                if (auto node = module_scope_info.get_scope()->get_node())
                    node->accept(this);
        }
    }

    void Analyzer::visit(ast::Reference &node) {
        _res_reference = null;
        // Find the scope where name is located
        auto scope = find_name(node.get_path()[0]->get_text());
        // Yell if the scope cannot be located
        if (!scope)
            throw error("undefined reference", &node);
        // Now check for references inside the scope
        for (size_t i = 1; i < node.get_path().size(); i++) {
            auto path_element = node.get_path()[i]->get_text();
            if (!scope->has_variable(path_element))
                throw error("undefined reference", &node);
            scope = scope->get_variable(path_element);
        }
        _res_reference = scope;
    }

    void Analyzer::visit(ast::type::Reference &node) {
        // Find the type scope
        node.get_reference()->accept(this);
        auto type_scope = _res_reference;
        // Check if the reference is a type
        if (type_scope->get_type() != scope::ScopeType::COMPOUND)
            throw ErrorGroup<AnalyzerError>()
                    .error(error("reference is not a type", &node))
                    .note(error("declared here", type_scope));
        // Check for type arguments
        std::vector<TypeInfo> type_args;
        for (auto type_arg: node.get_type_args()) {
            type_arg->accept(this);
            type_args.push_back(_res_type_info);
        }
        _res_type_info.reset();
        _res_type_info.type = cast<scope::Compound>(&*type_scope);
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