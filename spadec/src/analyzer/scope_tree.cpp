#include "scope_tree.hpp"
#include "lexer/token.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"

#define get_parent_scope() (scope_stack.at(scope_stack.size() - 2))

namespace spadec
{
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

    void ScopeTreeBuilder::add_symbol(const string &name, const std::shared_ptr<Token> &decl_site, std::shared_ptr<scope::Scope> scope) {
        auto parent_scope = (scope_stack.at(scope_stack.size() - 2));
        auto symbol_path = get_current_path() / name;

        if (scope->get_type() == scope::ScopeType::FUNCTION) {
            // Compute the function name and symbol path
            auto fun_scope = cast<scope::Function>(scope);
            // Find the function set in the parent scope
            std::shared_ptr<scope::FunctionSet> fun_set;
            if (const auto existing_scope = parent_scope->get_variable(name)) {
                if (existing_scope->get_type() == scope::ScopeType::FUNCTION_SET) {
                    fun_set = cast<scope::FunctionSet>(existing_scope);
                } else    // Something else was defined with the same name
                    throw ErrorGroup<AnalyzerError>()
                            .error(error(std::format("redeclaration of '{}'", symbol_path.to_string()), fun_scope->get_node()))
                            .note(error("already declared here", existing_scope));
            } else {
                // There was no existing functions with the same name
                // So create a function set and add the current function to it
                fun_set = std::make_shared<scope::FunctionSet>();
                fun_set->set_path(symbol_path);                     // set the symbol path of the function set
                parent_scope->new_variable(name, null, fun_set);    // add the function set to the parent scope
            }

            auto fun_name = fun_scope->get_function_node()->get_name()->get_text() + "#" + std::to_string(fun_set->get_members().size());
            auto fun_sym_path = get_current_path() / fun_name;
            fun_scope->set_path(fun_sym_path);                               // set the symbol path of the function
            fun_scope->get_function_node()->set_qualified_name(fun_name);    // set the qualified name of the function
            fun_set->new_variable(fun_name, decl_site, fun_scope);           // add the function to the set
            SPDLOG_DEBUG(std::format("added symbol '{}'", fun_sym_path.to_string()));
        } else {
            if (parent_scope->has_variable(name)) {
                throw ErrorGroup<AnalyzerError>()
                        .error(error(std::format("redeclaration of '{}'", symbol_path.to_string()), scope->get_node()))
                        .note(error("already declared here", parent_scope->get_variable(name)));
            } else {
                parent_scope->new_variable(name, decl_site, scope);
                scope->set_path(symbol_path);
            }
            SPDLOG_DEBUG(std::format("added symbol '{}'", symbol_path.to_string()));
        }
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
        for (const auto &modifier: modifiers)
            // Check for duplicate modifiers
            if (++modifier_counts[modifier->get_type()] > 1)
                throw error(std::format("duplicate modifier: {}", modifier->get_text()), modifier);

#define CHECK_EXCLUSIVE(a, b)                                                                                                                        \
    do {                                                                                                                                             \
        if (modifier_counts[a] + modifier_counts[b] > 1)                                                                                             \
            throw error(std::format("{} and {} are mutually exclusive", TokenInfo::get_repr(a), TokenInfo::get_repr(b)), node);                      \
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
            }
        }

        // Module level declaration specific checks
        if (scope_stack.back()->get_type() == scope::ScopeType::MODULE) {
            if (modifier_counts[TokenType::PRIVATE] > 0)
                throw error("module level declarations cannot be 'private'", node);
            if (modifier_counts[TokenType::INTERNAL] > 0)
                throw error("module level declarations cannot be 'internal'", node);
            if (modifier_counts[TokenType::PROTECTED] > 0)
                throw error("module level declarations cannot be 'protected'", node);

            if (modifier_counts[TokenType::STATIC] > 0)
                throw error("module level declarations cannot be 'static'", node);
            if (modifier_counts[TokenType::OVERRIDE] > 0)
                throw error("module level declarations cannot be 'override'", node);

            if (is<ast::decl::Function>(node)) {
                if (modifier_counts[TokenType::ABSTRACT] > 0)
                    throw error("global functions cannot be 'abstract'", node);
                if (modifier_counts[TokenType::FINAL] > 0)
                    throw error("global functions cannot be 'final'", node);
            }
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
        if (const auto compound = dynamic_cast<ast::decl::Compound *>(node)) {
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
                if (modifier_counts[TokenType::FINAL] > 0)
                    throw error("interfaces cannot be 'final'", node);
                if (modifier_counts[TokenType::OVERRIDE] > 0)
                    throw error("interfaces cannot be 'override'", node);
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
        // Parent class/compound specific checks
        if (const auto compound = dynamic_cast<ast::decl::Compound *>(scope_stack.back()->get_node())) {
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
                if (modifier_counts[TokenType::FINAL] > 0 && modifier_counts[TokenType::STATIC] == 0)
                    throw error("'final' members are not allowed in interfaces (but final static is allowed)", node);
                if (modifier_counts[TokenType::OVERRIDE] > 0)
                    throw error("'override' members are not allowed in interfaces", node);

                // constants and static variables are allowed in interfaces
                if (const auto field = dynamic_cast<ast::decl::Variable *>(node)) {
                    if (field->get_token()->get_type() != TokenType::CONST && modifier_counts[TokenType::STATIC] == 0) {
                        throw error("fields are not allowed in interfaces (static and const fields are allowed)", node);
                    }
                }
                break;
            case TokenType::ANNOTATION:
                if (modifier_counts[TokenType::ABSTRACT] > 0)
                    throw error("'abstract' members are not allowed in annotations", node);
                break;
            default:
                throw Unreachable();    // surely some parser error
            }
        }
        // Constructor specific checks
        if (const auto fun_node = dynamic_cast<ast::decl::Function *>(node); fun_node && fun_node->get_name()->get_type() == TokenType::INIT) {
            if (modifier_counts[TokenType::ABSTRACT] > 0)
                throw error("constructor cannot be 'abstract'", fun_node);
            if (modifier_counts[TokenType::FINAL] > 0)
                throw error("constructor cannot be 'final'", fun_node);
            if (modifier_counts[TokenType::STATIC] > 0)
                throw error("constructor cannot be 'static'", fun_node);
            if (modifier_counts[TokenType::OVERRIDE] > 0)
                throw error("constructor cannot be 'override'", fun_node);
        }
    }

    void ScopeTreeBuilder::visit(ast::Reference &node) {}

    void ScopeTreeBuilder::visit(ast::type::Reference &node) {}

    void ScopeTreeBuilder::visit(ast::type::Function &node) {}

    void ScopeTreeBuilder::visit(ast::type::TypeLiteral &node) {}

    void ScopeTreeBuilder::visit(ast::type::Nullable &node) {}

    void ScopeTreeBuilder::visit(ast::type::TypeBuilder &node) {}

    void ScopeTreeBuilder::visit(ast::type::TypeBuilderMember &node) {}

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

    void ScopeTreeBuilder::visit(ast::expr::Lambda &node) {}

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
        } else if (node.get_name()->get_type() == TokenType::INIT) {
            if (scope_stack.back()->get_type() == scope::ScopeType::COMPOUND &&
                cast<scope::Compound>(scope_stack.back())->get_compound_node()->get_token()->get_type() == TokenType::INTERFACE)
                throw error("constructors are not allowed in interfaces", &node);
        }

        auto scope = begin_scope<scope::Function>(node);
        add_symbol(node.get_name()->get_text(), node.get_name(), scope);
        // make undefined interface functions implicitly abstract
        if (const auto compound = scope->get_enclosing_compound())
            if (compound->get_compound_node()->get_token()->get_type() == TokenType::INTERFACE && !node.get_definition())
                scope->set_abstract(true);
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::decl::Variable &node) {
        check_modifiers(&node, node.get_modifiers());
        if (scope_stack.empty())
            throw Unreachable();    // surely some parser error

        auto scope = begin_scope<scope::Variable>(node);
        add_symbol(node.get_name()->get_text(), node.get_name(), scope);
        // make interface constants implicitly static
        if (const auto compound = scope->get_enclosing_compound())
            if (compound->get_compound_node()->get_token()->get_type() == TokenType::INTERFACE && scope->is_const())
                scope->set_static(true);
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
        bool effectively_static = false;

        check_modifiers(&node, node.get_modifiers());
        if (!scope_stack.empty()) {
            if (scope_stack.back()->get_type() != scope::ScopeType::MODULE && scope_stack.back()->get_type() != scope::ScopeType::COMPOUND)
                throw Unreachable();    // surely some parser error
            if (scope_stack.back()->get_type() == scope::ScopeType::COMPOUND) {
                switch (cast<scope::Compound>(scope_stack.back())->get_compound_node()->get_token()->get_type()) {
                case TokenType::CLASS:
                    if (node.get_token()->get_type() == TokenType::ANNOTATION)
                        throw error("annotations are not allowed in classes", &node);
                    break;
                case TokenType::ENUM:
                    if (node.get_token()->get_type() == TokenType::ENUM)
                        throw error("nested enums are not allowed", &node);
                    if (node.get_token()->get_type() == TokenType::ANNOTATION)
                        throw error("annotations are not allowed in enums", &node);
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
                effectively_static = true;
            }
        } else {
            throw Unreachable();    // surely some parser error
        }

        auto scope = begin_scope<scope::Compound>(node);
        if (effectively_static)
            scope->set_static(true);

        add_symbol(node.get_name()->get_text(), node.get_name(), scope);
        if (node.get_token()->get_type() == TokenType::INTERFACE) {
            // Set interface to abstract by default
            scope->set_abstract(true);
        }
        for (auto enumerator: node.get_enumerators()) {
            enumerator->accept(this);
        }
        for (auto member: node.get_members()) {
            member->accept(this);
        }
        end_scope();
    }

    void ScopeTreeBuilder::visit(ast::Import &node) {}

    void ScopeTreeBuilder::visit(ast::Module &node) {
        if (!scope_stack.empty())
            throw Unreachable();
        auto scope = begin_scope<scope::Module>(node);
        for (auto import: node.get_imports()) import->accept(this);
        for (auto member: node.get_members()) member->accept(this);
        module_scope = cast<scope::Module>(scope_stack.back());
        end_scope();
    }

    const std::shared_ptr<scope::Module> &ScopeTreeBuilder::build() {
        module->accept(this);
        return module_scope;
    }
}    // namespace spadec