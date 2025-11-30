#include <limits>
#include <map>

#include "context.hpp"
#include "../utils/error.hpp"
#include "lexer/token.hpp"

namespace spasm
{
    ValueContext::operator CpInfo() const {
        if (auto result = std::get_if<int64_t>(&value)) {
            return CpInfo::from_int(*result);
        } else if (auto result = std::get_if<double>(&value)) {
            return CpInfo::from_float(*result);
        } else if (auto result = std::get_if<string>(&value)) {
            return CpInfo::from_string(*result);
        } else if (auto result = std::get_if<char>(&value)) {
            return CpInfo::from_char(*result);
        } else if (auto result = std::get_if<vector<ValueContext>>(&value)) {
            vector<CpInfo> array;
            array.reserve(result->size());
            for (const auto &item: *result) array.push_back(item);
            return CpInfo::from_array(array);
        } else
            throw Unreachable();
    }

    bool MethodContext::add_match(const string &name, const std::shared_ptr<MatchContext> &match) {
        if (const auto it = matches.find(name); it != matches.end())
            return false;
        matches[name] = {matches.size(), match};
        return true;
    }

    std::optional<size_t> MethodContext::get_match(const string &name) const {
        if (const auto it = matches.find(name); it != matches.end())
            return it->second.first;
        return std::nullopt;
    }

    vector<std::pair<string, std::shared_ptr<MatchContext>>> MethodContext::get_matches() const {
        vector<std::pair<string, std::shared_ptr<MatchContext>>> result(matches.size());
        for (const auto &[name, pair]: matches) {
            const auto &[idx, match] = pair;
            result[idx] = {name, match};
        }
        return result;
    }

    bool MethodContext::define_label(const string &label) {
        if (const auto it = labels.find(label); it != labels.end())
            return false;
        labels[label] = code.size();
        return true;
    }

    std::optional<uint32_t> MethodContext::get_label_pos(const string &label) const {
        if (const auto it = labels.find(label); it != labels.end())
            return labels.at(label);
        return std::nullopt;
    }

    uint16_t MethodContext::patch_jump_to(const std::shared_ptr<Token> &label, const uint32_t current_pos) {
        if (const auto it = labels.find(label->get_text()); it != labels.end()) {
            const uint32_t label_pos = it->second;
            if (current_pos > label_pos)
                return -(current_pos + 2 - label_pos);
            else
                return label_pos - current_pos - 2;
        }
        unresolved_labels[label->get_text()].emplace_back(label, current_pos);
        return 0;
    }

    vector<std::shared_ptr<Token>> MethodContext::resolve_labels() {
        vector<std::shared_ptr<Token>> undefined;
        for (const auto &[name, locations]: unresolved_labels)
            if (labels.contains(name)) {
                for (const auto &[token, current_pos]: locations) {
                    auto jmp_val = patch_jump_to(token, current_pos);
                    code[current_pos] = (jmp_val >> 8) & std::numeric_limits<uint8_t>::max();
                    code[current_pos + 1] = jmp_val & std::numeric_limits<uint8_t>::max();
                }
            } else
                for (const auto &[token, _]: locations) undefined.push_back(token);
        return undefined;
    }

    LineInfo MethodContext::get_line_info() const {
        std::map<uint32_t, vector<uint8_t>> line_table;
        for (auto lineno: linenos) {
            auto &list = line_table[lineno];
            if (list.empty()) {
                list.push_back(1);
            } else if (list.back() >= std::numeric_limits<uint8_t>::max()) {
                list.push_back(1);
            } else
                list.back()++;
        }

        LineInfo line_info;
        for (const auto &[lineno, repeat_list]: line_table)
            for (const auto &repeat: repeat_list) {
                NumberInfo number_info;
                number_info.times = repeat;
                number_info.lineno = lineno;
                if (line_info.numbers.size() >= std::numeric_limits<uint16_t>::max())
                    break;
                line_info.numbers.push_back(number_info);
            }
        line_info.number_count = line_info.numbers.size() & std::numeric_limits<uint16_t>::max();
        return line_info;
    }

    cpidx ModuleContext::get_constant(const ValueContext &value) {
        if (auto it = constants.find(value); it != constants.end())
            return it->second;
        if (constants.size() + 1 >= std::numeric_limits<cpidx>::max())
            throw AssemblerError(std::format("size of constant pool cannot be greater than {}", std::numeric_limits<cpidx>{}.max()), file_path);
        return constants[value] = constants.size();
    }
}    // namespace spasm
