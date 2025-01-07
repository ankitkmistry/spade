#pragma once

#include <numeric>

#include "parser/ast.hpp"

namespace spade
{
    class SymbolPath final {
        std::vector<string> elements;

      public:
        SymbolPath(const string &name = "");
        SymbolPath(const SymbolPath &other) = default;
        SymbolPath(SymbolPath &&other) noexcept = default;
        SymbolPath &operator=(const SymbolPath &other) = default;
        SymbolPath &operator=(SymbolPath &&other) noexcept = default;
        ~SymbolPath() = default;

        SymbolPath operator/(const string &element) const {
            SymbolPath path(*this);
            path.elements.push_back(element);
            return path;
        }

        SymbolPath operator/(const SymbolPath &other) const {
            SymbolPath path(*this);
            path.elements.insert(path.elements.end(), other.elements.begin(), other.elements.end());
            return path;
        }

        SymbolPath &operator/=(const string &element) {
            elements.push_back(element);
            return *this;
        }

        SymbolPath &operator/=(const SymbolPath &other) {
            elements.insert(elements.end(), other.elements.begin(), other.elements.end());
            return *this;
        }

        bool empty() const {
            return elements.empty();
        }

        string get_name() {
            return elements.empty() ? "" : elements.back();
        }

        SymbolPath get_parent() const {
            SymbolPath path(*this);
            path.elements.pop_back();
            return path;
        }

        string to_string() const {
            return std::accumulate(elements.begin(), elements.end(), string(),
                                   [](const string &a, const string &b) { return a + (a.empty() ? "" : ".") + b; });
        }
    };

    class Analyzer {
        fs::path root_path;
        std::unordered_map<SymbolPath, std::shared_ptr<ast::AstNode>> symbol_table;

        void set_root_path(const std::vector<std::shared_ptr<ast::Module>> &modules);

      public:
        void analyze(const std::vector<std::shared_ptr<ast::Module>> &modules);
    };
}    // namespace spade
