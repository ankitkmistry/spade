#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "spimp/error.hpp"
#include "utils/error.hpp"

namespace spade
{
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
            if (_res_param_info.b_default)
                throw error("positional only parameter cannot have default value", param);
            if (_res_param_info.b_variadic)
                throw error("positional only parameter cannot be variadic", param);
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
        fun->set_pos_kwd_params(pos_kwd_params);

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
        fun->set_kwd_only_params(kwd_only_params);

        if (found_variadic && node.get_kwd_only().back() != found_variadic)
            throw ErrorGroup<AnalyzerError>().error(error("variadic parameter must be the last parameter", found_variadic));
    }

    void Analyzer::visit(ast::decl::Function &node) {
        if (auto fun_level = get_current_scope()->get_enclosing_function();
            get_current_scope()->get_type() == scope::ScopeType::FUNCTION || fun_level) {
            // TODO: check for function level declarations
            return;
        }

        std::shared_ptr<scope::FunctionSet> fun_set = find_scope<scope::FunctionSet>(node.get_name()->get_text());
        std::shared_ptr<scope::Function> scope = find_scope<scope::Function>(node.get_qualified_name());

        if (mode == Mode::DECLARATION) {
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
                function_scopes.push_back(scope);
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
                check_fun_set(fun_set);
                cur_scope = old_cur_scope;    // restore context
            }

            auto definition = node.get_definition();

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
            if (auto definition = node.get_definition())
                definition->accept(this);

        end_scope();    // pop the function
        end_scope();    // pop the function set
    }

    void Analyzer::visit(ast::decl::Variable &node) {
        std::shared_ptr<scope::Variable> scope;
        if (get_current_scope()->get_type() == scope::ScopeType::FUNCTION || get_current_scope()->get_enclosing_function()) {
            scope = declare_variable(node.get_name());
        } else
            scope = find_scope<scope::Variable>(node.get_name()->get_text());

        if (scope->get_eval() == scope::Variable::Eval::NOT_STARTED) {
            scope->set_eval(scope::Variable::Eval::PROGRESS);
            // resolve_assign automatically sets eval to DONE
            resolve_assign(node.get_type(), node.get_expr(), node);
        }
        end_scope();
    }

    void Analyzer::visit(ast::decl::Parent &node) {
        node.get_reference()->accept(this);
        // Check if the super class is a scope::Compound
        if (_res_expr_info.tag != ExprInfo::Type::STATIC)
            throw error("reference is not a type", &node);
        // Get the parent type info and type args if any
        TypeInfo parent_type_info = _res_expr_info.type_info;
        if (!node.get_type_args().empty()) {
            parent_type_info.type_args.reserve(node.get_type_args().size());
            for (auto type_arg: node.get_type_args()) {
                type_arg->accept(this);
                parent_type_info.type_args.push_back(_res_type_info);
            }
        }
        _res_type_info.reset();
        _res_type_info = parent_type_info;
    }

    void Analyzer::visit(ast::decl::Enumerator &node) {
        auto scope = find_scope<scope::Enumerator>(node.get_name()->get_text());
        auto parent_enum = scope->get_enclosing_compound();
        if (node.get_expr()) {
            if (parent_enum->has_variable("init"))
                throw error(std::format("enumerator cannot have an initializer due to '{}'",
                                        parent_enum->get_variable("init")->to_string()),
                            node.get_expr());
        } else if (node.get_args()) {
            auto args = *node.get_args();
            if (!parent_enum->has_variable("init"))
                throw error("enumerator cannot be called with ctor, no declaration provided", node.get_expr());
            FunctionInfo fn_infos(&*cast<scope::FunctionSet>(parent_enum->get_variable("init")));
            // Build args
            std::vector<ArgInfo> arg_infos;
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

    static bool check_fun_exactly_same(const scope::Function *fun1, const scope::Function *fun2) {
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
        for (const auto &param1: fun1->get_pos_only_params()) {
            for (const auto &param2: fun2->get_pos_only_params()) {
                if (param1.b_const != param2.b_const)
                    return false;
                if (param1.type_info != param2.type_info)
                    return false;
            }
        }
        for (const auto &param1: fun1->get_pos_kwd_params()) {
            for (const auto &param2: fun2->get_pos_kwd_params()) {
                if (param1.b_const != param2.b_const || param1.b_variadic != param2.b_variadic ||
                    param1.b_default != param2.b_default || param1.name != param2.name || param1.type_info != param2.type_info)
                    return false;
            }
        }
        for (const auto &param1: fun1->get_kwd_only_params()) {
            for (const auto &param2: fun2->get_kwd_only_params()) {
                if (param1.b_const != param2.b_const || param1.b_variadic != param2.b_variadic ||
                    param1.b_default != param2.b_default || param1.name != param2.name || param1.type_info != param2.type_info)
                    return false;
            }
        }
        return true;
    }

    void Analyzer::check_compatible_supers(const std::shared_ptr<scope::Compound> &klass,
                                           const std::vector<scope::Compound *> &supers,
                                           const std::vector<std::shared_ptr<ast::decl::Parent>> &nodes) const {
        std::unordered_map<string, std::shared_ptr<scope::Variable>> super_fields;
        std::unordered_map<string, FunctionInfo> super_functions;
        // member_table : map[string => vector[Scope]]
        //                  where key(string) is the name of the member
        //                        value(vector[Scope]) is the list of matching scopes
        std::unordered_map<string, std::vector<scope::Scope *>> member_table;

        for (auto super: supers) {
            for (const auto &[member_name, member]: super->get_members()) {
                const auto &[_, member_scope] = member;
                switch (member_scope->get_type()) {
                    case scope::ScopeType::FOLDER_MODULE:
                    case scope::ScopeType::MODULE:
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
                        if (auto var_scope = cast<scope::Variable>(member_scope); !var_scope->is_static())
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
            if (members.size() <= 1)
                continue;
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
                    return fn_pair.second->is_static();    // static functions are never inherited
                });
                if (!fn_infos.empty())
                    mem_fns.extend(fn_infos);
            }
            if (mem_fns.size() <= 1)
                continue;
            // Check if they are ambiguous
            const auto &fun_map = mem_fns.get_functions();
            for (auto it1 = fun_map.begin(); it1 != fun_map.end(); ++it1) {
                auto fun1 = cast<scope::Function>(it1->second);
                for (auto it2 = std::next(it1); it2 != fun_map.end(); ++it2) {
                    auto fun2 = cast<scope::Function>(it2->second);
                    ErrorGroup<AnalyzerError> err_grp;
                    check_funs(fun1, fun2, err_grp);
                    if (!err_grp.get_errors().empty()) {
                        if (!fun1->is_abstract() && !fun2->is_abstract())
                            errors.extend(err_grp);
                        else if (!check_fun_exactly_same(fun1, fun2))
                            errors.extend(err_grp);
                    }
                }
            }
            if (!errors.get_errors().empty()) {
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
            super_functions[member_name] = mem_fns;
        }
        // Set the super fields and functions in the class
        klass->set_super_fields(super_fields);
        klass->set_super_functions(super_functions);
    }

    void Analyzer::visit(ast::decl::Compound &node) {
        auto scope = find_scope<scope::Compound>(node.get_name()->get_text());
        if (scope->get_eval() == scope::Compound::Eval::NOT_STARTED) {
            scope->set_eval(scope::Compound::Eval::PROGRESS);
            if (node.get_parents().empty()) {
                std::shared_ptr<scope::Compound> default_super;
                switch (node.get_token()->get_type()) {
                    case TokenType::CLASS:
                        default_super = cast<scope::Compound>(internals[Internal::SPADE_ANY]);
                        break;
                    case TokenType::INTERFACE:
                        break;
                    case TokenType::ENUM:
                        default_super = cast<scope::Compound>(internals[Internal::SPADE_ENUM]);
                        break;
                    case TokenType::ANNOTATION:
                        default_super = cast<scope::Compound>(internals[Internal::SPADE_ANNOTATION]);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
                if (default_super)
                    scope->inherit_from(&*default_super);
            } else {
                bool has_super_class = false;
                size_t super_interface_count = 0;
                std::vector<scope::Compound *> supers;
                supers.reserve(node.get_parents().size());

                for (auto parent: node.get_parents()) {
                    parent->accept(this);
                    auto parent_compound = _res_type_info.type;
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
                            if (has_super_class && node.get_token()->get_type() == TokenType::CLASS) {
                                throw error(
                                        std::format("'{}' can inherit only one class but got another one", scope->to_string()),
                                        parent);
                            }
                            has_super_class = true;
                            break;
                        case TokenType::INTERFACE:
                            super_interface_count++;
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
                        break;
                    case TokenType::ANNOTATION:
                        if (has_super_class)
                            throw error("annotations cannot inherit from a class", scope);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
                // check for compatibility
                check_compatible_supers(scope, supers, node.get_parents());
                // perform inheritance
                for (const auto &super: supers) scope->inherit_from(super);
            }
            for (auto member: node.get_members()) member->accept(this);
            scope->set_eval(scope::Compound::Eval::DONE);
        }
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
        if (get_current_scope())
            find_scope<scope::Module>(node.get_name());
        else
            cur_scope = &*module_scopes.at(&node).get_scope();

        for (auto import: node.get_imports()) import->accept(this);
        for (auto member: node.get_members()) member->accept(this);

        end_scope();
    }

    void Analyzer::visit(ast::FolderModule &node) {
        std::shared_ptr<scope::Scope> scope;
        if (get_current_scope())
            scope = find_scope<scope::Module>(node.get_name());
        else {
            scope = module_scopes.at(&node).get_scope();
            cur_scope = &*scope;
        }

        for (auto [name, member]: scope->get_members()) {
            auto [_, scope] = member;
            scope->get_node()->accept(this);
        }

        end_scope();
    }
}    // namespace spade