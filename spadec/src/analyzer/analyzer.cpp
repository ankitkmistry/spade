#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "scope_tree.hpp"
#include "spimp/error.hpp"
#include "symbol_path.hpp"
#include "utils/error.hpp"
#include <memory>
#include <utility>

// TODO: implement generics

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
        if (candidates.size() == 0)
            throw err_grp;
        else if (candidates.size() > 1) {
            ErrorGroup<AnalyzerError> err_grp;
            err_grp.error(error(std::format("ambiguous call to '{}'", fun_set->to_string()), &node));
            for (const auto &candidate: candidates) {
                err_grp.note(error("possible candidate declared here", candidate->get_node()));
            }
            throw err_grp;
        }

        auto candidate = candidates[0];
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
        return expr_info;
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

    void Analyzer::visit(ast::stmt::Block &node) {}

    void Analyzer::visit(ast::stmt::If &node) {}

    void Analyzer::visit(ast::stmt::While &node) {}

    void Analyzer::visit(ast::stmt::DoWhile &node) {}

    void Analyzer::visit(ast::stmt::Throw &node) {}

    void Analyzer::visit(ast::stmt::Catch &node) {}

    void Analyzer::visit(ast::stmt::Try &node) {}

    void Analyzer::visit(ast::stmt::Continue &node) {}

    void Analyzer::visit(ast::stmt::Break &node) {}

    void Analyzer::visit(ast::stmt::Return &node) {}

    void Analyzer::visit(ast::stmt::Yield &node) {}

    void Analyzer::visit(ast::stmt::Expr &node) {}

    void Analyzer::visit(ast::stmt::Declaration &node) {}

    void Analyzer::visit(ast::decl::TypeParam &node) {}

    void Analyzer::visit(ast::decl::Constraint &node) {}

    void Analyzer::visit(ast::decl::Param &node) {
        auto fun = cast<scope::Function>(get_current_scope());

        ParamInfo param_info;
        param_info.b_const = node.get_is_const() != null;
        param_info.b_variadic = node.get_variadic() != null;
        param_info.b_default = node.get_default_expr() != null;
        param_info.name = node.get_name()->get_text();
        param_info.type_info = resolve_assign(node.get_type(), node.get_default_expr(), node);
        param_info.node = &node;

        _res_param_info.reset();
        _res_param_info = param_info;
    }

    void Analyzer::visit(ast::decl::Params &node) {
        auto fun = cast<scope::Function>(get_current_scope());
        std::shared_ptr<ast::decl::Param> found_variadic;
        std::shared_ptr<ast::decl::Param> found_default;

        std::vector<ParamInfo> pos_only_params;
        pos_only_params.reserve(node.get_pos_only().size());
        for (const auto &param: node.get_pos_only()) {
            param->accept(this);
            if (_res_param_info.b_variadic) {
                if (found_variadic)
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("variadic parameters is allowed only once", param))
                            .note(error("already declared here", found_variadic));
                found_variadic = param;
            }
            if (_res_param_info.b_default) {
                throw error("positional only parameter cannot have default value", param);
            }
            pos_only_params.push_back(_res_param_info);
        }
        fun->set_pos_only_params(pos_only_params);

        std::vector<ParamInfo> pos_kwd_params;
        pos_kwd_params.reserve(node.get_pos_kwd().size());
        for (const auto &param: node.get_pos_kwd()) {
            param->accept(this);
            if (_res_param_info.b_variadic) {
                if (found_variadic)
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("variadic parameters is allowed only once", param))
                            .note(error("already declared here", found_variadic));
                found_variadic = param;
            }
            if (!_res_param_info.b_default) {
                if (found_default)
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("incorrect ordering of default parameters", param))
                            .note(error("already declared here", found_default));
            } else {
                found_default = param;
            }
            pos_kwd_params.push_back(_res_param_info);
        }
        fun->set_pos_kwd_params(pos_kwd_params);

        // check for variadic parameters ambiguity
        if (found_variadic && ((!node.get_pos_only().empty() && node.get_pos_only().back() != found_variadic) ||
                               (!node.get_pos_kwd().empty() && node.get_pos_kwd().back() != found_variadic)))
            throw ErrorGroup<AnalyzerError>().error(error("variadic parameter must be the last parameter", found_variadic));
        found_variadic = null;    // variadic parameters is separate for kwd paremeter

        std::vector<ParamInfo> kwd_only_params;
        kwd_only_params.reserve(node.get_kwd_only().size());
        for (const auto &param: node.get_kwd_only()) {
            param->accept(this);
            if (_res_param_info.b_variadic) {
                if (found_variadic)
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("variadic parameters is allowed only once", param))
                            .note(error("already declared here", found_variadic));
                found_variadic = param;
            }
            _res_param_info.b_kwd_only = true;
            kwd_only_params.push_back(_res_param_info);
        }
        fun->set_kwd_only_params(kwd_only_params);

        if (found_variadic && node.get_kwd_only().back() != found_variadic)
            throw ErrorGroup<AnalyzerError>().error(error("variadic parameter must be the last parameter", found_variadic));
    }

    void Analyzer::visit(ast::decl::Function &node) {
        std::shared_ptr<scope::FunctionSet> fun_set = find_scope<scope::FunctionSet>(node.get_name()->get_text());
        std::shared_ptr<scope::Function> scope = find_scope<scope::Function>(node.get_qualified_name());

        // TODO: check for function level declarations

        if (scope->get_proto_eval() == scope::Function::ProtoEval::NOT_STARTED) {
            scope->set_proto_eval(scope::Function::ProtoEval::PROGRESS);

            if (auto type = node.get_return_type()) {
                type->accept(this);
                scope->set_ret_type(_res_type_info);
            } else {
                TypeInfo return_type;
                return_type.type = scope->is_init() ? scope->get_enclosing_compound()
                                                    : cast<scope::Compound>(&*internals[Internal::SPADE_VOID]);
                scope->set_ret_type(return_type);
            }

            if (auto params = node.get_params())
                params->accept(this);

            scope->set_proto_eval(scope::Function::ProtoEval::DONE);
        }

        if (!fun_set->is_redecl_check()) {
            fun_set->set_redecl_check(true);
            auto old_cur_scope = get_current_scope();    // save the context
            end_scope();                                 // pop the function
            end_scope();                                 // pop the function set
            // Collect all other defintions
            for (const auto &[member_name, member]: fun_set->get_members()) {
                const auto &[_, member_scope] = member;
                if (scope != member_scope)
                    member_scope->get_node()->accept(this);
            }
            // for (const auto &[member_name, member]: fun_set->get_members()) {
            //     const auto &[_, member_scope] = member;
            //     auto fun_scope = cast<scope::Function>(member_scope);
            //     std::vector<ArgInfo> arg_infos;
            //     for (const auto &param: fun_scope->get_pos_only_params()) {
            //         ArgInfo arg_info;
            //         arg_info.b_kwd = false;
            //         arg_info.name = "";
            //         arg_info.expr_info.tag = ExprInfo::Type::NORMAL;
            //         arg_info.expr_info.type_info = param.type_info;
            //         arg_infos.push_back(arg_info);
            //     }
            //     for (const auto &param: fun_scope->get_pos_kwd_params()) {
            //         ArgInfo arg_info;
            //         arg_info.b_kwd = true;
            //         arg_info.name = param.name;
            //         arg_info.expr_info.tag = ExprInfo::Type::NORMAL;
            //         arg_info.expr_info.type_info = param.type_info;
            //         arg_infos.push_back(arg_info);
            //     }
            //     for (const auto &param: fun_scope->get_kwd_only_params()) {
            //         ArgInfo arg_info;
            //         arg_info.b_kwd = true;
            //         arg_info.name = param.name;
            //         arg_info.expr_info.tag = ExprInfo::Type::NORMAL;
            //         arg_info.expr_info.type_info = param.type_info;
            //         arg_infos.push_back(arg_info);
            //     }
            //     auto candidates=resolve_call_candidates(&*fun_set, arg_infos, node);
            // }
            // TODO: Check all the functions using brute force
            cur_scope = old_cur_scope;    // restore context
        }

        auto definition = node.get_definition();

        if (scope->get_enclosing_function() != null) {
            if (definition == null)
                throw error("function must have a definition", &node);
        } else if (auto compound = scope->get_enclosing_compound()) {
            switch (compound->get_compound_node()->get_token()->get_type()) {
                case TokenType::CLASS: {
                    if (scope->is_init() && definition == null) {
                        throw error("constructor must have a definition", &node);
                    }
                    if (scope->is_abstract()) {
                        if (definition != null)
                            throw error("abstract function cannot have a definition", &node);
                    } else {
                        if (definition == null)
                            throw error("function must have a definition", &node);
                    }
                    break;
                }
                case TokenType::INTERFACE: {
                    if (scope->is_static()) {
                        if (definition == null)
                            throw error("static function must have a definition", &node);
                    }
                    break;
                }
                case TokenType::ENUM: {
                    if (scope->is_init() && definition == null) {
                        throw error("constructor must have a definition", &node);
                    }
                    if (definition == null)
                        throw error("function must have a definition", &node);
                    break;
                }
                case TokenType::ANNOTATION: {
                    if (scope->is_init() && definition == null) {
                        throw error("constructor must have a definition", &node);
                    }
                    if (definition == null)
                        throw error("function must have a definition", &node);
                    break;
                }
                default:
                    throw Unreachable();    // surely some parser error
            }
        } else {
            if (definition == null)
                throw error("function must have a definition", &node);
        }

        if (definition)
            definition->accept(this);

        end_scope();    // pop the function
        end_scope();    // pop the function set
    }

    void Analyzer::visit(ast::decl::Variable &node) {
        std::shared_ptr<scope::Variable> scope;
        if (get_current_scope()->get_type() == scope::ScopeType::FUNCTION) {
            scope = begin_scope<scope::Variable>(node);
            // Add the variable to the parent scope
            auto parent_scope = get_parent_scope();
            if (!parent_scope->new_variable(node.get_name(), scope)) {
                auto org_def = scope->get_decl_site(node.get_name()->get_text());
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("redeclaration of '{}'", node.get_name()->get_text()), node.get_name()))
                        .note(error("already declared here", org_def));
            };
            // Check if the variable is not overshadowing parameters
            if (auto fun = scope->get_enclosing_function()) {
                if (fun->has_param(node.get_name()->get_text())) {
                    auto param = fun->get_param(node.get_name()->get_text());
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(
                                    std::format("function parameters cannot be overshadowed '{}'", node.get_name()->get_text()),
                                    node.get_name()))
                            .note(error("already declared here", param.node));
                }
            }
        } else {
            scope = find_scope<scope::Variable>(node.get_name()->get_text());
        }

        if (scope->get_eval() == scope::Variable::Eval::NOT_STARTED) {
            scope->set_eval(scope::Variable::Eval::PROGRESS);
            resolve_assign(node.get_type(), node.get_expr(), node);
        }
        end_scope();
    }

    void Analyzer::visit(ast::decl::Parent &node) {
        _res_type_info.reset();
        node.get_reference()->accept(this);
        // Check if the super class is a scope::Compound
        if (_res_reference->get_type() != scope::ScopeType::COMPOUND)
            throw ErrorGroup<AnalyzerError>()
                    .error(error("reference is not a type", &node))
                    .note(error("declared here", _res_reference));
        // Get the parent type info and type args if any
        TypeInfo parent_type_info;
        parent_type_info.type = cast<scope::Compound>(&*_res_reference);
        if (!node.get_type_args().empty()) {
            for (auto type_arg: node.get_type_args()) {
                type_arg->accept(this);
                parent_type_info.type_args.push_back(_res_type_info);
            }
        }
        _res_type_info = parent_type_info;
    }

    void Analyzer::visit(ast::decl::Enumerator &node) {}

    void Analyzer::visit(ast::decl::Compound &node) {
        auto scope = find_scope<scope::Compound>(node.get_name()->get_text());
        if (node.get_parents().empty()) {
            scope->inherit_from(cast<scope::Compound>(internals[Internal::SPADE_ANY]));
        } else {
            for (auto parent: node.get_parents()) {
                parent->accept(this);
                scope->inherit_from(_res_type_info.type);
            }
        }
        for (auto member: node.get_members()) member->accept(this);
        end_scope();
    }

    void Analyzer::visit(ast::Import &node) {
        auto scope = get_current_scope();
        // Put the alias if present
        auto name = node.get_alias() ? node.get_alias() : node.get_name();
        if (auto module_sptr = node.get_module().lock()) {
            auto value = module_scopes.at(&*module_sptr).get_scope();
            if (!scope->new_variable(name, value)) {
                // Find the original declaration
                auto org_def = scope->get_decl_site(name->get_text());
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("redeclaration of '{}'", name->get_text()), name))
                        .note(error("already declared here", org_def));
            }
        } else {
            LOGGER.log_error("import statement is not resolved");
        }
    }

    void Analyzer::visit(ast::Module &node) {
        if (get_current_scope() == null) {
            auto scope = module_scopes.at(&node).get_scope();
            cur_scope = &*scope;
        } else {
            find_scope<scope::Module>(node.get_name());
        }

        for (auto import: node.get_imports()) import->accept(this);
        for (auto member: node.get_members()) member->accept(this);

        end_scope();
    }

    void Analyzer::visit(ast::FolderModule &node) {
        std::shared_ptr<scope::Scope> scope;
        if (get_current_scope() == null) {
            scope = module_scopes.at(&node).get_scope();
            cur_scope = &*scope;
        } else {
            scope = find_scope<scope::Module>(node.get_name());
        }

        for (auto [name, member]: scope->get_members()) {
            auto [_, scope] = member;
            scope->get_node()->accept(this);
        }

        end_scope();
    }
}    // namespace spade