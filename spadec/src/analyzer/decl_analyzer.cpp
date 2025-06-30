#include <algorithm>
#include <unordered_set>

#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "symbol_path.hpp"
#include "utils/error.hpp"

namespace spade
{
    void Analyzer::visit(ast::decl::TypeParam &node) {}

    void Analyzer::visit(ast::decl::Constraint &node) {}

    void Analyzer::visit(ast::decl::Param &node) {
        ParamInfo param_info;
        param_info.b_const = node.get_is_const() != null;
        param_info.b_variadic = node.get_variadic() != null;
        param_info.b_default = node.get_default_expr() != null;
        param_info.name = node.get_name()->get_text();
        param_info.type_info = resolve_assign(node.get_type(), node.get_default_expr(), node);
        param_info.node = &node;

        _res_param_info.reset();
        _res_param_info = param_info;

        // Diagnostic specific
        param_info.type_info.increase_usage();
    }

    void Analyzer::visit(ast::decl::Params &node) {
        ParamsInfo params;
        std::shared_ptr<ast::decl::Param> found_variadic;
        std::shared_ptr<ast::decl::Param> found_default;

        std::vector<ParamInfo> pos_only_params;
        pos_only_params.reserve(node.get_pos_only().size());
        for (const auto &param: node.get_pos_only()) {
            param->accept(this);
            if (_res_param_info.b_default)
                throw error("positional only parameter cannot have default value", param);
            if (_res_param_info.b_variadic)
                throw error("positional only parameter cannot be variadic", param);
            pos_only_params.push_back(_res_param_info);
        }
        params.pos_only = pos_only_params;

        std::vector<ParamInfo> pos_kwd_params;
        pos_kwd_params.reserve(node.get_pos_kwd().size());
        for (const auto &param: node.get_pos_kwd()) {
            param->accept(this);
            if (_res_param_info.b_variadic) {
                if (found_variadic)
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("variadic parameter is allowed only once", param))
                            .note(error("already declared here", found_variadic));
                found_variadic = param;
            }
            if (!_res_param_info.b_default) {
                if (found_default && !_res_param_info.b_variadic)
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("incorrect ordering of default parameters", param))
                            .note(error("already declared here", found_default));
            } else {
                found_default = param;
            }
            pos_kwd_params.push_back(_res_param_info);
        }
        params.pos_kwd = pos_kwd_params;

        // check for variadic parameters ambiguity
        if (found_variadic /* && !node.get_pos_kwd().empty() */ && node.get_pos_kwd().back() != found_variadic)
            throw error("variadic parameter must be the last parameter", found_variadic);
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
        if (found_variadic && node.get_kwd_only().back() != found_variadic)
            throw ErrorGroup<AnalyzerError>().error(error("variadic parameter must be the last parameter", found_variadic));

        params.kwd_only = kwd_only_params;
        _res_params_info.reset();
        _res_params_info = params;
    }

    void Analyzer::visit(ast::decl::Function &node) {
        if (auto fun_level = get_current_scope()->get_enclosing_function();
            get_current_scope()->get_type() == scope::ScopeType::FUNCTION || fun_level) {
            // TODO: check for function level declarations
            return;
        }

        const auto &fun_set = find_scope<scope::FunctionSet>(node.get_name()->get_text());
        const auto &scope = find_scope<scope::Function>(node.get_qualified_name());

        if (mode == Mode::DECLARATION) {
            if (scope->get_proto_eval() == scope::Function::ProtoEval::NOT_STARTED) {
                scope->set_proto_eval(scope::Function::ProtoEval::PROGRESS);

                if (auto type = node.get_return_type()) {
                    type->accept(this);
                    scope->set_ret_type(_res_type_info);
                } else {
                    TypeInfo return_type;
                    return_type.basic().type =
                            scope->is_init() ? scope->get_enclosing_compound() : get_internal<scope::Compound>(Internal::SPADE_VOID);
                    scope->set_ret_type(return_type);
                }

                if (const auto &params = node.get_params()) {
                    params->accept(this);
                    scope->set_pos_only_params(_res_params_info.pos_only);
                    scope->set_pos_kwd_params(_res_params_info.pos_kwd);
                    scope->set_kwd_only_params(_res_params_info.kwd_only);
                }

#define check_ret_type_bool(OPERATOR)                                                                                                                \
    if (node.get_name()->get_text() == OV_OP_##OPERATOR && scope->get_ret_type().tag == TypeInfo::Kind::BASIC &&                                     \
        scope->get_ret_type().basic().type != get_internal<scope::Compound>(Internal::SPADE_BOOL)) {                                                 \
        throw error(std::format("'{}' must return a '{}'", scope->to_string(), get_internal(Internal::SPADE_BOOL)->to_string()), scope);             \
    }
                // relational operators specific check
                check_ret_type_bool(CONTAINS);
                check_ret_type_bool(LT);
                check_ret_type_bool(LE);
                check_ret_type_bool(EQ);
                check_ret_type_bool(NE);
                check_ret_type_bool(GE);
                check_ret_type_bool(GT);
#undef check_ret_type_bool

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
                    check_fun_set(fun_set);
                    cur_scope = old_cur_scope;    // restore context
                }

                // Check for abstract, final and override functions
                // This code provides the semantics for the `abstract`, `final` and `override` keywords
                if (auto compound = get_current_scope()->get_enclosing_compound()) {
                    if (scope->is_abstract() && !compound->is_abstract())
                        throw error("abstract function cannot be declared in non-abstract class", &node);
                    if (!scope->is_abstract() && compound->get_super_functions().contains(node.get_name()->get_text())) {
                        auto &super_fun_info = compound->get_super_functions()[node.get_name()->get_text()];
                        ErrorGroup<AnalyzerError> errors;
                        std::unordered_set<SymbolPath> to_be_removed;
                        for (const auto &[super_fun_path, super_fun]: super_fun_info.get_functions()) {
                            if (check_fun_exactly_same(&*scope, super_fun)) {
                                if (super_fun->is_abstract()) {
                                    // Diagnostic specific
                                    super_fun->increase_usage();

                                    to_be_removed.insert(super_fun_path);
                                    continue;
                                }
                                if (super_fun->is_final()) {
                                    errors.error(error(std::format("function is marked as final in super '{}'",
                                                                   super_fun->get_enclosing_compound()->to_string()),
                                                       scope))
                                            .note(error("declared here", super_fun));
                                    continue;
                                }
                                if (!scope->is_override()) {
                                    errors.error(error("function overrides another function but is not marked as override", scope))
                                            .note(error("declared here", super_fun));
                                    continue;
                                } else {
                                    // Diagnostic specific
                                    super_fun->increase_usage();
                                }
                            } else
                                // also check if there is any conflict with the super function
                                check_funs(&*scope, super_fun, errors);
                        }
                        // remove super function that are marked as abstract (but implemented in the child class)
                        super_fun_info.remove_if([&](const std::pair<const SymbolPath &, const scope::Function *> &pair) {
                            return to_be_removed.contains(pair.first);
                        });
                        if (errors)
                            throw errors;
                    }
                }

                scope->set_proto_eval(scope::Function::ProtoEval::DONE);
                function_scopes.push_back(scope);
            }

            const auto &definition = node.get_definition();

            if (scope->get_enclosing_function() != null) {
                if (definition == null)
                    throw error("function must have a definition", &node);
            } else if (auto compound = scope->get_enclosing_compound()) {
                switch (compound->get_compound_node()->get_token()->get_type()) {
                case TokenType::CLASS:
                    if (scope->is_init() && definition == null)
                        throw error("constructor must have a definition", &node);
                    if (scope->is_abstract()) {
                        if (definition != null)
                            throw error("abstract function cannot have a definition", &node);
                    } else {
                        if (definition == null)
                            throw error("function must have a definition", &node);
                    }
                    break;
                case TokenType::INTERFACE:
                    if (scope->is_static() && definition == null)
                        throw error("static function must have a definition", &node);
                    break;
                case TokenType::ENUM:
                    if (scope->is_init() && definition == null)
                        throw error("constructor must have a definition", &node);
                    if (definition == null)
                        throw error("function must have a definition", &node);
                    break;
                case TokenType::ANNOTATION:
                    if (scope->is_init() && definition == null)
                        throw error("constructor must have a definition", &node);
                    if (definition == null)
                        throw error("function must have a definition", &node);
                    break;
                default:
                    throw Unreachable();    // surely some parser error
                }
            } else {
                if (definition == null)
                    throw error("function must have a definition", &node);
            }
        }

        if (mode == Mode::DEFINITION)
            if (const auto &definition = node.get_definition())
                definition->accept(this);

        end_scope();    // pop the function
        end_scope();    // pop the function set
    }

    void Analyzer::visit(ast::decl::Variable &node) {
        std::shared_ptr<scope::Variable> scope;
        if (get_current_function()) {
            scope = declare_variable(node);
            scope->set_path(node.get_name()->get_text());
        } else
            scope = find_scope<scope::Variable>(node.get_name()->get_text());

        if (!get_current_function() && !get_current_compound()) {
            if (scope->is_const() && !node.get_expr())
                throw error("globals constants should be initialized when declared", &node);
        }

        if (scope->get_eval() == scope::Variable::Eval::NOT_STARTED) {
            scope->set_eval(scope::Variable::Eval::PROGRESS);
            // resolve_assign automatically sets eval to DONE
            resolve_assign(node.get_type(), node.get_expr(), node);

            // Diagnostic specific
            scope->get_type_info().increase_usage();
            if (node.get_expr())
                scope->set_assigned(true);
        }
        end_scope();
    }

    void Analyzer::visit(ast::decl::Parent &node) {
        node.get_reference()->accept(this);
        // Check if the super class is a scope::Compound
        if (_res_expr_info.tag != ExprInfo::Kind::STATIC)
            throw error("reference is not a type", &node);
        // Get the parent type info and type args if any
        TypeInfo parent_type_info = _res_expr_info.type_info();
        if (!node.get_type_args().empty()) {
            parent_type_info.basic().type_args.reserve(node.get_type_args().size());
            for (const auto &type_arg: node.get_type_args()) {
                type_arg->accept(this);
                parent_type_info.basic().type_args.push_back(_res_type_info);
            }
        }
        _res_type_info.reset();
        _res_type_info = parent_type_info;

        // Diagnostic specific
        _res_type_info.increase_usage();
    }

    void Analyzer::visit(ast::decl::Enumerator &node) {
        auto scope = find_scope<scope::Enumerator>(node.get_name()->get_text());
        auto parent_enum = scope->get_enclosing_compound();
        if (node.get_expr()) {
            if (parent_enum->has_variable("init"))
                throw error(std::format("enumerator cannot have an initializer due to '{}'", parent_enum->get_variable("init")->to_string()),
                            node.get_expr());
        } else if (node.get_args()) {
            auto args = *node.get_args();
            if (!parent_enum->has_variable("init"))
                throw error("enumerator cannot be called with ctor, no declaration provided", node.get_expr());
            FunctionInfo fn_infos(&*cast<scope::FunctionSet>(parent_enum->get_variable("init")));
            // Build args
            std::vector<ArgumentInfo> arg_infos;
            arg_infos.reserve(args.size());
            for (auto arg: args) {
                arg->accept(this);
                if (!arg_infos.empty() && arg_infos.back().b_kwd && !_res_arg_info.b_kwd)
                    throw error("mixing non-keyword and keyword arguments is not allowed", arg);
                arg_infos.push_back(_res_arg_info);
            }
            resolve_call(fn_infos, arg_infos, node);
        }
        end_scope();
    }

    bool Analyzer::check_fun_exactly_same(const scope::Function *fun1, const scope::Function *fun2) {
        if (fun1->is_private() != fun2->is_private())
            return false;
        if (fun1->is_internal() != fun2->is_internal())
            return false;
        if (fun1->is_module_private() != fun2->is_module_private())
            return false;
        if (fun1->is_protected() != fun2->is_protected())
            return false;
        if (fun1->is_public() != fun2->is_public())
            return false;

        return *fun1 == *fun2;
    }

    void Analyzer::check_compatible_supers(const std::shared_ptr<scope::Compound> &klass, const std::vector<scope::Compound *> &supers,
                                           const std::vector<std::shared_ptr<ast::decl::Parent>> &nodes) const {
        std::unordered_map<string, std::shared_ptr<scope::Variable>> super_fields;
        std::unordered_map<string, FunctionInfo> super_functions;
        // member_table : map[string => vector[Scope]]
        //                  where key(string) is the name of the member
        //                        value(vector[Scope]) is the list of matching scopes
        std::unordered_map<string, std::vector<scope::Scope *>> member_table;

        for (const auto super: supers) {
            if (!super)
                continue;
            for (const auto &[member_name, member]: super->get_members()) {
                const auto &[_, member_scope] = member;
                switch (member_scope->get_type()) {
                case scope::ScopeType::FOLDER_MODULE:
                case scope::ScopeType::MODULE:
                case scope::ScopeType::LAMBDA:
                case scope::ScopeType::FUNCTION:
                case scope::ScopeType::BLOCK:
                    throw Unreachable();              // surely some parser or scope tree error
                case scope::ScopeType::COMPOUND:      // nested compounds are static, they are never inherited
                case scope::ScopeType::ENUMERATOR:    // enumerators are static, they are never inherited
                    break;
                case scope::ScopeType::FUNCTION_SET: {
                    member_table[member_name].push_back(&*member_scope);
                    break;
                }
                case scope::ScopeType::VARIABLE: {
                    // Check important side effect of inheritance rules
                    if (const auto &var_scope = cast<scope::Variable>(member_scope); !var_scope->is_static())
                        super_fields[member_name] = var_scope;
                    break;
                }
                }
            }
            // Directly add the fields to the super fields as they do not collide
            super_fields.insert(super->get_super_fields().begin(), super->get_super_fields().end());
            // Check for super functions
            for (const auto &[name, fun_infos]: super->get_super_functions()) {
                for (const auto &[_, fun_set]: fun_infos.get_function_sets()) {
                    member_table[name].push_back(fun_set);
                }
            }
        }

        for (const auto &[member_name, members]: member_table) {
            // IMPORTANT SIDE-EFFECT OF INHERITANCE RULES
            // ----------------------------------------------------------------------------
            //
            // Fields with the same name are not possible because:
            // 1. Static fields can be defined in any compound but they are discarded during inheritance
            // 2. Non-static fields are allowed only in class, enum and annotation
            // 3. Any class can inherit only one class which eliminates duplicate fields in super compounds
            // 4. Enum and annotation can inherit only interfaces (which are not allowed to have non-static fields)
            //
            // Hence, this eliminates the need to check for same fields
            // NOTE: This implies that if members.size() > 1, then all of them are functions
            // ----------------------------------------------------------------------------

            // Check for member functions
            ErrorGroup<AnalyzerError> errors;
            FunctionInfo mem_fns;
            for (const auto &member: members) {
                FunctionInfo fn_infos(&*cast<scope::FunctionSet>(member));
                fn_infos.remove_if([](const std::pair<const spade::SymbolPath &, const spade::scope::Function *> &fn_pair) {
                    return fn_pair.second->is_static() || fn_pair.second->is_init();    // static functions and constructors are never inherited
                });
                if (!fn_infos.empty())
                    mem_fns.extend(fn_infos);
            }
            if (mem_fns.size() > 1) {
                // Check if they are ambiguous
                const auto &fun_map = mem_fns.get_functions();
                if (fun_map.size() < MAX_FUN_CHECK_SEQ) {
                    // sequential algorithm
                    std::unordered_set<SymbolPath> to_be_removed;
                    for (auto it1 = fun_map.begin(); it1 != fun_map.end(); ++it1) {
                        auto fun1 = cast<scope::Function>(it1->second);
                        for (auto it2 = std::next(it1); it2 != fun_map.end(); ++it2) {
                            auto fun2 = cast<scope::Function>(it2->second);
                            ErrorGroup<AnalyzerError> err_grp;
                            check_funs(fun1, fun2, err_grp);
                            if (err_grp) {
                                if (!fun1->is_abstract() && !fun2->is_abstract()) {
                                    errors.extend(err_grp);
                                    break;
                                }
                                if (!check_fun_exactly_same(fun1, fun2)) {
                                    errors.extend(err_grp);
                                    break;
                                }
                                if (!fun1->is_abstract() || !fun2->is_abstract()) {
                                    // Remove the abstract function if the implementation
                                    // is already provided by another class
                                    auto abstract_fn_path = fun1->is_abstract() ? fun1->get_path() : fun2->get_path();
                                    to_be_removed.insert(abstract_fn_path);
                                }
                            }
                        }
                    }
                    // Remove the abstract prototypes of the implemented abstract fns
                    for (const auto &path: to_be_removed) {
                        mem_fns.remove(path);
                    }
                } else {
                    // parallel algorithm
                    using FunOperand = std::pair<scope::Function *, scope::Function *>;

                    std::vector<FunOperand> functions;
                    // Reserve space for the number of combinations
                    // Number of combinations = nC2 = n(n-1)/2
                    // where n is the number of functions in the set
                    functions.reserve(fun_map.size() * (fun_map.size() - 1) / 2);
                    for (auto it1 = fun_map.begin(); it1 != fun_map.end(); ++it1) {
                        auto fun1 = cast<scope::Function>(it1->second);
                        for (auto it2 = std::next(it1); it2 != fun_map.end(); ++it2) {
                            auto fun2 = cast<scope::Function>(it2->second);
                            functions.emplace_back(fun1, fun2);
                        }
                    }

                    std::mutex for_each_mutex;
                    std::for_each(std::execution::par, functions.begin(), functions.end(), [&](const FunOperand &item) {
                        ErrorGroup<AnalyzerError> err_grp;
                        check_funs(item.first, item.second, err_grp);
                        if (err_grp) {
                            if (!item.first->is_abstract() && !item.second->is_abstract()) {
                                std::lock_guard lg(for_each_mutex);
                                errors.extend(err_grp);
                                return;
                            }
                            if (!check_fun_exactly_same(item.first, item.second)) {
                                std::lock_guard lg(for_each_mutex);
                                errors.extend(err_grp);
                                return;
                            }
                            if (!item.first->is_abstract() || !item.second->is_abstract()) {
                                std::lock_guard lg(for_each_mutex);
                                // Remove the abstract function prototype if the implementation
                                // is already provided by another class
                                auto abstract_fn_path = item.first->is_abstract() ? item.first->get_path() : item.second->get_path();
                                mem_fns.remove(abstract_fn_path);
                            }
                        }
                    });
                }
            }
            if (errors) {
                string msg;
                for (const auto &[_, fun_set]: mem_fns.get_function_sets()) {
                    msg += "'" + fun_set->get_enclosing_compound()->to_string() + "', ";
                }
                msg.pop_back();
                msg.pop_back();
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("incompatible super classes {}", msg), LineInfoVector(nodes)))
                        .extend(errors);
            }
            super_functions[member_name].extend(mem_fns);
        }
        // Set the super fields and functions in the class
        klass->set_super_fields(super_fields);
        klass->set_super_functions(super_functions);
    }

    void Analyzer::visit(ast::decl::Compound &node) {
        auto scope = find_scope<scope::Compound>(node.get_name()->get_text());
        if (scope->get_eval() == scope::Compound::Eval::NOT_STARTED) {
            scope->set_eval(scope::Compound::Eval::PROGRESS);

            bool has_super_class = false;
            size_t super_interface_count = 0;
            std::vector<scope::Compound *> supers;

            for (auto parent: node.get_parents()) {
                parent->accept(this);
                auto parent_compound = _res_type_info.basic().type;
                if (parent_compound->is_final())
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("cannot inherit final '{}'", parent_compound->to_string()), parent))
                            .note(error("declared here", parent_compound));
                // check for cyclical inheritance
                switch (parent_compound->get_eval()) {
                case scope::Compound::Eval::NOT_STARTED: {
                    auto old_cur_scope = get_current_scope();
                    cur_scope = parent_compound->get_parent();
                    parent_compound->get_node()->accept(this);
                    cur_scope = old_cur_scope;
                    break;
                }
                case scope::Compound::Eval::PROGRESS:
                    throw ErrorGroup<AnalyzerError>()
                            .error(error("detected cyclical inheritance", scope))
                            .note(error("declared here", parent_compound));
                case scope::Compound::Eval::DONE:
                    break;
                }
                // check for parent combinations
                switch (parent_compound->get_compound_node()->get_token()->get_type()) {
                case TokenType::CLASS:
                    if (has_super_class && node.get_token()->get_type() == TokenType::CLASS)
                        throw error(std::format("'{}' can inherit only one class but got another one", scope->to_string()), parent);
                    if (parent_compound == &*scope)
                        throw error("cannot inherit the class itself", parent);
                    has_super_class = true;
                    break;
                case TokenType::INTERFACE:
                    super_interface_count++;
                    if (parent_compound == &*scope)
                        throw error("cannot inherit the interface itself", parent);
                    break;
                case TokenType::ENUM:
                    throw error("enums cannot be inherited", parent);
                case TokenType::ANNOTATION:
                    throw error("annoations cannot be inherited", parent);
                default:
                    throw Unreachable();    // surely some parser error
                }
                supers.push_back(parent_compound);
            }
            switch (node.get_token()->get_type()) {
            case TokenType::CLASS:
                if (!has_super_class)
                    if (auto super = get_internal<scope::Compound>(Internal::SPADE_ANY); super != &*scope)
                        supers.push_back(super);
                break;
            case TokenType::INTERFACE:
                if (has_super_class)
                    throw error("interfaces cannot inherit from a class", scope);
                if (super_interface_count > 1)
                    throw error("interfaces cannot inherit from more than 1 interface", scope);
                break;
            case TokenType::ENUM:
                if (has_super_class)
                    throw error("enums cannot inherit from a class", scope);
                supers.push_back(get_internal<scope::Compound>(Internal::SPADE_ENUM));
                break;
            case TokenType::ANNOTATION:
                if (has_super_class)
                    throw error("annotations cannot inherit from a class", scope);
                supers.push_back(get_internal<scope::Compound>(Internal::SPADE_ANNOTATION));
                break;
            default:
                throw Unreachable();    // surely some parser error
            }
            // check for compatibility
            if (!supers.empty())
                check_compatible_supers(scope, supers, node.get_parents());
            // perform inheritance
            for (const auto &super: supers) scope->inherit_from(super);

            // visit the members
            for (auto member: node.get_members()) member->accept(this);
            // check for undeclared abstract functions if the compound is not abstract or interface
            // NOTE: interfaces are abstract by default
            if (!scope->is_abstract()) {
                ErrorGroup<AnalyzerError> errors;
                for (const auto &[_, fun_infos]: scope->get_super_functions()) {
                    for (const auto &[_, fun]: fun_infos.get_functions()) {
                        if (fun->is_abstract())
                            errors.error(error(std::format("'{}' is not implemented", fun->to_string()), scope)).note(error("declared here", fun));
                    }
                }
                if (errors)
                    throw errors;
            }
            scope->set_eval(scope::Compound::Eval::DONE);
        }
        end_scope();
    }

    void Analyzer::visit(ast::Import &node) {
        auto module = get_current_module();
        bool open_import = false;
        const auto &elements = node.get_elements();

        std::vector<fs::path> prior_paths;
        prior_paths.push_back(module->get_module_node()->get_file_path().parent_path());
        if (elements[0] != "." && elements[0] != "..")
            for (const auto &path: compiler_options.import_search_dirs) prior_paths.push_back(path);

        std::vector<string> remaining_elms;
        fs::path mod_path;
        for (auto path: prior_paths) {
            remaining_elms = elements;
            std::ranges::reverse(remaining_elms);
            for (const auto &element: elements) {
                if (element == "*") {
                    open_import = true;
                    remaining_elms.pop_back();
                    break;
                }
                path /= element;
                if (fs::exists(path))
                    mod_path = path;
                else {
                    auto extended_path = path.concat(".sp");
                    if (fs::exists(extended_path))
                        mod_path = extended_path;
                    remaining_elms.pop_back();
                    break;
                }
                remaining_elms.pop_back();
            }
            if (!mod_path.empty())
                break;
        }
        if (mod_path.empty())
            throw error("cannot resolve import", &node);

        std::shared_ptr<scope::Scope> result;
        if (fs::is_regular_file(mod_path)) {
            if (mod_path.extension() != ".sp")
                throw error(std::format("dependency is not a spade source file: '{}'", mod_path.generic_string()), &node);
            result = resolve_file(mod_path);
        }
        if (fs::is_directory(mod_path))
            result = resolve_directory(mod_path);
        if (!result)
            throw error("cannot resolve import", &node);

        std::ranges::reverse(remaining_elms);
        for (const auto &element: remaining_elms) {
            if (element == "*")
                break;
            if (!result->has_variable(element)) {
                SymbolPath sym_path;
                for (const auto &element: elements) {
                    if (element == remaining_elms[0])
                        break;
                    sym_path /= element;
                }
                result->set_path(sym_path);
                throw error(std::format("'{}' has no member named '{}'", result->to_string(), element), &node);
            }
            result = result->get_variable(element);
        }

        if (open_import) {
            module->new_open_import(result, node);
        } else {
            string name = node.get_alias() ? node.get_alias()->get_text()                               //
                                           : (elements.back() == "*" ? elements[elements.size() - 2]    //
                                                                     : elements.back());
            module->new_import(name, result, node);
        }
    }

    void Analyzer::visit(ast::Module &node) {
        if (!basic_mode) {
            if (get_current_scope())
                find_scope<scope::Module>(node.get_name());
            else
                cur_scope = &*module_scopes.at(node.get_file_path());
        }
        for (auto member: node.get_members()) member->accept(this);
        end_scope();
    }
}    // namespace spade