#include <limits>

#include "context.hpp"
#include "../utils/error.hpp"
#include "elpops/elpdef.hpp"
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