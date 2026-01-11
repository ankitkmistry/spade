#include "symbol_path.hpp"

namespace spadec
{
    SymbolPath::SymbolPath(const string &name) {
        if (name.empty())
            return;
        std::stringstream ss(name);
        string element;
        while (std::getline(ss, element, '.')) elements.push_back(element);
    }

    string SymbolPath::to_string() const {
        string result;
        for (const auto &element: elements) {
            result += element + ".";
        }
        if (!result.empty())
            result.pop_back();
        return result;
    }
}    // namespace spadec
