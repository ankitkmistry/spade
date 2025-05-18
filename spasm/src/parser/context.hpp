#pragma once

#include <limits>
#include <unordered_map>
#include <variant>

#include "../utils/common.hpp"
#include "../lexer/token.hpp"
#include "boost/functional/hash.hpp"
#include "elpops/elpdef.hpp"

namespace spasm
{
    struct ValueContext {
        // int64_t int_val;
        // double float_val;
        // string str_val;
        // char chr_value;
        // vector<ValueContext> arr_val;
        std::variant<int64_t, double, string, char, vector<ValueContext>> value;

        ValueContext() = default;

        ValueContext(auto value) : value(value) {}

        operator CpInfo() const;

        bool operator==(const ValueContext &other) const {
            return value == other.value;
        }

        bool operator!=(const ValueContext &other) const {
            return value != other.value;
        }
    };
}    // namespace spasm

template<>
struct boost::hash<spasm::ValueContext> {
    size_t operator()(const spasm::ValueContext &ctx) const noexcept {
        if (const auto result = std::get_if<int64_t>(&ctx.value)) {
            return boost::hash<int64_t>{}(*result);
        } else if (const auto result = std::get_if<double>(&ctx.value)) {
            return boost::hash<double>{}(*result);
        } else if (const auto result = std::get_if<string>(&ctx.value)) {
            return boost::hash<string>{}(*result);
        } else if (const auto result = std::get_if<char>(&ctx.value)) {
            return boost::hash<char>{}(*result);
        } else {
            const auto array = std::get<std::vector<spasm::ValueContext>>(ctx.value);
            size_t seed = 0;
            for (const auto &item: array) boost::hash_combine(seed, item);
            return seed;
        }
    }
};

template<>
struct std::hash<spasm::ValueContext> {
    size_t operator()(const spasm::ValueContext &ctx) const noexcept {
        return boost::hash<spasm::ValueContext>()(ctx);
    }
};

namespace spasm
{
    struct ExceptionContext {
        std::shared_ptr<Token> from_label;
        std::shared_ptr<Token> to_label;
        std::shared_ptr<Token> dest_label;
        string type;
    };

    enum class ContextType {
        MODULE,
        CLASS,
        METHOD,
    };

    class Context {
        ContextType kind;

      public:
        Context(ContextType kind) : kind(kind) {}

        ContextType get_kind() const {
            return kind;
        }

        virtual ~Context() {}
    };

    class MethodContext : public Context {
        uint32_t cur_lineno = 0;
        std::unordered_map<string, decltype(std::declval<MethodInfo>().args_count)> args;
        std::unordered_map<string, decltype(std::declval<MethodInfo>().locals_count)> locals;
        vector<uint8_t> code;
        std::unordered_map<string, uint32_t> labels;
        std::unordered_map<string, vector<std::pair<std::shared_ptr<Token>, uint32_t>>> unresolved_labels;
        std::vector<uint32_t> linenos;

      public:
        MethodContext() : Context(ContextType::METHOD) {}

        MethodContext(const MethodContext &) = default;
        MethodContext(MethodContext &&) = default;
        MethodContext &operator=(const MethodContext &) = default;
        MethodContext &operator=(MethodContext &&) = default;
        ~MethodContext() = default;

        bool add_arg(const string &name) {
            if (auto it = args.find(name); it != args.end())
                return false;
            args[name] = args.size();
            return true;
        }

        bool add_local(const string &name) {
            if (auto it = locals.find(name); it != locals.end())
                return false;
            locals[name] = locals.size();
            return true;
        }

        std::optional<decltype(args)::value_type::second_type> get_arg(const string &name) const {
            if (auto it = args.find(name); it != args.end())
                return args.at(name);
            return std::nullopt;
        }

        std::optional<decltype(locals)::value_type::second_type> get_local(const string &name) const {
            if (auto it = locals.find(name); it != locals.end())
                return locals.at(name);
            return std::nullopt;
        }

        void set_line(uint32_t lineno) {
            cur_lineno = lineno;
        }

        void emit(Opcode opcode) {
            code.push_back(static_cast<uint8_t>(opcode));
            linenos.push_back(cur_lineno);
        }

        void emit(uint8_t value) {
            code.push_back(value);
            linenos.push_back(cur_lineno);
        }

        const vector<uint8_t> &get_code() const {
            return code;
        }

        bool define_label(const string &label) {
            if (auto it = labels.find(label); it != labels.end())
                return false;
            labels[label] = code.size();
            return true;
        }

        std::optional<uint32_t> get_label_pos(const string &label) const {
            if (auto it = labels.find(label); it != labels.end())
                return labels.at(label);
            return std::nullopt;
        }

        uint16_t patch_jump_to(const std::shared_ptr<Token> &label) {
            return patch_jump_to(label, code.size() & std::numeric_limits<uint32_t>::max());
        }

        uint16_t patch_jump_to(const std::shared_ptr<Token> &label, const uint32_t current_pos) {
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

        vector<std::shared_ptr<Token>> resolve_labels();

        LineInfo get_line_info() const;
    };

    class ModuleContext : public Context {
        fs::path file_path;
        string init;
        std::unordered_map<ValueContext, cpidx> constants;

      public:
        explicit ModuleContext(const fs::path &file_path) : Context(ContextType::MODULE), file_path(file_path) {}

        ModuleContext(const ModuleContext &) = default;
        ModuleContext(ModuleContext &&) = default;
        ModuleContext &operator=(const ModuleContext &) = default;
        ModuleContext &operator=(ModuleContext &&) = default;
        ~ModuleContext() = default;

        const string &get_init() const {
            return init;
        }

        void set_init(const string &str) {
            init = str;
        }

        cpidx get_constant(const ValueContext &value);

        vector<ValueContext> get_constants() const {
            vector<ValueContext> result(constants.size());
            for (const auto &[value, index]: constants) result[index] = value;
            return result;
        }
    };
}    // namespace spasm