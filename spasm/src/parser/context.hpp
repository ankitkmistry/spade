#pragma once

#include <limits>
#include <unordered_set>
#include <variant>

#include "../utils/common.hpp"
#include "../lexer/token.hpp"
#include "boost/functional/hash.hpp"

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

    class MatchContext {
        std::unordered_map<cpidx, std::shared_ptr<Token>> cases;
        std::shared_ptr<Token> default_label;

      public:
        bool add_case(cpidx value, const std::shared_ptr<Token> label) {
            if (const auto it = cases.find(value); it != cases.end())
                return false;
            cases[value] = label;
            return true;
        }

        const std::unordered_map<cpidx, std::shared_ptr<Token>> &get_cases() const {
            return cases;
        }

        std::shared_ptr<Token> get_default_label() const {
            return default_label;
        }

        void set_default_label(const std::shared_ptr<Token> &label) {
            default_label = label;
        }
    };

    enum class ContextType { MODULE, METHOD, CLASS };

    class Context {
        ContextType kind;

      public:
        Context(ContextType kind) : kind(kind) {}

        ContextType get_kind() const {
            return kind;
        }

        virtual ~Context() {}
    };

    class ClassContext : public Context {
        std::unordered_set<string> type_params;

      public:
        ClassContext() : Context(ContextType::CLASS) {}

        void add_type_param(const Sign &sign) {
            type_params.insert(sign.to_string());
        }

        bool has_type_param(const Sign &sign) {
            return type_params.contains(sign.to_string());
        }
    };

    class MethodContext : public Context {
        uint32_t cur_lineno = 0;

        std::unordered_map<string, decltype(std::declval<MethodInfo>().args_count)> args;
        std::unordered_map<string, decltype(std::declval<MethodInfo>().locals_count)> locals;
        std::unordered_set<string> type_params;

        vector<uint8_t> code;
        std::unordered_map<string, uint32_t> labels;
        std::unordered_map<string, vector<std::pair<std::shared_ptr<Token>, uint32_t>>> unresolved_labels;

        std::vector<uint32_t> linenos;

        std::unordered_map<string, std::pair<size_t, std::shared_ptr<MatchContext>>> matches;

      public:
        MethodContext() : Context(ContextType::METHOD) {}

        MethodContext(const MethodContext &) = default;
        MethodContext(MethodContext &&) = default;
        MethodContext &operator=(const MethodContext &) = default;
        MethodContext &operator=(MethodContext &&) = default;
        ~MethodContext() = default;

        bool add_match(const string &name, const std::shared_ptr<MatchContext> &match);
        std::optional<size_t> get_match(const string &name) const;
        vector<std::pair<string, std::shared_ptr<MatchContext>>> get_matches() const;

        bool add_arg(const string &name) {
            if (const auto it = args.find(name); it != args.end())
                return false;
            args[name] = args.size();
            return true;
        }

        bool add_local(const string &name) {
            if (const auto it = locals.find(name); it != locals.end())
                return false;
            locals[name] = locals.size();
            return true;
        }

        std::optional<decltype(args)::value_type::second_type> get_arg(const string &name) const {
            if (const auto it = args.find(name); it != args.end())
                return args.at(name);
            return std::nullopt;
        }

        std::optional<decltype(locals)::value_type::second_type> get_local(const string &name) const {
            if (const auto it = locals.find(name); it != locals.end())
                return locals.at(name);
            return std::nullopt;
        }

        void add_type_param(const Sign &sign) {
            type_params.insert(sign.to_string());
        }

        bool has_type_param(const Sign &sign) {
            return type_params.contains(sign.to_string());
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

        bool define_label(const string &label);
        std::optional<uint32_t> get_label_pos(const string &label) const;

        uint16_t patch_jump_to(const std::shared_ptr<Token> &label) {
            return patch_jump_to(label, code.size() & std::numeric_limits<uint32_t>::max());
        }

        uint16_t patch_jump_to(const std::shared_ptr<Token> &label, const uint32_t current_pos);
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