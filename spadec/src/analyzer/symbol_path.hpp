#pragma once

#include "utils/common.hpp"

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

        bool operator==(const SymbolPath &other) const {
            return elements == other.elements;
        }

        bool operator!=(const SymbolPath &other) const {
            return elements != other.elements;
        }

        SymbolPath operator+(const string &element) const {
            SymbolPath path(*this);
            if (!path.empty())
                path.elements.back() += element;
            else
                path.elements.push_back(element);
            return path;
        }

        SymbolPath &operator+=(const string &element) {
            if (!empty())
                elements.back() += element;
            else
                elements.push_back(element);
            return *this;
        }

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

        string get_name() const {
            return elements.empty() ? "" : elements.back();
        }

        SymbolPath get_parent() const {
            SymbolPath path(*this);
            path.elements.pop_back();
            return path;
        }

        string to_string() const;

        const std::vector<string> &get_elements() const {
            return elements;
        }
    };
}    // namespace spade

template<>
class std::hash<spade::SymbolPath> {
  public:
    size_t operator()(const spade::SymbolPath &path) const {
        return std::hash<std::string>{}(path.to_string());
    }
};