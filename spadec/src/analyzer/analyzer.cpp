#include <set>
#include <unordered_set>

#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "parser/parser.hpp"
#include "parser/printer.hpp"
#include "scope.hpp"
#include "scope_tree.hpp"
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

    scope::Module *Analyzer::get_current_module() const {
        return get_current_scope()->get_type() == scope::ScopeType::MODULE ? cast<scope::Module>(get_current_scope())
                                                                           : get_current_scope()->get_enclosing_module();
    }

    scope::Compound *Analyzer::get_current_compound() const {
        return get_current_scope()->get_type() == scope::ScopeType::COMPOUND ? cast<scope::Compound>(get_current_scope())
                                                                             : get_current_scope()->get_enclosing_compound();
    }

    scope::Function *Analyzer::get_current_function() const {
        return get_current_scope()->get_type() == scope::ScopeType::FUNCTION ? cast<scope::Function>(get_current_scope())
                                                                             : get_current_scope()->get_enclosing_function();
    }

    void Analyzer::load_internal_modules() {
        basic_mode = true;
        std::shared_ptr<scope::Module> basic_module = resolve_file(compiler_options.basic_module_path);

        mode = Mode::DECLARATION;
        cur_scope = &*basic_module;
        basic_module->get_module_node()->accept(this);

        mode = Mode::DEFINITION;
        for (auto function: function_scopes) {
            cur_scope = function->get_parent()->get_parent();
            function->get_node()->accept(this);
        }
        function_scopes.clear();    // Prevent further revisit

        internals[Internal::SPADE] = basic_module;

        internals[Internal::SPADE_ANY] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_ANY)]);
        internals[Internal::SPADE_ENUM] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_ENUM)]);
        internals[Internal::SPADE_ANNOTATION] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_ANNOTATION)]);
        internals[Internal::SPADE_THROWABLE] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_THROWABLE)]);

        internals[Internal::SPADE_INT] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_INT)]);
        internals[Internal::SPADE_FLOAT] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_FLOAT)]);
        internals[Internal::SPADE_BOOL] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_BOOL)]);
        internals[Internal::SPADE_STRING] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_STRING)]);
        internals[Internal::SPADE_VOID] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_VOID)]);

        internals[Internal::SPADE_SLICE] = basic_module->get_variable(INTERNAL_NAMES[static_cast<int>(Internal::SPADE_SLICE)]);

        basic_mode = false;
    }

    // Optimized Levenshtein distance function (O(min(m, n)) space)
    static int levenshtein(const std::string &s1, const std::string &s2) {
        const size_t m = s1.size(), n = s2.size();
        if (m == 0)
            return n;
        if (n == 0)
            return m;
        // Always use the smaller string for the row
        if (m < n)
            return levenshtein(s2, s1);

        std::vector<int> prev(n + 1), curr(n + 1);
        for (size_t j = 0; j <= n; ++j) prev[j] = j;
        for (size_t i = 1; i <= m; ++i) {
            curr[0] = i;
            for (size_t j = 1; j <= n; ++j) {
                if (s1[i - 1] == s2[j - 1])
                    curr[j] = prev[j - 1];
                else
                    curr[j] = 1 + std::min({prev[j], curr[j - 1], prev[j - 1]});
            }
            std::swap(prev, curr);
        }
        return prev[n];
    }

    static std::set<std::string> fuzzy_search_best(const std::string &query, const std::unordered_set<std::string> &candidates) {
        constexpr const size_t MAX_RESULTS = 6;

        int min_dist = std::numeric_limits<int>::max();
        std::set<std::string> results;

        for (const auto &candidate: candidates) {
            if (candidate.starts_with("%"))
                continue;

            int dist = levenshtein(query, candidate);
            if (dist < min_dist) {
                min_dist = dist;
                results.clear();
                results.insert(candidate);
            } else if (dist == min_dist)
                results.insert(candidate);

            if (results.size() >= MAX_RESULTS && dist == min_dist)
                break;
        }
        return results;
    }

    ExprInfo Analyzer::resolve_name(const string &name, const ast::AstNode &node) {
        const auto cur_module = get_current_module();
        const auto cur_compound = get_current_scope()->get_enclosing_compound();

        std::unordered_set<string> names;

        ExprInfo expr_info;
        scope::Scope *result = null;
        scope::Scope *scope;
        for (scope = get_current_scope(); scope && scope != cur_module; scope = scope->get_parent()) {
            if (scope->get_type() == scope::ScopeType::COMPOUND) {
                // Eval the compound if not already evaled
                auto compound = cast<scope::Compound>(scope);
                if (compound->get_eval() == scope::Compound::Eval::NOT_STARTED) {
                    const auto old_cur_scope = get_current_scope();
                    cur_scope = compound->get_parent();
                    compound->get_node()->accept(this);
                    cur_scope = old_cur_scope;
                }
            }
            if (scope->has_variable(name)) {
                result = &*scope->get_variable(name);
                break;
            }
            for (const auto &[name, _]: scope->get_members()) names.insert(name);

            if (scope->get_type() == scope::ScopeType::FUNCTION) {
                // Check for parameters
                const auto function = cast<scope::Function>(scope);
                for (const auto &param: function->get_pos_only_params()) {
                    if (param.name == name) {
                        expr_info.tag = ExprInfo::Kind::NORMAL;
                        expr_info.value_info.b_const = param.b_const;
                        expr_info.value_info.b_lvalue = true;
                        expr_info.type_info() = param.type_info;
                        return expr_info;
                    }
                    names.insert(param.name);
                }
                for (const auto &param: function->get_pos_kwd_params()) {
                    if (param.name == name) {
                        expr_info.tag = ExprInfo::Kind::NORMAL;
                        expr_info.value_info.b_const = param.b_const;
                        expr_info.value_info.b_lvalue = true;
                        expr_info.type_info() = param.type_info;
                        return expr_info;
                    }
                    names.insert(param.name);
                }
                for (const auto &param: function->get_kwd_only_params()) {
                    if (param.name == name) {
                        expr_info.tag = ExprInfo::Kind::NORMAL;
                        expr_info.value_info.b_const = param.b_const;
                        expr_info.value_info.b_lvalue = true;
                        expr_info.type_info() = param.type_info;
                        return expr_info;
                    }
                    names.insert(param.name);
                }
            }
            if (scope->get_type() == scope::ScopeType::LAMBDA) {
                // Check for parameters
                const auto lambda = cast<scope::Lambda>(scope);
                for (const auto &param: lambda->get_fn().pos_only_params()) {
                    if (param.name == name) {
                        expr_info.tag = ExprInfo::Kind::NORMAL;
                        expr_info.value_info.b_const = param.b_const;
                        expr_info.value_info.b_lvalue = true;
                        expr_info.type_info() = param.type_info;
                        return expr_info;
                    }
                    names.insert(param.name);
                }
                for (const auto &param: lambda->get_fn().pos_kwd_params()) {
                    if (param.name == name) {
                        expr_info.tag = ExprInfo::Kind::NORMAL;
                        expr_info.value_info.b_const = param.b_const;
                        expr_info.value_info.b_lvalue = true;
                        expr_info.type_info() = param.type_info;
                        return expr_info;
                    }
                    names.insert(param.name);
                }
                for (const auto &param: lambda->get_fn().kwd_only_params()) {
                    if (param.name == name) {
                        expr_info.tag = ExprInfo::Kind::NORMAL;
                        expr_info.value_info.b_const = param.b_const;
                        expr_info.value_info.b_lvalue = true;
                        expr_info.type_info() = param.type_info;
                        return expr_info;
                    }
                    names.insert(param.name);
                }
            }
            // Check in current compound
            if (cur_compound && scope == cur_compound) {
                if (cur_compound->get_super_fields().contains(name)) {
                    // Check in super class fields
                    result = &*cur_compound->get_super_fields().at(name);
                } else if (cur_compound->get_super_functions().contains(name)) {
                    // Check in super class functions
                    expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
                    expr_info.value_info.b_const = true;
                    expr_info.value_info.b_lvalue = true;
                    expr_info.functions() = cur_compound->get_super_functions().at(name);
                    return expr_info;
                }
                for (const auto &[name, _]: cur_compound->get_super_fields()) names.insert(name);
                for (const auto &[name, _]: cur_compound->get_super_functions()) names.insert(name);
            }
        }

        // Check in current module
        if (!result && scope == cur_module) {
            if (cur_module->has_variable(name))
                // Check module global variables
                result = &*cur_module->get_variable(name);
            else if (cur_module->has_import(name)) {
                // Check module imports
                if (auto opt = cur_module->get_import(name)) {
                    auto &import = **opt;
                    result = import.scope;
                    if (result)
                        import.b_used = true;
                }
            } else {
                // Check module open imports
                for (auto &import: cur_module->get_open_imports()) {
                    if (import.scope->has_variable(name)) {
                        result = &*cur_module->get_variable(name);
                        import.b_used = true;
                        break;
                    }
                    for (const auto &[name, _]: import.scope->get_members()) names.insert(name);
                }
            }
            for (const auto &[name, _]: cur_module->get_members()) names.insert(name);
            for (const auto &[name, _]: cur_module->get_imports()) names.insert(name);
        }

        // Check for spade module
        if (!result && !basic_mode && internals[Analyzer::Internal::SPADE]->has_variable(name)) {
            result = &*internals[Analyzer::Internal::SPADE]->get_variable(name);
        }
        if (!basic_mode)
            for (const auto &[name, _]: internals[Analyzer::Internal::SPADE]->get_members()) names.insert(name);

        // Yell if the scope cannot be located
        if (!result) {
            const auto results = fuzzy_search_best(name, names);
            if (results.empty())
                throw error(std::format("undefined reference: '{}'", name), &node);
            if (results.size() == 1)
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("undefined reference: '{}'", name), &node))
                        .help(error(std::format("did you mean '{}'?", *results.begin())));
            string final_str;
            for (const auto &result: results) {
                final_str += "'" + result + "', ";
            }
            final_str.pop_back();
            final_str.pop_back();
            throw ErrorGroup<AnalyzerError>()
                    .error(error(std::format("undefined reference: '{}'", name), &node))
                    .help(error(std::format("did you mean one of {}?", final_str)));
        }

        // Resolve the context
        resolve_context(result, node);
        switch (result->get_type()) {
        case scope::ScopeType::FOLDER_MODULE:
        case scope::ScopeType::MODULE:
            expr_info.tag = ExprInfo::Kind::MODULE;
            expr_info.value_info.b_const = true;
            expr_info.module() = cast<scope::Module>(&*result);
            break;
        case scope::ScopeType::COMPOUND:
            expr_info.tag = ExprInfo::Kind::STATIC;
            expr_info.value_info.b_const = true;
            expr_info.type_info().basic().type = cast<scope::Compound>(&*result);
            break;
        case scope::ScopeType::FUNCTION:
            throw Unreachable();    // surely some scope tree builder error
        case scope::ScopeType::FUNCTION_SET:
            expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
            expr_info.value_info.b_const = true;
            expr_info.functions() = cast<scope::FunctionSet>(result);
            break;
        case scope::ScopeType::VARIABLE:
            expr_info = get_var_expr_info(cast<scope::Variable>(result), node);
            break;
        case scope::ScopeType::ENUMERATOR:
            expr_info.tag = ExprInfo::Kind::NORMAL;
            expr_info.value_info.b_const = true;
            expr_info.type_info().basic().type = result->get_enclosing_compound();
            break;
        case scope::ScopeType::BLOCK:
        case scope::ScopeType::LAMBDA:
            throw Unreachable();
        }
        expr_info.value_info.b_lvalue = true;
        expr_info.value_info.scope = result;
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

        // Do not check static context if we are accessing a ctor
        {    // static context code
            bool static_context = false;
            if (const auto fun = get_current_function())
                static_context = fun->is_static();
            if (from_scope->get_type() == scope::ScopeType::VARIABLE)
                static_context = cast<const scope::Variable>(from_scope)->is_static();
            if (to_scope->get_type() == scope::ScopeType::FUNCTION && cast<const scope::Function>(to_scope)->is_init())
                static_context = false;

            if (static_context) {
                switch (to_scope->get_type()) {
                case scope::ScopeType::FOLDER_MODULE:    // modules can be referenced from static ctx
                case scope::ScopeType::MODULE:           // modules can be referenced from static ctx
                case scope::ScopeType::COMPOUND:         // compounds can be referenced from static ctx
                    break;
                case scope::ScopeType::FUNCTION:    // only static functions can be referenced from static ctx
                    if (!cast<const scope::Function>(to_scope)->is_static()) {
                        errors.error(error(std::format("cannot access non-static '{}' from static context", to_scope->to_string()), &node))
                                .note(error("declared here", to_scope));
                        return;
                    }
                    break;
                case scope::ScopeType::VARIABLE:    // only static variables can be referenced from static ctx
                    if (!cast<const scope::Variable>(to_scope)->is_static()) {
                        errors.error(error(std::format("cannot access non-static '{}' from static context", to_scope->to_string()), &node))
                                .note(error("declared here", to_scope));
                        return;
                    }
                    break;
                case scope::ScopeType::ENUMERATOR:    // enumerators can be referenced from static ctx
                    break;
                case scope::ScopeType::FUNCTION_SET:    // spare function sets
                case scope::ScopeType::LAMBDA:          // spare lambdas
                case scope::ScopeType::BLOCK:
                    break;
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
        case scope::ScopeType::LAMBDA:
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
        if (errors)
            throw errors;
    }

    void Analyzer::resolve_context(const std::shared_ptr<scope::Scope> scope, const ast::AstNode &node) const {
        ErrorGroup<AnalyzerError> errors;
        resolve_context(get_current_scope(), &*scope, node, errors);
        if (errors)
            throw errors;
    }

    void Analyzer::check_cast(scope::Compound *from, scope::Compound *to, const ast::AstNode &node, bool safe) {
        if (from == null || to == null) {
            LOGGER.log_warn("check_cast: one of the scope::Compound is null, casting cannot be done");
            LOGGER.log_debug(std::format("check_cast: from = {}, to = {}", (from ? "non-null" : "null"), (to ? "non-null" : "null")));
            return;
        }
        if (from == to)
            return;
        // any value is convertible to string by to_string() function
        if (to == get_internal(Internal::SPADE_STRING))
            return;
        // any value is convertible to void, though it is not the direct ancestor of all objects
        if (to == get_internal(Internal::SPADE_VOID))
            return;

        {
            const auto report_err = [&] {
                if (safe)
                    warning("expression is always 'null'", &node);
                else
                    throw error(std::format("cannot cast '{}' to '{}'", from->to_string(), to->to_string()), &node);
            };
            // Hardcoded conversions supported by the compiler
            if (from == get_internal(Internal::SPADE_ANY) && to != get_internal(Internal::SPADE_ANY)) {
                report_err();
                return;
            } else if (from == get_internal(Internal::SPADE_INT)) {
                if (to == get_internal(Internal::SPADE_FLOAT))
                    return;
                else {
                    report_err();
                    return;
                }
            } else if (from == get_internal(Internal::SPADE_FLOAT)) {
                if (to == get_internal(Internal::SPADE_INT))
                    return;
                else {
                    report_err();
                    return;
                }
            }
        }

        // take advantage of super classes
        if (from->has_super(to))
            return;

        // Setup error state
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
                            err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()), to_member_scope))
                                    .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(), from->to_string()),
                                                from_member_scope));
                        }
                    } else if (from_member_scope->get_type() == scope::ScopeType::VARIABLE) {
                        // check if they are the same type of variable (var, const)
                        if (cast<ast::decl::Variable>(from_member_scope->get_node())->get_token()->get_type() !=
                            cast<ast::decl::Variable>(to_member_scope->get_node())->get_token()->get_type()) {
                            if (!error_state)
                                error_state = true;
                            err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()), to_member_scope))
                                    .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(), from->to_string()),
                                                from_member_scope));
                        }
                    }
                } else {
                    if (!error_state)
                        error_state = true;
                    err_grp.note(error(std::format("see '{}' in '{}'", to_member_scope->to_string(), to->to_string()), to_member_scope))
                            .note(error(std::format("also see '{}' in '{}'", from_member_scope->to_string(), from->to_string()), from_member_scope));
                }
            } else {
                if (!error_state)
                    error_state = true;
                err_grp.note(error(std::format("'{}' does not have similar member like '{}'", from->to_string(), to_member_scope->to_string()),
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
        // Check type inference
        switch (expr_info.tag) {
        case ExprInfo::Kind::NORMAL:
            if (type_info.tag != expr_info.type_info().tag)
                throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);
            if (!type_info.nullable() && expr_info.type_info().nullable())
                expr_info.is_null() ? throw error(std::format("cannot assign 'null' to type '{}'", type_info.to_string()), &node)
                                    : throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(),
                                                              type_info.to_string()),
                                                  &node);

            switch (expr_info.type_info().tag) {
            case TypeInfo::Kind::BASIC: {
                if (!expr_info.is_null() && type_info.basic().type != expr_info.type_info().basic().type &&
                    !expr_info.type_info().basic().type->has_super(type_info.basic().type))
                    throw error(
                            std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);

                TypeInfo result(type_info);

                if (type_info.basic().type_args.empty() && expr_info.type_info().basic().type_args.empty()) {
                    // no type args, plain vanilla
                } else if (/* type_info.basic().type_args.empty() && */ !expr_info.type_info().basic().type_args.empty()) {
                    // deduce from type_info
                    result.basic().type_args = expr_info.type_info().basic().type_args;
                } else if (!type_info.basic().type_args.empty() /* && expr_info.type_info().basic().type_args.empty() */) {
                    // deduce from expr_info
                    // TODO: check type args
                } else /* if(!type_info.basic().type_args.empty() && !expr_info.type_info().basic().type_args.empty()) */ {
                    if (type_info.basic().type_args.size() != expr_info.type_info().basic().type_args.size())
                        throw error(std::format("failed to deduce type arguments"), &node);    // failed deducing
                    // now both type_info and expr_info have typeargs (same size)
                    // check if both of them are same
                    // TODO: implement covariance and contravariance
                }

                return result;
            }
            case TypeInfo::Kind::FUNCTION:
                if (type_info != expr_info.type_info())
                    throw error(
                            std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);
                return type_info;
            }
        case ExprInfo::Kind::STATIC:
            if (type_info.tag != TypeInfo::Kind::BASIC)
                throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);
            if (!type_info.basic().is_type_literal())
                throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);
            if (!type_info.nullable() && expr_info.type_info().nullable())
                throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);
            return type_info;
        case ExprInfo::Kind::MODULE:
            throw error(std::format("cannot assign a module to type '{}'", type_info.to_string()), &node);
        case ExprInfo::Kind::FUNCTION_SET:
            if (type_info.tag != TypeInfo::Kind::FUNCTION)
                throw error(std::format("cannot assign value of type '{}' to type '{}'", expr_info.type_info().to_string(), type_info.to_string()),
                            &node);

            std::vector<scope::Function *> matched_functions;
            for (const auto &[_, function]: expr_info.functions().get_functions())
                if (type_info.function() == function)
                    matched_functions.push_back(function);

            if (matched_functions.empty())
                throw error(std::format("cannot assign value of type '{}' to type '{}' because of no possible candidate",
                                        expr_info.type_info().to_string(), type_info.to_string()),
                            &node);
            if (matched_functions.size() > 1) {
                ErrorGroup<AnalyzerError> errors;
                errors.error(error(std::format("cannot assign value of type '{}' to type '{}' because of multiple candidates",
                                               expr_info.type_info().to_string(), type_info.to_string()),
                                   &node));
                for (const auto fn: matched_functions) {
                    errors.note(error("possible candidate declared here", fn));
                }
                throw errors;
            }
            return type_info;
        }
    }

    TypeInfo Analyzer::resolve_assign(const std::shared_ptr<ast::Type> &type, const std::shared_ptr<ast::Expression> &expr,
                                      const ast::AstNode &node) {
        TypeInfo type_info;
        if (type && expr) {
            type->accept(this);
            type_info = _res_type_info;

            if (const auto scope = dynamic_cast<scope::Variable *>(get_current_scope())) {
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
            resolve_indexer(_res_expr_info, true, node);
            switch (_res_expr_info.tag) {
            case ExprInfo::Kind::NORMAL:
                type_info = _res_expr_info.type_info();
                break;
            case ExprInfo::Kind::STATIC:
                type_info.reset();    // `type` literal
                break;
            case ExprInfo::Kind::MODULE:
                throw error("cannot assign a module to a variable", &node);
            case ExprInfo::Kind::FUNCTION_SET:
                // TODO: implement function types
                throw Unreachable();
            }
        } else {
            // type_info.reset();
            type_info.basic().type = get_internal<scope::Compound>(Internal::SPADE_ANY);
            // TODO: check for functions
            // nullable by default
            type_info.nullable() = true;
        }
        // Assigning to a variable, so set the type info
        if (const auto scope = dynamic_cast<scope::Variable *>(get_current_scope())) {
            scope->set_type_info(type_info);
            scope->set_eval(scope::Variable::Eval::DONE);
        }
        return type_info;
    }

    void Analyzer::check_fun_params(const std::vector<ArgumentInfo> &arg_infos, const std::vector<ParamInfo> pos_only,
                                    const std::vector<ParamInfo> pos_kwd, const std::vector<ParamInfo> kwd_only, const ast::AstNode &node,
                                    ErrorGroup<AnalyzerError> &err_grp) {
        // Separate out the value and keyword arguments
        std::vector<ArgumentInfo> value_args;
        std::unordered_map<string, ArgumentInfo> kwargs;
        for (const auto &arg_info: arg_infos) {
            if (arg_info.b_kwd)
                kwargs[arg_info.name] = arg_info;
            else
                value_args.emplace_back(arg_info);
        }

        size_t arg_id = 0;
        // Check positional only parameters
        if (value_args.size() < pos_only.size()) {
            err_grp.error(error(std::format("expected {} positional arguments but got {}", pos_only.size(), arg_infos.size()), &node));
        } else
            for (const auto &param: pos_only) {
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
        for (const auto &param: pos_kwd) {
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
            err_grp.error(error(std::format("expected at most {} value arguments but got {} value arguments", arg_id, value_args.size()), &node));
        // Add kwd_only_params to kwd_params map
        for (const auto &param: kwd_only) {
            if (!param.b_default && !param.b_variadic)
                min_kw_arg_count++;
            if (param.b_variadic)
                kwd_only_variadic = param;
            else
                kwd_params[param.name] = param;
        }
        // Minimum keyword arguments should be present
        if (kwargs.size() < min_kw_arg_count)
            err_grp.error(error(std::format("expected at least {} keyword arguments but got {} keyword arguments", min_kw_arg_count, kwargs.size()),
                                &node));
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
    }

    bool Analyzer::check_fun_call(scope::Function *function, const std::vector<ArgumentInfo> &arg_infos, const ast::AstNode &node,
                                  ErrorGroup<AnalyzerError> &errors) {
        if (!function->is_variadic() && !function->is_default() && function->param_count() != arg_infos.size()) {
            errors.error(error(std::format("expected {} arguments but got {}", function->param_count(), arg_infos.size()), &node))
                    .note(error("declared here", function));
            return false;
        }
        if (arg_infos.size() < function->min_param_count()) {
            errors.error(error(std::format("expected at least {} arguments but got {}", function->min_param_count(), arg_infos.size()), &node))
                    .note(error("declared here", function));
            return false;
        }

        ErrorGroup<AnalyzerError> err_grp;

        check_fun_params(arg_infos, function->get_pos_only_params(), function->get_pos_kwd_params(), function->get_kwd_only_params(), node, err_grp);

        if (err_grp) {
            err_grp.note(error("declared here", function));
            errors.extend(err_grp);
            return false;
        }
        return true;
    }

    bool Analyzer::check_fun_call(const FunctionType &function, const std::vector<ArgumentInfo> &arg_infos, const ast::AstNode &node,
                                  ErrorGroup<AnalyzerError> &errors) {
        if (!function.is_variadic() && !function.is_default() && function.param_count() != arg_infos.size()) {
            errors.error(error(std::format("expected {} arguments but got {}", function.param_count(), arg_infos.size()), &node));
            return false;
        }
        if (arg_infos.size() < function.min_param_count()) {
            errors.error(error(std::format("expected at least {} arguments but got {}", function.min_param_count(), arg_infos.size()), &node));
            return false;
        }

        ErrorGroup<AnalyzerError> err_grp;

        check_fun_params(arg_infos, function.pos_only_params(), function.pos_kwd_params(), function.kwd_only_params(), node, err_grp);

        if (err_grp) {
            errors.extend(err_grp);
            return false;
        }
        return true;
    }

    ExprInfo Analyzer::resolve_call(const FunctionInfo &funs, const std::vector<ArgumentInfo> &arg_infos, const ast::AstNode &node) {
        // Check for redeclarations if any
        for (const auto &[_, fun_set]: funs.get_function_sets()) {
            if (!fun_set->is_redecl_check()) {
                // fun_set can never be empty (according to scope tree builder)
                const auto old_cur_scope = get_current_scope();
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
                        if (param.type_info != arg_infos[i].expr_info.type_info())
                            priority = 3;
                        i++;
                    }
                    for (auto param: fun->get_pos_kwd_params()) {
                        if (priority == 3)
                            break;
                        if (param.type_info != arg_infos[i].expr_info.type_info())
                            priority = 3;
                        i++;
                    }
                    for (auto param: fun->get_kwd_only_params()) {
                        if (priority == 3)
                            break;
                        if (param.type_info != arg_infos[i].expr_info.type_info())
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
                    err_grp.note(error(std::format("possible candidate declared here: '{}'", fun->to_string()), fun->get_node()))
                            .note(error("this error should not have occurred, please raise a github issue"));
                throw err_grp;
            }
            candidate = candidates[0];
        }

        resolve_context(candidate, node);

        candidate->increase_usage();
        LOGGER.log_debug(std::format("resolved call candidate: {}", candidate->to_string()));

        ExprInfo expr_info;
        expr_info.tag = ExprInfo::Kind::NORMAL;
        expr_info.type_info() = candidate->get_ret_type();
        expr_info.value_info.scope = candidate;
        // TODO: also convey generic info
        return expr_info;
    }

    void Analyzer::resolve_indexer(ExprInfo &result, bool get, const ast::AstNode &node) {
        if (indexer_info) {
            ErrorGroup<AnalyzerError> errors;

            // mimic as if it was non-nullable because we check nullabilty is already checked in the indexer visitor
            ExprInfo caller_info;
            caller_info = indexer_info.caller_info;
            caller_info.type_info().nullable() = false;

            auto member = get_member(caller_info, get ? OV_OP_GET_ITEM : OV_OP_SET_ITEM, node, errors);
            if (errors)
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("'{}' is not indexable", indexer_info.caller_info.to_string()), indexer_info.node))
                        .extend(errors);
            switch (member.tag) {
            case ExprInfo::Kind::NORMAL:
            case ExprInfo::Kind::STATIC:
            case ExprInfo::Kind::MODULE:
                throw error(std::format("'{}' is not indexable", indexer_info.caller_info.to_string()), indexer_info.node);
                break;
            case ExprInfo::Kind::FUNCTION_SET: {
                result = resolve_call(member.functions(), indexer_info.arg_infos, node);
                // This is the property of safe indexer operator
                // where 'a?[...]' returns 'a[...]' if 'a' is not null, else returns null
                if (indexer_info.caller_info.type_info().nullable())
                    result.type_info().nullable() = true;
                break;
            }
            }
            indexer_info.reset();
        }
    }

    ExprInfo Analyzer::get_var_expr_info(scope::Variable *var_scope, const ast::AstNode &node) {
        ExprInfo expr_info;
        expr_info.tag = ExprInfo::Kind::NORMAL;
        switch (var_scope->get_eval()) {
        case scope::Variable::Eval::NOT_STARTED: {
            const auto old_cur_scope = get_current_scope();    // save context
            cur_scope = var_scope->get_parent();               // change context
            var_scope->get_node()->accept(this);               // visit variable
            cur_scope = old_cur_scope;                         // reset context
            expr_info.type_info() = var_scope->get_type_info();
            break;
        }
        case scope::Variable::Eval::PROGRESS:
            if (get_current_scope()->get_type() == scope::ScopeType::VARIABLE) {
                auto cur_var_scope = cast<scope::Variable>(get_current_scope());
                if (cur_var_scope->get_eval() == scope::Variable::Eval::DONE) {
                    expr_info.type_info() = cur_var_scope->get_type_info();    // sense correct
                    break;
                }
            }
            expr_info.type_info().basic().type = get_internal<scope::Compound>(Internal::SPADE_ANY);
            expr_info.type_info().nullable() = true;
            warning(std::format("type inference is ambiguous, defaulting to '{}'", expr_info.type_info().to_string()), &node);
            note("declared here", var_scope);
            break;
        case scope::Variable::Eval::DONE:
            expr_info.type_info() = var_scope->get_type_info();
            break;
        }
        if (var_scope->get_variable_node()->get_token()->get_type() == TokenType::CONST)
            expr_info.value_info.b_const = true;
        return expr_info;
    }

    std::shared_ptr<scope::Variable> Analyzer::declare_variable(ast::decl::Variable &node) {
        auto scope = std::make_shared<scope::Variable>(&node);
        auto name = node.get_name();
        if (const auto fun = get_current_function()) {
            // Check if the variable is not overshadowing parameters
            if (fun->has_param(name->get_text()))
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("function parameters cannot be overshadowed '{}'", name->get_text()), name))
                        .note(error("already declared here", fun->get_param(name->get_text()).node));
            // Add the variable to the parent scope
            if (!get_current_scope()->new_variable(name, scope))
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("redeclaration of '{}'", name->get_text()), name))
                        .note(error("already declared here", get_current_scope()->get_variable(name->get_text())));
        }
        cur_scope = &*scope;
        return scope;
    }

    static bool check_fun_kwd_params(const scope::Function *fun1, const std::vector<ParamInfo> &fun1_pos_kwd,
                                     const std::vector<ParamInfo> &fun1_pos_kwd_default, const scope::Function *fun2,
                                     const std::vector<ParamInfo> &fun2_pos_kwd, const std::vector<ParamInfo> &fun2_pos_kwd_default);

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
    void Analyzer::check_funs(const scope::Function *fun1, const scope::Function *fun2, ErrorGroup<AnalyzerError> &errors) const {
#define AMBIGUOUS()                                                                                                                                  \
    do {                                                                                                                                             \
        errors.error(error(std::format("ambiguous declaration of '{}'", fun1->to_string()), fun1))                                                   \
                .note(error(std::format("check another declaration here: '{}'", fun2->to_string()), fun2));                                          \
        return;                                                                                                                                      \
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
                    if (!fun1_pos_only[i].type_info.weak_equals(fun2_pos_only[i].type_info))
                        return;
                }
                size_t min_size = std::min(fun1_pos_only.size(), fun2_pos_only.size());
                fun1_pos_only.erase(fun1_pos_only.begin(), fun1_pos_only.begin() + min_size);
                fun2_pos_only.erase(fun2_pos_only.begin(), fun2_pos_only.begin() + min_size);
            }
            if (!fun1_pos_only.empty() && !fun2_pos_kwd.empty()) {
                for (size_t i = 0; i < fun1_pos_only.size() && i < fun2_pos_kwd.size(); i++) {
                    if (!fun1_pos_only[i].type_info.weak_equals(fun2_pos_kwd[i].type_info))
                        return;
                }
                size_t min_size = std::min(fun1_pos_only.size(), fun2_pos_kwd.size());
                fun1_pos_only.erase(fun1_pos_only.begin(), fun1_pos_only.begin() + min_size);
                fun2_pos_kwd.erase(fun2_pos_kwd.begin(), fun2_pos_kwd.begin() + min_size);
            }
            if (!fun1_pos_kwd.empty() && !fun2_pos_only.empty()) {
                for (size_t i = 0; i < fun1_pos_kwd.size() && i < fun2_pos_only.size(); i++) {
                    if (!fun1_pos_kwd[i].type_info.weak_equals(fun2_pos_only[i].type_info))
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
                    if (!fun1_pos_kwd[i].type_info.weak_equals(fun2_pos_kwd[i].type_info)) {
                        if (check_fun_kwd_params(fun1, fun1_pos_kwd, fun1_pos_kwd_default, fun2, fun2_pos_kwd, fun2_pos_kwd_default))
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
                     i < fun1_pos_kwd.size() + fun1_pos_kwd_default.size() && i < fun2_pos_kwd.size() + fun2_pos_kwd_default.size(); i++) {
                    auto param1 = i < fun1_pos_kwd.size() ? fun1_pos_kwd[i] : fun1_pos_kwd_default[i - fun1_pos_kwd.size()];
                    auto param2 = i < fun2_pos_kwd.size() ? fun2_pos_kwd[i] : fun2_pos_kwd_default[i - fun2_pos_kwd.size()];
                    if (i >= fun1_pos_kwd.size() && i >= fun2_pos_kwd.size()) {
                        // if both are default then types must be different
                        if (param1.type_info.weak_equals(param2.type_info))
                            AMBIGUOUS();
                    } else if (!param1.type_info.weak_equals(param2.type_info)) {
                        if (check_fun_kwd_params(fun1, fun1_pos_kwd, fun1_pos_kwd_default, fun2, fun2_pos_kwd, fun2_pos_kwd_default))
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
                                     const std::vector<ParamInfo> &fun2_pos_kwd, const std::vector<ParamInfo> &fun2_pos_kwd_default) {
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
                    if (param.type_info.weak_equals(fun2_kwd[name].type_info))
                        return false;
                } else if (!param.type_info.weak_equals(fun2_kwd[name].type_info))
                    return true;
                fun2_kwd.erase(name);
            } else if (param.b_default)
                continue;
            else if (!fun2_kwd_only_variadic || !param.type_info.weak_equals(fun2_kwd_only_variadic->type_info))
                return true;
        }
        for (const auto &[name, param]: fun2_kwd) {
            if (param.b_default)
                continue;
            else if (!fun2_kwd_only_variadic || !param.type_info.weak_equals(fun2_kwd_only_variadic->type_info))
                return true;
        }
        return false;
    }

    void Analyzer::check_fun_set(const std::shared_ptr<scope::FunctionSet> &fun_set) {
        const auto old_cur_scope = get_current_scope();
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
                if (errors) {
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

        if (err_grp)
            throw err_grp;
        cur_scope = old_cur_scope;
    }

    ExprInfo Analyzer::get_member(const ExprInfo &caller_info, const string &member_name, bool safe, const ast::AstNode &node,
                                  ErrorGroup<AnalyzerError> &errors) {
        ExprInfo expr_info;

        switch (caller_info.tag) {
        case ExprInfo::Kind::NORMAL: {
            if (caller_info.type_info().tag != TypeInfo::Kind::BASIC) {
                errors.error(error(std::format("cannot access from type '{}'", caller_info.to_string()), &node));
                return expr_info;
            }
            if (caller_info.is_null()) {
                errors.error(error("cannot access 'null'", &node));
                return expr_info;
            }
            if (caller_info.type_info().nullable() && !safe) {
                errors.error(error(std::format("cannot access member of nullable '{}'", caller_info.to_string()), &node))
                        .help(error("use safe dot access operator '?.'"));
                return expr_info;
            }
            if (!caller_info.type_info().nullable() && safe) {
                errors.error(error(std::format("cannot use safe dot access operator on non-nullable '{}'", caller_info.to_string()), &node))
                        .help(error("use the normal dot access operator '.' instead of '?.'"));
                return expr_info;
            }
            if (caller_info.type_info().basic().is_type_literal()) {
                warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                expr_info.tag = ExprInfo::Kind::NORMAL;
                expr_info.type_info().basic().type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                expr_info.type_info().nullable() = true;
            } else {
                auto member_scope = caller_info.type_info().basic().type->get_variable(member_name);
                if (!member_scope)
                    // Provision of super fields and functions
                    if (const auto compound = caller_info.type_info().basic().type) {
                        if (compound->get_super_fields().contains(member_name)) {
                            member_scope = compound->get_super_fields().at(member_name);
                        } else if (compound->get_super_functions().contains(member_name)) {
                            expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
                            expr_info.value_info.b_const = true;
                            expr_info.functions() = compound->get_super_functions().at(member_name);
                            break;
                        }
                    }
                if (!member_scope) {
                    std::unordered_set<string> names;
                    for (const auto &[name, _]: caller_info.type_info().basic().type->get_members()) names.insert(name);
                    const auto results = fuzzy_search_best(member_name, names);

                    errors.error(error(std::format("'{}' has no member named '{}'", caller_info.to_string(), member_name), &node));
                    if (results.size() == 1)
                        errors.help(error(std::format("did you mean '{}'?", *results.begin())));
                    else {
                        string final_str;
                        for (const auto &result: results) {
                            final_str += "'" + result + "', ";
                        }
                        final_str.pop_back();
                        final_str.pop_back();
                        errors.help(error(std::format("did you mean one of {}?", final_str)));
                    }
                    return expr_info;
                }
                resolve_context(member_scope, node);
                switch (member_scope->get_type()) {
                case scope::ScopeType::COMPOUND:
                    expr_info.tag = ExprInfo::Kind::STATIC;
                    expr_info.value_info.b_const = true;
                    expr_info.type_info().basic().type = cast<scope::Compound>(&*member_scope);
                    break;
                case scope::ScopeType::FUNCTION:
                    throw Unreachable();    // surely some symbol tree builder error
                case scope::ScopeType::FUNCTION_SET:
                    expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
                    expr_info.value_info.b_const = true;
                    expr_info.functions() = cast<scope::FunctionSet>(&*member_scope);
                    break;
                case scope::ScopeType::VARIABLE:
                    expr_info = get_var_expr_info(&*cast<scope::Variable>(member_scope), node);
                    break;
                case scope::ScopeType::ENUMERATOR:
                    errors.error(error("cannot access enumerator from an object (you should use the type)", &node))
                            .help(error(std::format("use {}.{}", caller_info.type_info().basic().type->to_string(false), member_name)));
                    return expr_info;
                case scope::ScopeType::FOLDER_MODULE:
                case scope::ScopeType::MODULE:
                case scope::ScopeType::LAMBDA:
                case scope::ScopeType::BLOCK:
                    throw Unreachable();    // surely some parser error
                }
                expr_info.value_info.scope = &*member_scope;
            }
            break;
        }
        case ExprInfo::Kind::STATIC: {
            if (caller_info.type_info().tag != TypeInfo::Kind::BASIC) {
                errors.error(error(std::format("cannot access from type '{}'", caller_info.to_string()), &node));
                return expr_info;
            }
            if (caller_info.type_info().nullable() && !safe) {
                errors.error(error(std::format("cannot access member of nullable '{}'", caller_info.to_string()), &node))
                        .help(error("use safe dot access operator '?.'"));
                return expr_info;
            }
            if (!caller_info.type_info().nullable() && safe) {
                errors.error(error(std::format("cannot use safe dot access operator on non-nullable '{}'", caller_info.to_string()), &node))
                        .help(error("use the normal dot access operator '.' instead of '?.'"));
                return expr_info;
            }
            if (caller_info.type_info().basic().is_type_literal()) {
                warning("'type' causes dynamic resolution, hence expression becomes 'spade.any?'", &node);
                expr_info.tag = ExprInfo::Kind::NORMAL;
                expr_info.type_info().basic().type = get_internal<scope::Compound>(Internal::SPADE_ANY);
                expr_info.type_info().nullable() = true;
            } else {
                auto member_scope = caller_info.type_info().basic().type->get_variable(member_name);
                if (!member_scope) {
                    errors.error(error(std::format("'{}' has no member named '{}'", caller_info.to_string(), member_name), &node));
                    return expr_info;
                }
                resolve_context(member_scope, node);
                switch (member_scope->get_type()) {
                case scope::ScopeType::COMPOUND:
                    expr_info.tag = ExprInfo::Kind::STATIC;
                    expr_info.value_info.b_const = true;
                    expr_info.type_info().basic().type = cast<scope::Compound>(&*member_scope);
                    break;
                case scope::ScopeType::FUNCTION:
                    throw Unreachable();    // surely some symbol tree builder error
                case scope::ScopeType::FUNCTION_SET:
                    expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
                    expr_info.value_info.b_const = true;
                    expr_info.functions() = cast<scope::FunctionSet>(&*member_scope);
                    expr_info.functions().remove_if([](const std::pair<const SymbolPath &, const scope::Function *> &item) {
                        return !item.second->is_static() && !item.second->is_init();
                    });
                    if (expr_info.functions().empty()) {
                        errors.error(error(std::format("cannot access non-static '{}' of '{}'", member_scope->to_string(), caller_info.to_string()),
                                           &node));
                        return expr_info;
                    }
                    break;
                case scope::ScopeType::VARIABLE: {
                    auto var_scope = cast<scope::Variable>(member_scope);
                    if (!var_scope->is_static()) {
                        errors.error(
                                error(std::format("cannot access non-static '{}' of '{}'", var_scope->to_string(), caller_info.to_string()), &node));
                        return expr_info;
                    }
                    expr_info = get_var_expr_info(&*var_scope, node);
                    break;
                }
                case scope::ScopeType::ENUMERATOR:
                    expr_info.type_info().basic().type = caller_info.type_info().basic().type;
                    expr_info.value_info.b_const = true;
                    expr_info.tag = ExprInfo::Kind::NORMAL;
                    break;
                case scope::ScopeType::FOLDER_MODULE:
                case scope::ScopeType::MODULE:
                case scope::ScopeType::LAMBDA:
                case scope::ScopeType::BLOCK:
                    throw Unreachable();    // surely some parser error
                }
                expr_info.value_info.scope = &*member_scope;
            }
            break;
        }
        case ExprInfo::Kind::MODULE: {
            if (safe) {
                errors.error(error("cannot use safe dot access operator on a module", &node));
                return expr_info;
            }
            if (!caller_info.module()->has_variable(member_name)) {
                errors.error(error(std::format("cannot access member: '{}'", member_name), &node));
                return expr_info;
            }
            auto member_scope = caller_info.module()->get_variable(member_name);
            if (!member_scope) {
                errors.error(error(std::format("'{}' has no member named '{}'", caller_info.to_string(), member_name), &node));
                return expr_info;
            }
            resolve_context(member_scope, node);
            switch (member_scope->get_type()) {
            case scope::ScopeType::FOLDER_MODULE:
            case scope::ScopeType::MODULE:
                expr_info.tag = ExprInfo::Kind::MODULE;
                expr_info.value_info.b_const = true;
                expr_info.module() = cast<scope::Module>(&*member_scope);
                break;
            case scope::ScopeType::COMPOUND:
                expr_info.tag = ExprInfo::Kind::STATIC;
                expr_info.value_info.b_const = true;
                expr_info.type_info().basic().type = cast<scope::Compound>(&*member_scope);
                break;
            case scope::ScopeType::FUNCTION:
                throw Unreachable();    // surely some symbol tree builder error
            case scope::ScopeType::FUNCTION_SET:
                expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
                expr_info.value_info.b_const = true;
                expr_info.functions() = cast<scope::FunctionSet>(&*member_scope);
                break;
            case scope::ScopeType::VARIABLE:
                expr_info = get_var_expr_info(&*cast<scope::Variable>(member_scope), node);
                break;
            case scope::ScopeType::LAMBDA:
            case scope::ScopeType::BLOCK:
            case scope::ScopeType::ENUMERATOR:
                throw Unreachable();    // surely some parser error
            }
            expr_info.value_info.scope = &*member_scope;
            break;
        }
        case ExprInfo::Kind::FUNCTION_SET: {
            errors.error(error("cannot access member of callable type", &node));
            return expr_info;
        }
        }

        // This is the property of safe dot operator
        // where 'a?.b' returns 'a.b' if 'a' is not null, else returns null
        if (safe) {
            switch (expr_info.tag) {
            case ExprInfo::Kind::NORMAL:
            case ExprInfo::Kind::STATIC:
                expr_info.type_info().nullable() = true;
                break;
            case ExprInfo::Kind::MODULE:
                break;
            case ExprInfo::Kind::FUNCTION_SET:
                expr_info.functions().b_nullable = true;
                break;
            }
        }
        expr_info.value_info.b_lvalue = caller_info.value_info.b_lvalue;
        // Fix for `self.a` const error bcz `self.a` is not constant if it is declared non-const
        if (!expr_info.value_info.b_const)
            expr_info.value_info.b_const = caller_info.value_info.b_const && !caller_info.value_info.b_self;
        expr_info.value_info.b_self = false;

        return expr_info;
    }

    ExprInfo Analyzer::get_member(const ExprInfo &caller_info, const string &member_name, const ast::AstNode &node,
                                  ErrorGroup<AnalyzerError> &errors) {
        return get_member(caller_info, member_name, false, node, errors);
    }

    ExprInfo Analyzer::get_member(const ExprInfo &caller_info, const string &member_name, bool safe, const ast::AstNode &node) {
        ErrorGroup<AnalyzerError> errors;
        auto expr_info = get_member(caller_info, member_name, safe, node, errors);
        if (errors)
            throw errors;
        return expr_info;
    }

    ExprInfo Analyzer::get_member(const ExprInfo &caller_info, const string &member_name, const ast::AstNode &node) {
        ErrorGroup<AnalyzerError> errors;
        auto expr_info = get_member(caller_info, member_name, node, errors);
        if (errors)
            throw errors;
        return expr_info;
    }

    ExprInfo Analyzer::eval_expr(const std::shared_ptr<ast::Expression> &expr, const ast::AstNode &node) {
        expr->accept(this);
        if (const auto scope = _res_expr_info.value_info.scope)
            scope->increase_usage();
        resolve_indexer(_res_expr_info, true, node);
        return _res_expr_info;
    }

    std::shared_ptr<scope::Module> Analyzer::resolve_file(const fs::path &path) {
        auto file_path = fs::canonical(path);
        if (!basic_mode && module_scopes.contains(file_path))
            // Do not reload if it is already resolved
            return cast<scope::Module>(module_scopes[file_path]);

        // Process the file as usual
        std::ifstream in(file_path);
        if (!in)
            return null;
        std::stringstream buffer;
        buffer << in.rdbuf();
        Lexer lexer(file_path, buffer.str());
        Parser parser(file_path, &lexer);
        auto tree = parser.parse();
        ScopeTreeBuilder builder(tree);
        auto module = builder.build();
        module->claim(tree);

        if (!basic_mode)
            module_scopes[path] = module;    // Set it resolved

        // Resolve import declarations
        const auto old_cur_scope = get_current_scope();
        cur_scope = &*module;
        for (const auto &import: tree->get_imports()) import->accept(this);
        cur_scope = old_cur_scope;

        return module;
    }

    std::shared_ptr<scope::FolderModule> Analyzer::resolve_directory(const fs::path &path) {
        auto dir_path = fs::canonical(path);
        if (module_scopes.contains(dir_path))
            // Do not retry if it is already resolved
            return cast<scope::FolderModule>(module_scopes[dir_path]);
        // Process the directory by recursively traversing all spade source files
        std::shared_ptr<scope::FolderModule> module = std::make_shared<scope::FolderModule>();
        bool special_module = fs::exists(dir_path / "mod.sp");
        for (const auto &entry: fs::directory_iterator(dir_path)) {
            auto entry_path = fs::canonical(entry.path());
            if (entry_path == dir_path || entry_path == dir_path.parent_path())
                // Skip current and parent path entries
                continue;
            if (entry_path.filename().string()[0] == '.')
                // Skip '.XXX' entries
                continue;
            if (!basic_mode && fs::canonical(entry_path) == fs::canonical(compiler_options.basic_module_path))
                // Skip basic module as it is processed differently
                continue;
            std::shared_ptr<scope::Scope> scope;
            if (entry.is_directory())
                scope = resolve_directory(entry_path);
            if (entry.is_regular_file() && entry_path.extension() == ".sp")
                scope = resolve_file(entry_path);
            if (!scope)
                continue;
            if (special_module) {
                // Append all the inner scopes if it is a special module
                for (const auto &[member_name, member]: scope->get_members()) {
                    module->new_variable(member_name, member.first, member.second);
                }
            } else {
                module->new_variable(entry_path.stem().string(), null, scope);
            }
        }
        module_scopes[dir_path] = module;    // Set the path as resolved
        return module;
    }

    void Analyzer::check_usages(const std::shared_ptr<scope::Scope> &scope) {
        string name;
        {
            const string str = scope->get_path().get_name();
            name = str.substr(0, str.find_first_of('('));
        }

        if (name[0] != '_') {
            switch (scope->get_type()) {
            case scope::ScopeType::FOLDER_MODULE:
            case scope::ScopeType::MODULE:
            case scope::ScopeType::LAMBDA:
            case scope::ScopeType::BLOCK:
            case scope::ScopeType::FUNCTION_SET:
                break;
            case scope::ScopeType::COMPOUND:
                if (!cast<scope::Compound>(scope)->is_public())
                    if (scope->get_usage_count() == 0) {
                        warning(std::format("'{}' was never used", scope->to_string()), scope);
                        help(std::format("rename '{0:}' to '_{0:}' if you mean to keep it", name));
                        help(std::format("remove '{:}' as it is never used", name));
                        help(std::format("declare '{}' as 'public'", scope->to_string()));
                    }
                break;
            case scope::ScopeType::FUNCTION:
                if (!cast<scope::Function>(scope)->is_public())
                    if (scope->get_usage_count() == 0) {
                        warning(std::format("'{}' was never used", scope->to_string()), scope);
                        help(std::format("rename '{0:}' to '_{0:}' if you mean to keep it", name));
                        help(std::format("remove '{:}' as it is never used", name));
                        help(std::format("declare '{}' as 'public'", scope->to_string()));
                    }
                break;
            case scope::ScopeType::ENUMERATOR:
                if (!scope->get_enclosing_compound()->is_public())
                    if (scope->get_usage_count() == 0) {
                        warning(std::format("'{}' was never used", scope->to_string()), scope);
                        help(std::format("rename '{0:}' to '_{0:}' if you mean to keep it", name));
                        help(std::format("remove '{:}' as it is never used", name));
                        help(std::format("declare '{}' as 'public'", scope->get_enclosing_compound()->to_string()));
                    }
                break;
            case scope::ScopeType::VARIABLE: {
                const auto var = cast<scope::Variable>(scope);
                const auto MONTAGUE = [&](bool show_publicity) {
                    if (var->is_const()) {
                        if (var->get_usage_count() == 0) {
                            warning("constant was never accessed", var);
                            help(std::format("rename '{0:}' to '_{0:}' if you mean to keep it", name));
                            help(std::format("remove '{:}' as it is never used", name));
                            if (show_publicity)
                                help(std::format("declare '{}' as 'public'", var->to_string()));
                        }
                    } else {
                        if (var->get_usage_count() == 0 && !var->is_assigned()) {
                            warning("variable was never accessed or assigned", var);
                            help(std::format("rename '{0:}' to '_{0:}' if you mean to keep it", name));
                            help(std::format("remove '{:}' as it is never used", name));
                            if (show_publicity)
                                help(std::format("declare '{}' as 'public'", var->to_string()));
                        } else if (var->get_usage_count() == 0) {
                            warning("variable was never accessed", var);
                            help(std::format("rename '{0:}' to '_{0:}' if you mean to keep it", name));
                            help(std::format("remove '{:}' as it is never used", name));
                            if (show_publicity)
                                help(std::format("declare '{}' as 'public'", var->to_string()));
                        } else if (!var->is_assigned()) {
                            warning("variable was never assigned", var);
                            if (show_publicity)
                                help(std::format("declare '{}' as 'public'", var->to_string()));
                        }
                    }
                };

                if (!var->get_enclosing_function()) {
                    if (!var->is_public())
                        MONTAGUE(true);
                } else
                    MONTAGUE(false);
                break;
            }
            }
        }

        // Perform a DFS on the scope tree
        for (const auto &[_1, member]: scope->get_members()) {
            const auto &[_2, member_scope] = member;
            check_usages(member_scope);
        }
    }

    void Analyzer::analyze() {
        // Load the basic module
        load_internal_modules();

        mode = Mode::DECLARATION;

        // Resolve all import declarations
        auto module = cast<scope::Module>(module_scopes.begin()->second);
        cur_scope = &*module;
        for (const auto &import: module->get_module_node()->get_imports()) import->accept(this);

        LOGGER.log_debug("============================================================");
        LOGGER.log_debug("                COMPILER SEMANTIC ANALYSIS");
        LOGGER.log_debug("============================================================");

        // Visit all declarations
        for (const auto &[_, module_scope]: module_scopes) {
            if (const auto node = module_scope->get_node()) {
                cur_scope = null;
                node->accept(this);
            }
        }

        // Visit function definitions
        mode = Mode::DEFINITION;
        for (auto function: function_scopes) {
            cur_scope = function->get_parent()->get_parent();
            function->get_node()->accept(this);
        }

        // Check for usage diagnostics
        for (const auto &[_, module_scope]: module_scopes) {
            auto old_cur_scope = get_current_scope();
            cur_scope = &*module_scope;

            if (auto module = std::dynamic_pointer_cast<scope::Module>(module_scope)) {
                for (const auto &[_, import]: module->get_imports()) {
                    if (!import.b_used) {
                        warning("unused import", import.node);
                        help("remove the import declaration");
                    }
                }
                for (const auto &import: module->get_open_imports()) {
                    if (!import.b_used) {
                        warning("unused import", import.node);
                        help("remove the import declaration");
                    }
                }
            }
            check_usages(module_scope);

            cur_scope = old_cur_scope;
        }

        // Print ast to log
        {
            {
                std::stringstream ss;
                LOGGER.log_debug("============================================================");
                LOGGER.log_debug("                    COMPILER AST OUTPUT");
                LOGGER.log_debug("============================================================");
                Printer printer(internals[Internal::SPADE]->get_node());
                printer.write_to(ss);
                LOGGER.log_debug(ss.str());
            }
            for (const auto &[_, module]: module_scopes) {
                std::stringstream ss;
                LOGGER.log_debug("============================================================");
                LOGGER.log_debug("                    COMPILER AST OUTPUT");
                LOGGER.log_debug("============================================================");
                Printer printer(module->get_node());
                printer.write_to(ss);
                LOGGER.log_debug(ss.str());
            }
        }
    }

    void Analyzer::visit(ast::Reference &node) {
        // Find the scope where name is located
        scope::Scope *scope = null;
        auto expr_info = resolve_name(node.get_path()[0]->get_text(), node);
        switch (expr_info.tag) {
        case ExprInfo::Kind::NORMAL:
            scope = expr_info.type_info().basic().type;
            break;
        case ExprInfo::Kind::STATIC:
            scope = expr_info.type_info().basic().type;
            break;
        case ExprInfo::Kind::MODULE:
            scope = expr_info.module();
            break;
        case ExprInfo::Kind::FUNCTION_SET:
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
            _res_expr_info.tag = ExprInfo::Kind::MODULE;
            _res_expr_info.module() = cast<scope::Module>(scope);
            break;
        case scope::ScopeType::COMPOUND:
            _res_expr_info.tag = ExprInfo::Kind::STATIC;
            _res_expr_info.type_info().basic().type = cast<scope::Compound>(scope);
            break;
        case scope::ScopeType::FUNCTION:
            throw Unreachable();
        case scope::ScopeType::FUNCTION_SET:
            _res_expr_info.tag = ExprInfo::Kind::FUNCTION_SET;
            _res_expr_info.functions() = expr_info.functions();
            break;
        case scope::ScopeType::BLOCK:
        case scope::ScopeType::LAMBDA:
            throw Unreachable();
        case scope::ScopeType::VARIABLE:
        case scope::ScopeType::ENUMERATOR:
            _res_expr_info.tag = ExprInfo::Kind::NORMAL;
            _res_expr_info.type_info().basic().type = scope->get_enclosing_compound();
            break;
        }
    }

    void Analyzer::visit(ast::type::Reference &node) {
        // Find the type scope
        node.get_reference()->accept(this);
        // Check if the reference is a type
        if (_res_expr_info.tag != ExprInfo::Kind::STATIC)
            throw error("reference is not a type", &node);
        auto type_scope = _res_expr_info.type_info().basic().type;
        // Check for type arguments
        std::vector<TypeInfo> type_args;
        for (auto type_arg: node.get_type_args()) {
            type_arg->accept(this);
            type_args.push_back(_res_type_info);
        }
        _res_type_info.reset();
        _res_type_info.basic().type = type_scope;
        _res_type_info.basic().type_args = type_args;
    }

    void Analyzer::visit(ast::type::Function &node) {
        std::vector<ParamInfo> params;
        for (const auto &param_type: node.get_param_types()) {
            param_type->accept(this);
            params.emplace_back(ParamInfo{.name = "", .type_info = _res_type_info, .node = &*param_type});
        }

        node.get_return_type()->accept(this);
        TypeInfo return_type = _res_type_info;

        _res_type_info.reset();
        _res_type_info.function().return_type() = return_type;
        _res_type_info.function().pos_kwd_params() = params;
    }

    void Analyzer::visit(ast::type::TypeLiteral &node) {
        _res_type_info.reset();
    }

    void Analyzer::visit(ast::type::Nullable &node) {
        _res_type_info.reset();
        node.get_type()->accept(this);
        _res_type_info.nullable() = true;
    }

    void Analyzer::visit(ast::type::TypeBuilder &node) {
        // TODO: implement this
        _res_type_info.reset();
    }

    void Analyzer::visit(ast::type::TypeBuilderMember &node) {
        // TODO: implement this
    }
}    // namespace spade