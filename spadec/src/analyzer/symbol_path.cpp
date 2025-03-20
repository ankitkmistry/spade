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
}    // namespace spade