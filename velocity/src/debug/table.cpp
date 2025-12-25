#include "table.hpp"

namespace spade
{
    DataTable::DataTable(const string &title, const vector<string> &args) : title(title), keys(args), data(), width(0) {
        for (const auto &arg: args) {
            data[arg] = {};
        }
    }

    string DataTable::to_string() const {
        std::stringstream out;
        vector<size_t> maxes;
        string separator;
        vector<vector<string>> values_vector;

        // Initialize the maxes and keys
        size_t j = 0;
        for (const auto &values: data | std::views::values) {
            values_vector.push_back(values);
            size_t max = keys[j].length();
            for (const auto &value: values) {
                if (value.length() > max)
                    max = value.length();
            }
            maxes.push_back(max);
            j++;
        }

        // Build the separator
        separator += '+';
        for (auto max: maxes) separator += string(max + 2, '-') + '+';
        separator += '\n';

        // Build the string
        // Append the title
        out << title << '\n';
        // Append the separator
        out << separator;
        // Append the header
        out << '|';
        for (size_t i = 0; i < maxes.size(); ++i) {
            out << std::format(" {: <{}} ", keys[i], maxes[i]) << '|';
        }
        out << '\n';
        out << separator;

        // Append the rows
        for (int i = 0; i < width; ++i) {
            out << '|';
            int k = 0;
            for (auto values: values_vector) {
                auto value = values[i];
                out << std::format(" {} ", is_number(value) ? pad_right(value, maxes[k]) : pad_left(value, maxes[k])) << '|';
                k++;
            }
            out << '\n';
        }
        out << separator;
        return out.str();
    }

    std::ostream &operator<<(std::ostream &os, const DataTable &table) {
        return os << table.to_string() << '\n';
    }
}    // namespace spade
