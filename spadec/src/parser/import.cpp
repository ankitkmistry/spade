#include <fstream>
#include <ranges>

#include "import.hpp"

#include <iostream>
#include <sputils.hpp>

#include "parser.hpp"
#include "lexer/lexer.hpp"

namespace spade
{
    void ImportResolver::resolve_imports(std::shared_ptr<ast::Module> module) {
        resolved[module->get_file_path()] = module;
        LOGGER.log_info(std::format("resolved dependency: '{}'", module->get_file_path().generic_string()));
        std::vector<fs::path> import_paths;
        std::unordered_map<fs::path, std::shared_ptr<ast::Import>> nodes;
        for (const auto &sp_import: module->get_imports()) {
            auto path = sp_import->get_path(root_path, module);
            import_paths.push_back(path);
            nodes[path] = sp_import;
        }
        for (const auto &path: import_paths) {
            if (resolved.contains(path))
                break;
            if (!fs::exists(path))
                throw ImportError(std::format("cannot find dependency '{}'", path.generic_string()), module->get_file_path(),
                                  nodes[path]);
            if (!fs::is_regular_file(path))
                throw ImportError(std::format("dependency is not a file: '{}'", path.generic_string()), module->get_file_path(),
                                  nodes[path]);
            std::ifstream in(path);
            if (!in)
                throw FileOpenError(path.generic_string());
            std::stringstream ss;
            ss << in.rdbuf();
            Lexer lexer(path, ss.str());
            Parser parser(path, &lexer);
            auto mod = parser.parse();
            resolve_imports(mod);
        }
    }

    std::vector<std::shared_ptr<ast::Module>> ImportResolver::resolve_imports() {
        resolve_imports(module);
        std::vector<std::shared_ptr<ast::Module>> imports;
        imports.reserve(resolved.size());
        for (const auto &mod: resolved | std::views::values) imports.push_back(mod);
        return imports;
    }
}    // namespace spade