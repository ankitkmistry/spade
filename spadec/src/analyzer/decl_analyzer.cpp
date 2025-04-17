#include "analyzer.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
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
        // TODO: check for function level declarations
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
            // resolve_assign automatically sets eval to DONE
            resolve_assign(node.get_type(), node.get_expr(), node);
        }
        end_scope();
    }

    void Analyzer::visit(ast::decl::Parent &node) {
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
            parent_type_info.type_args.reserve(node.get_type_args().size());
            for (auto type_arg: node.get_type_args()) {
                type_arg->accept(this);
                parent_type_info.type_args.push_back(_res_type_info);
            }
        }
        _res_type_info.reset();
        _res_type_info = parent_type_info;
    }

    void Analyzer::visit(ast::decl::Enumerator &node) {}

    void Analyzer::visit(ast::decl::Compound &node) {
        auto scope = find_scope<scope::Compound>(node.get_name()->get_text());
        // TODO: Solve this ambiguity
        //
        // class A : B {}
        // class B : C {}
        // class C : A {}
        //
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
        if (get_current_scope())
            find_scope<scope::Module>(node.get_name());
        else {
            cur_scope = &*module_scopes.at(&node).get_scope();
        }

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