#include <numeric>

#include "symbol_path.hpp"

namespace spade
{
    SymbolPath::SymbolPath(const string &name) {
        if (name.empty())
            return;
        std::stringstream ss(name);
        string element;
        while (std::getline(ss, element, '.')) elements.push_back(element);
    }

    string SymbolPath::to_string() const {
        return std::accumulate(elements.begin(), elements.end(), string(),
                               [](const string &a, const string &b) { return a + (a.empty() ? "" : ".") + b; });
    }
}    // namespace spade