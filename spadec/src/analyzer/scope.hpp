#pragma once

#include <memory>
#include <stdexcept>
#include <unordered_map>
#include "analyzer.hpp"
#include "analyzer/analyzer.hpp"
#include "parser/ast.hpp"
#include "symbol_path.hpp"

namespace spade
{
    enum class ScopeType { MODULE, COMPOUND, FUNCTION, BLOCK };

    class Scope {
      protected:
        ScopeType type;
        ast::AstNode *node;
        std::vector<std::shared_ptr<Scope>> inner_scopes;

        std::unordered_map<string, ast::AstNode *> members;

      public:
        Scope(ScopeType type, ast::AstNode *node) : type(type), node(node) {}

        virtual ~Scope() = default;

        ScopeType get_type() const {
            return type;
        }

        ast::AstNode *get_node() const {
            return node;
        }

        const std::vector<std::shared_ptr<Scope>> &get_inner_scopes() const {
            return inner_scopes;
        }

        bool new_variable(const string &name, ast::AstNode *value) {
            if (members.contains(name))
                return false;
            members[name] = value;
            return true;
        }

        ast::AstNode *get_variable(const string &name) {
            try {
                return members.at(name);
            } catch (const std::out_of_range &) {
                return null;
            }
        }
    };

    class ModuleScope : public Scope {
      public:
        ModuleScope(ast::Module *node) : Scope(ScopeType::MODULE, node) {}

        ast::Module *get_module() const {
            return cast<ast::Module>(node);
        }
    };

    class CompoundScope : public Scope {
      public:
        CompoundScope(ast::decl::Compound *node) : Scope(ScopeType::COMPOUND, node) {}

        ast::decl::Compound *get_module() const {
            return cast<ast::decl::Compound>(node);
        }
    };

    class FunctionScope : public Scope {
      public:
        FunctionScope(ast::decl::Function *node) : Scope(ScopeType::FUNCTION, node) {}

        ast::decl::Function *get_module() const {
            return cast<ast::decl::Function>(node);
        }
    };

    class BlockScope : public Scope {
      public:
        BlockScope(ast::stmt::Block *node) : Scope(ScopeType::BLOCK, node) {}

        ast::stmt::Block *get_module() const {
            return cast<ast::stmt::Block>(node);
        }
    };
}    // namespace spade