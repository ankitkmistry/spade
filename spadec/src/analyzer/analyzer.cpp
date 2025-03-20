#include "analyzer.hpp"
#include "parser/ast.hpp"
#include "scope.hpp"
#include "scope_tree.hpp"
#include "utils/error.hpp"

namespace spade
{
    std::shared_ptr<scope::Module> Analyzer::get_current_module() {
        for (auto itr = scope_stack.rbegin(); itr != scope_stack.rend(); ++itr)
            if (auto scope = *itr; scope->get_type() == scope::ScopeType::MODULE)
                return cast<scope::Module>(scope);
        return null;
    }

    std::shared_ptr<scope::Scope> Analyzer::get_parent_scope() {
        return scope_stack[scope_stack.size() - 2];
    }

    std::shared_ptr<scope::Scope> Analyzer::get_current_scope() {
        return scope_stack.back();
    }

    void Analyzer::analyze(const std::vector<std::shared_ptr<ast::Module>> &modules) {
        if (modules.empty())
            return;
        // Build symbol table
        ScopeTreeBuilder builder(modules);
        module_scopes = builder.build();
        for (auto [_, module]: module_scopes) {
            if (module->get_type() == scope::ScopeType::FOLDER_MODULE)
                module->print(cast<scope::FolderModule>(module)->get_module_node()->get_name());
            else
                module->print(cast<scope::Module>(module)->get_module_node()->get_name());
        }
        // Start analysis
        for (auto module: modules) {
            module->accept(this);
        }
    }

    void Analyzer::visit(ast::Reference &node) {
        _res_reference = null;
        // Find the scope where name is located
        auto name = node.get_path()[0]->get_text();
        std::shared_ptr<scope::Scope> scope;
        for (auto itr = scope_stack.rbegin(); itr != scope_stack.rend(); ++itr) {
            if ((*itr)->has_variable(name))
                scope = (*itr)->get_variable(name);
        }
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
        node.get_reference()->accept(this);
        auto type_scope = _res_reference;
        if (type_scope->get_type() != scope::ScopeType::COMPOUND)
            throw ErrorGroup<AnalyzerError>(std::pair(ErrorType::ERROR, error("reference is not a type", &node)),
                                            std::pair(ErrorType::NOTE, error("declared here", type_scope)));
    }

    void Analyzer::visit(ast::type::Function &node) {}

    void Analyzer::visit(ast::type::TypeLiteral &node) {}

    void Analyzer::visit(ast::type::BinaryOp &node) {}

    void Analyzer::visit(ast::type::Nullable &node) {}

    void Analyzer::visit(ast::type::TypeBuilder &node) {}

    void Analyzer::visit(ast::type::TypeBuilderMember &node) {}

    void Analyzer::visit(ast::expr::Constant &node) {}

    void Analyzer::visit(ast::expr::Super &node) {}

    void Analyzer::visit(ast::expr::Self &node) {}

    void Analyzer::visit(ast::expr::DotAccess &node) {}

    void Analyzer::visit(ast::expr::Call &node) {}

    void Analyzer::visit(ast::expr::Argument &node) {}

    void Analyzer::visit(ast::expr::Reify &node) {}

    void Analyzer::visit(ast::expr::Index &node) {}

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
        if (get_parent_scope()->get_type() == scope::ScopeType::FUNCTION) {
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
        auto value = module_scopes[node.get_module().get()];
        if (!scope->new_variable(name, value)) {
            // Find the original declaration
            auto org_def = scope->get_decl_site(name->get_text());
            throw ErrorGroup<AnalyzerError>(
                    std::pair(ErrorType::ERROR, error(std::format("redeclaration of '{}'", name->get_text()), name)),
                    std::pair(ErrorType::NOTE, error("already declared here", org_def)));
        }
    }

    void Analyzer::visit(ast::Module &node) {
        auto scope = module_scopes[&node];
        scope_stack.push_back(scope);

        for (auto import: node.get_imports()) import->accept(this);
        for (auto member: node.get_members()) member->accept(this);

        scope_stack.pop_back();
    }

    void Analyzer::visit(ast::FolderModule &node) {
        auto scope = module_scopes[&node];
        scope_stack.push_back(scope);

        for (auto [name, member]: scope->get_members()) {
            auto [_, scope] = member;
            scope->get_node()->accept(this);
        }

        scope_stack.pop_back();
    }
}    // namespace spade