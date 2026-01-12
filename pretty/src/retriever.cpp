#include "retriever.hpp"
#include <cassert>
#include <cstddef>

namespace pretty
{
    void Retriever::add_command(const string &name, const std::function<void()> &action) {
        Node *node = null;
        auto *children = &roots;
        for (size_t i = 0; i < name.size(); i++) {
            char key = name[i];

            if (const auto it = children->find(key); it != children->end()) {
                node = &it->second;
                children = &node->children;

                if (i == name.size() - 1) {
                    node->is_leaf = true;
                    node->action = action;
                }
            } else {
                Node n;
                n.is_leaf = i == name.size() - 1;
                n.action = n.is_leaf ? action : null;
                auto [child, _] = children->emplace(key, n);

                node = &child->second;
                children = &node->children;
            }
        }
    }

    std::unordered_map<string, std::function<void()>> Retriever::get_command(const string &name) const {
        if (name.empty())
            return {};

        const Node *node = null;
        const auto *children = &roots;
        for (size_t i = 0; i < name.size(); i++) {
            char key = name[i];

            if (const auto it = children->find(key); it != children->end()) {
                node = &it->second;
                children = &node->children;
            } else {
                // TODO: implement fuzzy finding
                return {};
            }
        }

        assert(node != null);
        assert(children != null);

        if (node->is_leaf) {
            // clang-format off
            return {
                {name, node->action}
            };
            // clang-format on
        }

        std::unordered_map<string, std::function<void()>> result;
        for (const auto &[value, child]: *children) {
            collect(name, value, child, result);
        }
        return result;
    }

    void Retriever::collect(string name, char value, const Node &node, std::unordered_map<string, std::function<void()>> &result) const {
        name += value;
        if (node.is_leaf) {
            result.emplace(name, node.action);
        }

        for (const auto &[value, child]: node.children) {
            collect(name, value, child, result);
        }
    }

    // void Retriever::print() const {
    //     for (const auto &[val, node]: roots) {
    //         print_impl(val, node, 0);
    //     }
    // }
    //
    // void Retriever::print_impl(char value, const Node &node, size_t level) const {
    //     const string indent = string(level, ' ');
    //     std::cout << std::format("{}{} {}", indent, value, (node.is_leaf ? '*' : ' ')) << std::endl;
    //
    //     for (const auto &[val, node]: node.children) {
    //         print_impl(val, node, level + 1);
    //     }
    // }
}    // namespace pretty
