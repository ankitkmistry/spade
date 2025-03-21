#include "analyzer.hpp"
#include "info.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "scope_tree.hpp"
#include "spimp/error.hpp"
#include "symbol_path.hpp"
#include "utils/error.hpp"

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

    std::shared_ptr<scope::Module> Analyzer::get_current_module() const {
        for (auto itr = scope_stack.rbegin(); itr != scope_stack.rend(); ++itr)
            if (auto scope = *itr; scope->get_type() == scope::ScopeType::MODULE)
                return cast<scope::Module>(scope);
        return null;
    }

    std::shared_ptr<scope::Scope> Analyzer::get_parent_scope() const {
        return scope_stack[scope_stack.size() - 2];
    }

    std::shared_ptr<scope::Scope> Analyzer::get_current_scope() const {
        return scope_stack.back();
    }

    std::shared_ptr<scope::Scope> Analyzer::find_name(const string &name) const {
        std::shared_ptr<scope::Scope> scope;
        for (auto itr = scope_stack.rbegin(); itr != scope_stack.rend(); ++itr) {
            if ((*itr)->has_variable(name)) {
                scope = (*itr)->get_variable(name);
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
            +================================================================================================================+
            |                                               ACCESSORS                                                        |
            +===================+============================================================================================+
            |   private         | same class                                                                                 |
            |   internal        | same class, same module subclass                                                           |
            |   module private  | same class, same module subclass, same module,                                             |
            |   protected       | same class, same module subclass, same module, other module subclass                       |
            |   public          | same class, same module subclass, same module, other module subclass, other non-subclass   |
            +===================+============================================================================================+
        */
        // TODO: implement context resolution

        auto cur_mod = get_current_scope()->get_enclosing_module();
        auto scope_mod = scope->get_enclosing_module();
        if (!cur_mod || !scope_mod)
            throw Unreachable();    // this cannot happen

        std::vector<std::shared_ptr<Token>> modifiers;
        // scope is a variable of scope::Compound
        // scope.get_enclosing_compound() is never null
        switch (scope->get_type()) {
            case scope::ScopeType::COMPOUND:
            case scope::ScopeType::INIT:
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
                        throw ErrorGroup<AnalyzerError>(
                                std::pair(ErrorType::ERROR, error("cannot access 'private' member", &node)),
                                std::pair(ErrorType::NOTE, error("declared here", scope)));
                    return;
                }
                case TokenType::INTERNAL: {
                    // internal here
                    if (cur_mod != scope_mod) {
                        throw ErrorGroup<AnalyzerError>(
                                std::pair(ErrorType::ERROR, error("cannot access 'internal' member", &node)),
                                std::pair(ErrorType::NOTE, error("declared here", scope)));
                    }
                    auto cur_class = get_current_scope()->get_enclosing_compound();
                    auto scope_class = scope->get_enclosing_compound();
                    if (!cur_class || cur_class != scope_class || !cur_class->has_super(scope_class))
                        throw ErrorGroup<AnalyzerError>(
                                std::pair(ErrorType::ERROR, error("cannot access 'internal' member", &node)),
                                std::pair(ErrorType::NOTE, error("declared here", scope)));
                    return;
                }
                case TokenType::PROTECTED: {
                    auto cur_class = get_current_scope()->get_enclosing_compound();
                    auto scope_class = scope->get_enclosing_compound();
                    if (cur_mod != scope_mod && (!cur_class || !cur_class->has_super(scope_class))) {
                        throw ErrorGroup<AnalyzerError>(
                                std::pair(ErrorType::ERROR, error("cannot access 'protected' member", &node)),
                                std::pair(ErrorType::NOTE, error("declared here", scope)));
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
            throw ErrorGroup<AnalyzerError>(std::pair(ErrorType::ERROR, error("cannot access 'module private' member", &node)),
                                            std::pair(ErrorType::NOTE, error("declared here", scope)));
        }
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
        _res_type_info.reset();
        // Find the type scope
        node.get_reference()->accept(this);
        auto type_scope = _res_reference;
        // Check if the reference is a type
        if (type_scope->get_type() != scope::ScopeType::COMPOUND)
            throw ErrorGroup<AnalyzerError>(std::pair(ErrorType::ERROR, error("reference is not a type", &node)),
                                            std::pair(ErrorType::NOTE, error("declared here", type_scope)));
        // Check for type arguments
        std::vector<TypeInfo> type_args;
        for (auto type_arg: node.get_type_args()) {
            type_arg->accept(this);
            type_args.push_back(_res_type_info);
        }
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
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_BOOL]);
                break;
            case TokenType::FALSE:
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_BOOL]);
                break;
            case TokenType::NULL_:
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_ANY]);
                _res_expr_info.type_info.b_nullable = true;
                break;
            case TokenType::INTEGER:
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_INT]);
                break;
            case TokenType::FLOAT:
                _res_expr_info.type_info.type = cast<scope::Compound>(&*internals[Internal::SPADE_FLOAT]);
                break;
            case TokenType::STRING:
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
                        _res_expr_info.module = cast<scope::Module>(scope);
                        _res_expr_info.tag = ExprInfo::Type::MODULE;
                        break;
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*scope);
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        break;
                    case scope::ScopeType::INIT:
                        _res_expr_info.init = cast<scope::Init>(scope);
                        _res_expr_info.tag = ExprInfo::Type::INIT;
                        break;
                    case scope::ScopeType::FUNCTION:
                        _res_expr_info.function = cast<scope::Function>(scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION;
                        break;
                    case scope::ScopeType::BLOCK:
                        throw Unreachable();    // surely some parser error
                    case scope::ScopeType::VARIABLE:
                        _res_expr_info.type_info = cast<scope::Variable>(scope)->get_type_info();
                        _res_expr_info.tag = ExprInfo::Type::NORMAL;
                        break;
                    case scope::ScopeType::ENUMERATOR:
                        // TODO: implement enumerator
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
            (get_current_scope()->get_type() == scope::ScopeType::FUNCTION ||
             get_current_scope()->get_type() == scope::ScopeType::INIT) /* ||
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
             get_current_scope()->get_type() == scope::ScopeType::INIT ||
             get_current_scope()->get_type() == scope::ScopeType::VARIABLE ||
             get_current_scope()->get_type() == scope::ScopeType::ENUMERATOR)) {
            _res_expr_info.type_info.type = cast<scope::Compound>(&*get_parent_scope());
        } else {
            throw error("self is only allowed in class level declarations only", &node);
        }
    }

    void Analyzer::visit(ast::expr::DotAccess &node) {
        _res_expr_info.reset();
        node.get_caller()->accept(this);
        auto caller_info = _res_expr_info;
        switch (_res_expr_info.tag) {
            case ExprInfo::Type::NORMAL: {
                if (caller_info.type_info.b_nullable && !node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR, error("cannot access member of nullable type", &node)),
                            std::pair(ErrorType::NOTE, error("use safe dot access operator '?.'", &node)));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR,
                                      error("cannot use safe dot access operator on non-nullable type", &node)),
                            std::pair(ErrorType::NOTE, error("remove the safe dot access operator '?.'", &node)));
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
                    case scope::ScopeType::INIT:
                        _res_expr_info.init = cast<scope::Init>(member_scope);
                        _res_expr_info.tag = ExprInfo::Type::INIT;
                        break;
                    case scope::ScopeType::FUNCTION:
                        _res_expr_info.function = cast<scope::Function>(member_scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION;
                        break;
                    case scope::ScopeType::VARIABLE:
                        _res_expr_info.type_info = cast<scope::Variable>(member_scope)->get_type_info();
                        _res_expr_info.tag = ExprInfo::Type::NORMAL;
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
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR, error("cannot access member of nullable type", &node)),
                            std::pair(ErrorType::NOTE, error("use safe dot access operator '?.'", &node)));
                }
                if (!caller_info.type_info.b_nullable && node.get_safe()) {
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR,
                                      error("cannot use safe dot access operator on non-nullable type", &node)),
                            std::pair(ErrorType::NOTE, error("remove the safe dot access operator '?.'", &node)));
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
                    case scope::ScopeType::INIT:
                        _res_expr_info.init = cast<scope::Init>(member_scope);
                        _res_expr_info.tag = ExprInfo::Type::INIT;
                        break;
                    case scope::ScopeType::FUNCTION:
                        _res_expr_info.function = cast<scope::Function>(member_scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION;
                        break;
                    case scope::ScopeType::VARIABLE:
                        _res_expr_info.type_info = cast<scope::Variable>(member_scope)->get_type_info();
                        _res_expr_info.tag = ExprInfo::Type::NORMAL;
                        break;
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
                        _res_expr_info.module = cast<scope::Module>(scope);
                        _res_expr_info.tag = ExprInfo::Type::MODULE;
                        break;
                    case scope::ScopeType::COMPOUND:
                        _res_expr_info.type_info.type = cast<scope::Compound>(&*scope);
                        _res_expr_info.tag = ExprInfo::Type::STATIC;
                        break;
                    case scope::ScopeType::FUNCTION:
                        _res_expr_info.function = cast<scope::Function>(scope);
                        _res_expr_info.tag = ExprInfo::Type::FUNCTION;
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
            case ExprInfo::Type::INIT:
            case ExprInfo::Type::FUNCTION:
                throw error("cannot access member of function or constructor", &node);
        }
    }

    void Analyzer::visit(ast::expr::Call &node) {
        _res_expr_info.reset();
        node.get_caller()->accept(this);
    }

    void Analyzer::visit(ast::expr::Argument &node) {
        _res_expr_info.reset();
    }

    void Analyzer::visit(ast::expr::Reify &node) {
        _res_expr_info.reset();
        node.get_caller()->accept(this);
    }

    void Analyzer::visit(ast::expr::Index &node) {
        _res_expr_info.reset();
        node.get_caller()->accept(this);
    }

    void Analyzer::visit(ast::expr::Slice &node) {}

    void Analyzer::visit(ast::expr::Unary &node) {}

    void Analyzer::visit(ast::expr::Cast &node) {}

    void Analyzer::visit(ast::expr::Binary &node) {}

    void Analyzer::visit(ast::expr::ChainBinary &node) {}

    void Analyzer::visit(ast::expr::Ternary &node) {}

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

    void Analyzer::visit(ast::decl::Param &node) {}

    void Analyzer::visit(ast::decl::Params &node) {}

    void Analyzer::visit(ast::decl::Function &node) {
        std::shared_ptr<scope::Function> scope;
        if (get_current_scope()->get_type() == scope::ScopeType::FUNCTION) {
            scope = begin_scope<scope::Function>(node);
            // Add the variable to the parent scope
            auto parent_scope = get_parent_scope();
            if (!parent_scope->new_variable(node.get_name(), scope)) {
                auto org_def = scope->get_decl_site(node.get_name()->get_text());
                throw ErrorGroup<AnalyzerError>(
                        std::pair(ErrorType::ERROR,
                                  error(std::format("redeclaration of '{}'", node.get_name()->get_text()), node.get_name())),
                        std::pair(ErrorType::NOTE, error("already declared here", org_def)));
            }
        } else {
            scope = find_scope<scope::Function>(node.get_qualified_name());
        }

        if (auto type = node.get_return_type()) {
            type->accept(this);
        }

        end_scope();
    }

    void Analyzer::visit(ast::decl::Variable &node) {
        std::shared_ptr<scope::Variable> scope;
        if (get_current_scope()->get_type() == scope::ScopeType::FUNCTION) {
            scope = begin_scope<scope::Variable>(node);
            // Add the variable to the parent scope
            auto parent_scope = get_parent_scope();
            if (!parent_scope->new_variable(node.get_name(), scope)) {
                auto org_def = scope->get_decl_site(node.get_name()->get_text());
                throw ErrorGroup<AnalyzerError>(
                        std::pair(ErrorType::ERROR,
                                  error(std::format("redeclaration of '{}'", node.get_name()->get_text()), node.get_name())),
                        std::pair(ErrorType::NOTE, error("already declared here", org_def)));
            };
        } else {
            scope = find_scope<scope::Variable>(node.get_name()->get_text());
        }

        if (auto type = node.get_type()) {
            type->accept(this);
        }

        if (auto expr = node.get_expr()) {
            expr->accept(this);
        }

        end_scope();
    }

    void Analyzer::visit(ast::decl::Init &node) {
        auto scope = find_scope<scope::Function>(node.get_qualified_name());
        end_scope();
    }

    void Analyzer::visit(ast::decl::Parent &node) {}

    void Analyzer::visit(ast::decl::Enumerator &node) {}

    void Analyzer::visit(ast::decl::Compound &node) {
        auto scope = find_scope<scope::Compound>(node.get_name()->get_text());
        for (auto member: node.get_members()) member->accept(this);
        end_scope();
    }

    void Analyzer::visit(ast::Import &node) {
        auto scope = scope_stack.back();
        // Put the alias if present
        auto name = node.get_alias() ? node.get_alias() : node.get_name();
        if (auto module_sptr = node.get_module().lock()) {
            auto value = module_scopes.at(&*module_sptr).get_scope();
            if (!scope->new_variable(name, value)) {
                // Find the original declaration
                auto org_def = scope->get_decl_site(name->get_text());
                throw ErrorGroup<AnalyzerError>(
                        std::pair(ErrorType::ERROR, error(std::format("redeclaration of '{}'", name->get_text()), name)),
                        std::pair(ErrorType::NOTE, error("already declared here", org_def)));
            }
        } else {
            LOGGER.log_error("import statement is not resolved");
        }
    }

    void Analyzer::visit(ast::Module &node) {
        if (scope_stack.empty()) {
            auto scope = module_scopes.at(&node).get_scope();
            scope_stack.push_back(scope);
        } else {
            find_scope<scope::Module>(node.get_name());
        }

        for (auto import: node.get_imports()) import->accept(this);
        for (auto member: node.get_members()) member->accept(this);

        end_scope();
    }

    void Analyzer::visit(ast::FolderModule &node) {
        std::shared_ptr<scope::Scope> scope;
        if (scope_stack.empty()) {
            scope = module_scopes.at(&node).get_scope();
            scope_stack.push_back(scope);
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