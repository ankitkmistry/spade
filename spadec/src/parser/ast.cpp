#include "ast.hpp"

namespace spade
{
    static fs::path get_fs_path_from_sp_path(const string &str) {
        fs::path path;
        int start = 0;
        for (int i = 0; i < str.size(); ++i) {
            char c = str[i];
            if (c == '.') {
                path /= str.substr(start, i - start);
                start = i + 1;
            }
            if (i == str.size() - 1) {
                path /= str.substr(start, i - start + 1) + ".sp";
            }
        }
        return path;
    }

    fs::path ast::Import::get_path(const fs::path &root_path, const std::shared_ptr<Module> &module) const {
        if (path.empty()) return {};
        fs::path dir;
        if (path.size() > 1 && path[0] == '.') {
            if (path.size() > 2 && path[1] == '.') {
                dir = module->get_file_path().empty() ? fs::current_path()
                                                      : module->get_file_path().parent_path().parent_path();
                return dir / get_fs_path_from_sp_path(path.substr(2));
            }
            dir = module->get_file_path().empty() ? fs::current_path() : module->get_file_path().parent_path();
            return dir / get_fs_path_from_sp_path(path.substr(1));
        }
        dir = root_path.empty() ? fs::current_path() : root_path;
        return dir / get_fs_path_from_sp_path(path);
    }
}    // namespace spade