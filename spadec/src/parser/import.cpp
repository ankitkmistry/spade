#include <algorithm>
#include <execution>
#include <fstream>
#include <mutex>
#include <ranges>

#include "import.hpp"

#include "parser.hpp"
#include "lexer/lexer.hpp"

namespace spade
{
#ifdef ENABLE_MT
    static std::mutex resolve_imports_mutex;
#endif
    void ImportResolver::resolve_imports(std::shared_ptr<ast::Module> module) {
        std::vector<fs::path> import_paths;
        std::unordered_map<fs::path, std::shared_ptr<ast::Import>> nodes;
        {
#ifdef ENABLE_MT
            if (resolved.contains(module->get_file_path())) {
                return;    // Prevent circular imports
            }
            std::lock_guard lg(resolve_imports_mutex);
#endif
            // Set current module resolved
            resolved[module->get_file_path()] = module;
            LOGGER.log_info(std::format("resolved dependency: '{}'", module->get_file_path().generic_string()));
            // List up the import paths in current filesystem
            for (const auto &sp_import: module->get_imports()) {
                auto path = sp_import->get_path(root_path, module);
                import_paths.push_back(path);
                nodes[path] = sp_import;
            }
        }

#ifdef ENABLE_MT
        auto ex_policy = std::execution::par;
#else
        auto ex_policy = std::execution::seq;
#endif
        // Load the file
        std::for_each(ex_policy, import_paths.begin(), import_paths.end(), [&](const fs::path &path) {
            if (resolved.contains(path)) {
                nodes[path]->set_module(resolved[path]);
                return;    // Prevent circular imports
            }
            // Check for errors
            if (!fs::exists(path))
                throw ImportError(std::format("cannot find dependency '{}'", path.generic_string()), module->get_file_path(),
                                  nodes[path]);
            if (!fs::is_regular_file(path))
                throw ImportError(std::format("dependency is not a file: '{}'", path.generic_string()), module->get_file_path(),
                                  nodes[path]);
            // Process the file as usual
            std::ifstream in(path);
            if (!in)
                throw FileOpenError(path.generic_string());
            std::stringstream ss;
            ss << in.rdbuf();
            Lexer lexer(path, ss.str());
            Parser parser(path, &lexer);
            auto mod = parser.parse();
            resolve_imports(mod);
            nodes[path]->set_module(mod);
        });
    }

    std::vector<std::shared_ptr<ast::Module>> ImportResolver::resolve_imports() {
        resolve_imports(module);
        std::vector<std::shared_ptr<ast::Module>> imports;
        imports.reserve(resolved.size());
        for (const auto &mod: resolved | std::views::values) imports.push_back(mod);
        return imports;
    }
}    // namespace spade