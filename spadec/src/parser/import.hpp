#pragma once

#include "ast.hpp"

namespace spade
{
    class ImportResolver final {
        fs::path root_path;
        std::shared_ptr<ast::Module> module;
        std::unordered_map<fs::path, std::shared_ptr<ast::Module>> resolved;

        void resolve_imports(std::shared_ptr<ast::Module> module);

      public:
        explicit ImportResolver(const fs::path &root_path, const std::shared_ptr<ast::Module> &module)
            : root_path(root_path), module(module) {}

        std::vector<std::shared_ptr<ast::Module>> resolve_imports();

        std::shared_ptr<ast::Module> get_module() const {
            return module;
        }
    };
}    // namespace spade