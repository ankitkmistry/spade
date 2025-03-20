#include <numeric>

#include "scope_tree.hpp"
#include "parser/ast.hpp"

#define get_parent_scope() (scope_stack.at(scope_stack.size() - 2))

namespace spade
{
    static string deparenthesize(const string &str) {
        return str.substr(0, str.find_first_of('('));
    }

    SymbolPath ScopeTreeBuilder::get_current_path() const {
        auto scope_sptr = scope_stack.at(0);
        SymbolPath path(cast<ast::Module>(scope_sptr->get_node())->get_name());
        for (size_t i = 1; i < scope_stack.size(); i++) {
            for (const auto &[member_name, member]: scope_sptr->get_members()) {
                auto [_, scope] = member;
                if (scope == scope_stack[i]) {
                    path /= member_name;
                    break;
                }
            }
            scope_sptr = scope_stack[i];
        }
        return path;
    }

    void ScopeTreeBuilder::add_symbol(const string &name, const std::shared_ptr<Token> &decl_site,
                                      std::shared_ptr<scope::Scope> scope) {
        if (scope_stack.size() == 1) {
            if (scope->get_type() == scope::ScopeType::FOLDER_MODULE) {
                if (module_scopes.contains(cast<ast::FolderModule>(scope->get_node()))) {
                    auto module_scope = module_scopes.at(cast<ast::FolderModule>(scope->get_node())).get_scope();
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR, error(std::format("redeclaration of '{}'", name), scope)),
                            std::pair(ErrorType::NOTE, error("already declared here", module_scope)));
                } else {
                    scope->set_path(get_current_path().to_string());
                    module_scopes.emplace(cast<ast::FolderModule>(scope->get_node()), ScopeInfo(scope));
                }
            } else if (scope->get_type() == scope::ScopeType::MODULE) {
                if (module_scopes.contains(cast<ast::Module>(scope->get_node()))) {
                    auto module_scope = module_scopes.at(cast<ast::Module>(scope->get_node())).get_scope();
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR, error(std::format("redeclaration of '{}'", name), scope)),
                            std::pair(ErrorType::NOTE, error("already declared here", module_scope)));
                } else {
                    scope->set_path(get_current_path().to_string());
                    module_scopes.emplace(cast<ast::Module>(scope->get_node()), ScopeInfo(scope));
                }
            } else
                throw Unreachable();    // surely some parser error

            LOGGER.log_info(std::format("added symbol '{}'", get_current_path().to_string()));
            return;
        }

        auto node = scope->get_node();
        auto parent_scope = (scope_stack.at(scope_stack.size() - 2));
        auto symbol_path = (get_current_path() / name).to_string();
        if (parent_scope->has_variable(name))
            throw ErrorGroup<AnalyzerError>(
                    std::pair(ErrorType::ERROR, error(std::format("redeclaration of '{}'", symbol_path), node)),
                    std::pair(ErrorType::NOTE, error("already declared here", parent_scope->get_variable(name))));
        if (scope->get_type() != scope::ScopeType::FUNCTION) {
            for (const auto &[member_name, member]: parent_scope->get_members()) {
                auto member_scope = member.second;
                if (member_scope->get_type() == scope::ScopeType::FUNCTION && deparenthesize(member_name) == name) {
                    throw ErrorGroup<AnalyzerError>(
                            std::pair(ErrorType::ERROR, error(std::format("redeclaration of '{}'", symbol_path), node)),
                            std::pair(ErrorType::NOTE, error("already declared here", scope)));
                }
            }
        }
        parent_scope->new_variable(name, decl_site, scope);
        scope->set_path(symbol_path);
        LOGGER.log_info(std::format("added symbol '{}'", symbol_path));
    }

    void ScopeTreeBuilder::check_modifiers(ast::AstNode *node, const std::vector<std::shared_ptr<Token>> &modifiers) {
        std::unordered_map<TokenType, size_t> modifier_counts = {
                {TokenType::ABSTRACT,  0},
                {TokenType::FINAL,     0},
                {TokenType::STATIC,    0},
                {TokenType::OVERRIDE,  0},
                {TokenType::PRIVATE,   0},
                {TokenType::INTERNAL,  0},
                {TokenType::PROTECTED, 0},
                {TokenType::PUBLIC,    0},
        };
        // Get the count of each modifier
        for (const auto &modifier: modifiers) {
            try {
                modifier_counts.at(modifier->get_type())++;
            } catch (const std::out_of_range &) {
                throw Unreachable();    // surely some parser error
            }
        }
        // Check for duplicate modifiers
        for (const auto &[modifier, count]: modifier_counts) {
            if (count > 1) {
                throw error(std::format("duplicate modifier: {}", TokenInfo::get_repr(modifier)), node);
            }
        }

#define CHECK_EXCLUSIVE(a, b)                                                                                                  \
    do {                                                                                                                       \
        if (modifier_counts[a] + modifier_counts[b] > 1)                                                                       \
            throw error(std::format("{} and {} are mutually exclusive", TokenInfo::get_repr(a), TokenInfo::get_repr(b)),       \
                        node);                                                                                                 \
    } while (false)

        CHECK_EXCLUSIVE(TokenType::ABSTRACT, TokenType::FINAL);
        CHECK_EXCLUSIVE(TokenType::STATIC, TokenType::OVERRIDE);
        CHECK_EXCLUSIVE(TokenType::ABSTRACT, TokenType::PRIVATE);
        CHECK_EXCLUSIVE(TokenType::FINAL, TokenType::PRIVATE);
        CHECK_EXCLUSIVE(TokenType::OVERRIDE, TokenType::PRIVATE);

#undef CHECK_EXCLUSIVE

        if (modifier_counts[TokenType::PRIVATE] + modifier_counts[TokenType::PROTECTED] + modifier_counts[TokenType::INTERNAL] +
                    modifier_counts[TokenType::PUBLIC] >
            1) {
            throw error("access modifiers are mutually exclusive", node);
        }

        if (modifier_counts[TokenType::ABSTRACT] + modifier_counts[TokenType::STATIC] > 1) {
            try {
                if (cast<ast::decl::Compound>(node)->get_token()->get_type() == TokenType::CLASS &&
                    cast<ast::decl::Compound>(scope_stack.back()->get_node())->get_token()->get_type() == TokenType::CLASS) {
                } else
                    throw error("'abstract' and 'static' are mutually exclusive", node);
            } catch (const CastError &) {
                throw error("'abstract' and 'static' are mutually exclusive", node);
            } catch (const std::out_of_range &) {
                throw Unreachable();    // not a folder module expected a file module
            }
        }

        // Module level declaration specific checks
        try {
            if (scope_stack.back()->get_type() == scope::ScopeType::MODULE) {
                if (modifier_counts[TokenType::STATIC] > 0)
                    throw error("module level declarations cannot be 'static'", node);
                if (modifier_counts[TokenType::OVERRIDE] > 0)
                    throw error("module level declarations cannot be 'override'", node);

                if (is<ast::decl::Function>(node)) {
                    if (modifier_counts[TokenType::ABSTRACT] > 0)
                        throw error("global functions cannot be 'abstract'", node);
                }
            }
        } catch (const std::out_of_range &) {
            throw Unreachable();    // not a folder module expected a file module
        }
        // Variable specific checks
        if (is<ast::decl::Variable>(node)) {
            if (modifier_counts[TokenType::ABSTRACT] > 0)
                throw error("variables/constants cannot be 'abstract'", node);
            if (modifier_counts[TokenType::FINAL] > 0)
                throw error("variables/constants cannot be 'final'", node);
            if (modifier_counts[TokenType::OVERRIDE] > 0)
                throw error("variables/constants cannot be 'override'", node);
        }
        // Compound declaration specific checks
        try {
            if (auto compound = dynamic_cast<ast::decl::Compound *>(node)) {
                switch (compound->get_token()->get_type()) {
                    case TokenType::CLASS:
                        if (modifier_counts[TokenType::OVERRIDE] > 0)
                            throw error("classes cannot be 'override'", node);
                        break;
                    case TokenType::ENUM:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw error("enums cannot be 'abstract'", node);
                        if (modifier_counts[TokenType::OVERRIDE] > 0)
                            throw error("enums cannot be 'override'", node);
                        break;
                    case TokenType::INTERFACE:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw error("interfaces cannot be 'abstract'", node);
                        if (modifier_counts[TokenType::OVERRIDE] > 0)
                            throw error("interfaces cannot be 'override'", node);
                        if (modifier_counts[TokenType::FINAL] > 0)
                            throw error("interfaces cannot be 'final'", node);
                        break;
                    case TokenType::ANNOTATION:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw error("annotations cannot be 'abstract'", node);
                        if (modifier_counts[TokenType::OVERRIDE] > 0)
                            throw error("annotations cannot be 'override'", node);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
        } catch (const std::out_of_range &) {
            throw Unreachable();    // not a folder module expected a file module
        }
        // Parent class/compound specific checks
        try {
            if (auto compound = dynamic_cast<ast::decl::Compound *>(scope_stack.back()->get_node())) {
                switch (compound->get_token()->get_type()) {
                    case TokenType::CLASS:
                        break;
                    case TokenType::ENUM:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw error("'abstract' members are not allowed in enums", node);
                        if (modifier_counts[TokenType::FINAL] > 0)
                            throw error("'final' members are not allowed in enums", node);
                        break;
                    case TokenType::INTERFACE:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw error("'abstract' members are not allowed in interfaces", node);
                        if (modifier_counts[TokenType::OVERRIDE] > 0)
                            throw error("'override' members are not allowed in interfaces", node);
                        if (modifier_counts[TokenType::FINAL] > 0 && modifier_counts[TokenType::STATIC] == 0)
                            throw error("'final' members are not allowed in interfaces (but final static is allowed)", node);
                        break;
                    case TokenType::ANNOTATION:
                        if (modifier_counts[TokenType::ABSTRACT] > 0)
                            throw error("'abstract' members are not allowed in annotations", node);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
        } catch (const std::out_of_range &) {
            throw Unreachable();    // not a folder module expected a file module
        }
        // Constructor specific checks
        if (is<ast::decl::Init>(node)) {
            if (modifier_counts[TokenType::ABSTRACT] > 0)
                throw error("constructor cannot be 'abstract'", node);
            if (modifier_counts[TokenType::STATIC] > 0)
                throw error("constructor cannot be 'static'", node);
            if (modifier_counts[TokenType::STATIC] > 0)
                throw error("constructor cannot be 'final'", node);
            if (modifier_counts[TokenType::OVERRIDE] > 0)
                throw error("constructor cannot be 'override'", node);
        }
    }

    static string build_type_params_string(const std::vector<std::shared_ptr<ast::decl::TypeParam>> &type_params) {
        return std::accumulate(type_params.begin(), type_params.end(), string(),
                               [](const string &a, const std::shared_ptr<ast::decl::TypeParam> &b) {
                                   return a + (a.empty() ? "" : ",") + b->get_name()->get_text();
                               });
    }

    string ScopeTreeBuilder::build_params_string(const std::shared_ptr<ast::decl::Params> &params) {
        if (!params)
            return "";
        string param_string;
        param_string = std::accumulate(params->get_pos_only().begin(), params->get_pos_only().end(), param_string,
                                       [&](const string &a, const std::shared_ptr<ast::decl::Param> &b) {
                                           b->get_type()->accept(this);
                                           return a + (a.empty() ? "" : ",") + _res_type_signature;
                                       });
        param_string = std::accumulate(params->get_pos_kwd().begin(), params->get_pos_kwd().end(), param_string,
                                       [&](const string &a, const std::shared_ptr<ast::decl::Param> &b) {
                                           b->get_type()->accept(this);
                                           return a + (a.empty() ? "" : ",") + _res_type_signature;
                                       });
        param_string = std::accumulate(params->get_kwd_only().begin(), params->get_kwd_only().end(), param_string,
                                       [&](const string &a, const std::shared_ptr<ast::decl::Param> &b) {
                                           b->get_type()->accept(this);
                                           return a + (a.empty() ? "" : ",") + _res_type_signature;
                                       });
        return param_string;
    }

    string ScopeTreeBuilder::build_function_name(const ast::decl::Function &node) {
        std::stringstream ss;
        ss << node.get_name()->get_text();
        if (auto type_params = node.get_type_params(); !type_params.empty()) {
            ss << '[';
            ss << build_type_params_string(node.get_type_params());
            ss << ']';
        }
        auto params = node.get_params();
        ss << '(';
        ss << build_params_string(params);
        ss << ')';
        return ss.str();
    }

    string ScopeTreeBuilder::build_init_name(const ast::decl::Init &node) {
        return "init(" + build_params_string(node.get_params()) + ")";
    }

    ScopeTreeBuilder::ScopeTreeBuilder(const std::vector<std::shared_ptr<ast::Module>> &modules) : modules(modules) {
        if (!modules.empty()) {
            root_path = modules[0]->get_file_path().parent_path();
            for (const auto &module: modules) {
                fs::path path = module->get_file_path();
                fs::path current_path = path.parent_path();
                fs::path temp_path;

                auto common_it = root_path.begin();
                auto current_it = current_path.begin();

                for (; common_it != root_path.end() && current_it != current_path.end() && *common_it == *current_it;
                     ++common_it, ++current_it) {
                    temp_path /= *common_it;
                }
                root_path = temp_path;
            }
        }
    }

    void ScopeTreeBuilder::visit(ast::Reference &node) {
        // _res_type_signature = "";
        _res_type_signature = std::accumulate(
                node.get_path().begin(), node.get_path().end(), string(),
                [](const string &a, const std::shared_ptr<Token> &b) { return a + (a.empty() ? "" : ".") + b->get_text(); });
    }

    void ScopeTreeBuilder::visit(ast::type::Reference &node) {
        // _res_type_signature = "";
        node.get_reference()->accept(this);
        string result = _res_type_signature;
        if (!node.get_type_args().empty()) {
            result += "[";
            result += std::accumulate(node.get_type_args().begin(), node.get_type_args().end(), string(),
                                      [&](const string &a, const std::shared_ptr<ast::Type> &b) {
                                          b->accept(this);
                                          return a + (a.empty() ? "" : ",") + _res_type_signature;
                                      });
            result += "]";
        }
        _res_type_signature = result;
    }

    void ScopeTreeBuilder::visit(ast::type::Function &node) {
        _res_type_signature = "";
        string result = "(";
        result += std::accumulate(node.get_param_types().begin(), node.get_param_types().end(), string(),
                                  [&](const string &a, const std::shared_ptr<ast::Type> &b) {
                                      b->accept(this);
                                      return a + (a.empty() ? "" : ",") + _res_type_signature;
                                  });
        result += ")->";
        node.get_return_type()->accept(this);
        result += _res_type_signature;
        _res_type_signature = result;
    }

    void ScopeTreeBuilder::visit(ast::type::TypeLiteral &node) {
        _res_type_signature = "type";
    }

    void ScopeTreeBuilder::visit(ast::type::BinaryOp &node) {
        _res_type_signature = "";
        node.get_left()->accept(this);
        string result = _res_type_signature;
        result += node.get_op()->get_text();
        node.get_right()->accept(this);
        result += _res_type_signature;
        _res_type_signature = result;
    }

    void ScopeTreeBuilder::visit(ast::type::Nullable &node) {
        _res_type_signature = "";
        node.get_type()->accept(this);
        _res_type_signature += "?";
    }

    void ScopeTreeBuilder::visit(ast::type::TypeBuilder &node) {
        _res_type_signature = "";
        string result = "object{";
        result += std::accumulate(node.get_members().begin(), node.get_members().end(), string(),
                                  [&](const string &a, const std::shared_ptr<ast::type::TypeBuilderMember> &b) {
                                      b->accept(this);
                                      return a + (a.empty() ? "" : ",") + _res_type_signature;
                                  });
        result += "}";
        _res_type_signature = result;
    }

    void ScopeTreeBuilder::visit(ast::type::TypeBuilderMember &node) {
        _res_type_signature = "";
        string result = node.get_name()->get_text() + ":";
        node.get_type()->accept(this);
        _res_type_signature = result + _res_type_signature;
    }

    void ScopeTreeBuilder::visit(ast::expr::Constant &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Super &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Self &node) {}

    void ScopeTreeBuilder::visit(ast::expr::DotAccess &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Call &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Argument &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Reify &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Index &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Slice &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Unary &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Cast &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Binary &node) {}

    void ScopeTreeBuilder::visit(ast::expr::ChainBinary &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Ternary &node) {}

    void ScopeTreeBuilder::visit(ast::expr::Assignment &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Block &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::If &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::While &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::DoWhile &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Throw &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Catch &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Try &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Continue &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Break &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Return &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Yield &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Expr &node) {}

    void ScopeTreeBuilder::visit(ast::stmt::Declaration &node) {}

    void ScopeTreeBuilder::visit(ast::decl::TypeParam &node) {}

    void ScopeTreeBuilder::visit(ast::decl::Constraint &node) {}

    void ScopeTreeBuilder::visit(ast::decl::Param &node) {}

    void ScopeTreeBuilder::visit(ast::decl::Params &node) {}

    void ScopeTreeBuilder::visit(ast::decl::Function &node) {
        check_modifiers(&node, node.get_modifiers());
        if (scope_stack.empty()) {
            throw Unreachable();    // surely some parser error
        }

        auto scope = begin_scope<scope::Function>(node);
        auto qualified_name = build_function_name(node);
        node.set_qualified_name(qualified_name);
        add_symbol(qualified_name, node.get_name(), scope);
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::decl::Variable &node) {
        check_modifiers(&node, node.get_modifiers());
        if (!scope_stack.empty()) {
            // constants and static variables are allowed in interfaces
            if (scope_stack.back()->get_type() == scope::ScopeType::COMPOUND &&
                cast<scope::Compound>(scope_stack.back())->get_compound_node()->get_token()->get_type() ==
                        TokenType::INTERFACE) {
                if (node.get_token()->get_type() != TokenType::CONST &&
                    std::find_if(node.get_modifiers().begin(), node.get_modifiers().end(),
                                 [](const std::shared_ptr<Token> &modifier) {
                                     return modifier->get_type() == TokenType::STATIC;
                                 }) == node.get_modifiers().end())
                    throw error("fields are not allowed in interfaces (static and const fields are allowed)", &node);
            }
        } else
            throw Unreachable();    // surely some parser error

        auto scope = begin_scope<scope::Variable>(node);
        add_symbol(node.get_name()->get_text(), node.get_name(), scope);
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::decl::Init &node) {
        check_modifiers(&node, node.get_modifiers());
        if (!scope_stack.empty()) {
            if (scope_stack.back()->get_type() == scope::ScopeType::COMPOUND &&
                cast<scope::Compound>(scope_stack.back())->get_compound_node()->get_token()->get_type() != TokenType::INTERFACE)
                ;
            else
                throw error("constructors are not allowed in interfaces", &node);
        } else {
            throw Unreachable();    // surely some parser error
        }

        auto scope = begin_scope<scope::Init>(node);
        auto qualified_name = build_init_name(node);
        node.set_qualified_name(qualified_name);
        add_symbol(qualified_name, node.get_name(), scope);
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::decl::Parent &node) {}

    void ScopeTreeBuilder::visit(ast::decl::Enumerator &node) {
        if (!scope_stack.empty()) {
            if (scope_stack.back()->get_type() == scope::ScopeType::COMPOUND &&
                cast<scope::Compound>(scope_stack.back())->get_compound_node()->get_token()->get_type() == TokenType::ENUM)
                ;
            else
                throw error("enumerators are allowed in enums only", &node);
        } else {
            throw Unreachable();    // surely some parser error
        }

        auto scope = begin_scope<scope::Enumerator>(node);
        add_symbol(node.get_name()->get_text(), node.get_name(), scope);
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::decl::Compound &node) {
        check_modifiers(&node, node.get_modifiers());
        if (!scope_stack.empty()) {
            if (scope_stack.back()->get_type() != scope::ScopeType::MODULE &&
                scope_stack.back()->get_type() != scope::ScopeType::COMPOUND)
                throw Unreachable();    // surely some parser error
            if (scope_stack.back()->get_type() == scope::ScopeType::COMPOUND) {
                switch (cast<scope::Compound>(scope_stack.back())->get_compound_node()->get_token()->get_type()) {
                    case TokenType::CLASS:
                        break;
                    case TokenType::ENUM:
                        if (node.get_token()->get_type() == TokenType::ENUM)
                            throw error("nested enums are not allowed", &node);
                        if (node.get_token()->get_type() == TokenType::ANNOTATION)
                            throw error("annotations are not allowed in enums", &node);
                        break;
                        break;
                    case TokenType::INTERFACE:
                        if (node.get_token()->get_type() == TokenType::CLASS)
                            throw error("classes are not allowed in interfaces", &node);
                        if (node.get_token()->get_type() == TokenType::ENUM)
                            throw error("enums are not allowed in interfaces", &node);
                        if (node.get_token()->get_type() == TokenType::ANNOTATION)
                            throw error("annotations are not allowed in interfaces", &node);
                        break;
                    case TokenType::ANNOTATION:
                        if (node.get_token()->get_type() == TokenType::CLASS)
                            throw error("classes are not allowed in annotations", &node);
                        if (node.get_token()->get_type() == TokenType::ENUM)
                            throw error("enums are not allowed in annotations", &node);
                        if (node.get_token()->get_type() == TokenType::ANNOTATION)
                            throw error("annotations are not allowed in annotations", &node);
                        break;
                    default:
                        throw Unreachable();    // surely some parser error
                }
            }
        } else {
            throw Unreachable();    // surely some parser error
        }

        auto scope = begin_scope<scope::Compound>(node);
        add_symbol(node.get_name()->get_text(), node.get_name(), scope);
        for (auto enumerator: node.get_enumerators()) {
            enumerator->accept(this);
        }
        for (auto member: node.get_members()) {
            member->accept(this);
        }
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::Import &node) {
        if (scope_stack.empty())
            throw Unreachable();    // surely some parser error
        else if (scope_stack.back()->get_type() != scope::ScopeType::MODULE)
            throw Unreachable();    // surely some parser error
    }

    void ScopeTreeBuilder::visit(ast::Module &node) {
        if (!scope_stack.empty()) {
            if (scope_stack.back()->get_type() != scope::ScopeType::FOLDER_MODULE)
                throw Unreachable();    // surely some parser error (modules are allowed in modules only)
        }
        auto scope = begin_scope<scope::Module>(node);
        add_symbol(scope->get_module_node()->get_name(), null, scope);
        if (scope_stack.size() > 1) {
            module_scopes.emplace(&node, ScopeInfo(scope, false));
        }
        for (auto import: node.get_imports()) {
            import->accept(this);
        }
        for (auto member: node.get_members()) {
            member->accept(this);
        }
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::FolderModule &node) {}

    const std::unordered_map<ast::Module *, ScopeInfo> &ScopeTreeBuilder::build() {
        for (auto module: modules) {
            {
                size_t n = 0;
                auto relative_path = module->get_file_path().parent_path().lexically_relative(root_path);
                if (relative_path != ".") {
                    auto path = root_path;
                    for (auto itr = relative_path.begin(); itr != relative_path.end(); ++itr) {
                        path /= *itr;
                        // Shared pointer gets deleted after the loop
                        auto folder_module = new ast::FolderModule(path);
                        auto scope = begin_scope<scope::FolderModule>(*folder_module);
                        add_symbol(folder_module->get_name(), null, scope);
                        n++;
                    }
                }

                // Traverse the module
                module->accept(this);
                // Remove the folders from the scope stack
                for (size_t i = 0; i < n; i++) end_scope();
            }
        }
        return module_scopes;
    }
}    // namespace spade