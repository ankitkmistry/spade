#pragma once

#include "common.hpp"
#include <functional>
#include <unordered_map>

namespace pretty
{
    class Retriever {
        struct Node {
            bool is_leaf = false;
            std::function<void()> action;
            std::unordered_map<char, Node> children = {};
        };

        std::unordered_map<char, Node> roots = {};

      public:
        Retriever() = default;
        Retriever(const Retriever &) = default;
        Retriever(Retriever &&) = default;
        Retriever &operator=(const Retriever &) = default;
        Retriever &operator=(Retriever &&) = default;
        ~Retriever() = default;

        void add_command(const string &name, const std::function<void()> &action = null);
        std::unordered_map<string, std::function<void()>> get_command(const string &name) const;

        // void print() const;

      private:
        void collect(string name, char value, const Node &node, std::unordered_map<string, std::function<void()>> &result) const;
        // void print_impl(char value, const Node &node, size_t level) const;
    };
}    // namespace pretty
