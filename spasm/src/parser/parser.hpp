#pragma once

#include "../lexer/lexer.hpp"
#include "elpops/elpdef.hpp"
#include "context.hpp"
#include <concepts>
#include <memory>

namespace spasm
{
    class Parser final {
      private:
        fs::path file_path;
        vector<std::shared_ptr<Token>> tokens;
        int index = 0;

        string entry_point;

        vector<std::shared_ptr<Context>> context_stack;
        std::shared_ptr<Context> get_current_context() const;
        std::shared_ptr<ModuleContext> get_current_module() const;

        int64_t str2int(const string &str);
        int64_t str2int(const std::shared_ptr<Token> &token);
        std::shared_ptr<Token> current();
        std::shared_ptr<Token> peek(int i = 0);
        std::shared_ptr<Token> advance();

        std::shared_ptr<Token> match(const string &text) {
            if (peek()->get_text() == text)
                return advance();
            return null;
        }

        template<typename... Ts>
            requires(std::same_as<TokenType, Ts> && ...)
        static string make_expected_string(Ts... types) {
            std::array<TokenType, sizeof...(types)> list{types...};
            string result;
            for (int i = 0; i < list.size(); ++i) {
                result += TokenInfo::get_repr(list[i]);
                if (i < list.size() - 1)
                    result += ", ";
            }
            return result;
        }

        template<typename... T>
            requires(std::same_as<TokenType, T> && ...)
        std::shared_ptr<Token> match(T... types) {
            const vector<TokenType> ts = {types...};
            for (auto t: ts) {
                if (peek()->get_type() == t)
                    return advance();
            }
            return null;
        }

        template<typename... T>
            requires(std::same_as<TokenType, T> && ...)
        std::shared_ptr<Token> expect(T... types) {
            const vector<TokenType> ts = {types...};
            for (auto t: ts) {
                if (peek()->get_type() == t)
                    return advance();
            }
            throw error(std::format("expected {}", make_expected_string(types...)));
        }

        template<typename ContextType, typename... Args>
            requires std::derived_from<ContextType, Context>
        std::shared_ptr<ContextType> begin_context(Args... args) {
            auto ctx = std::make_shared<ContextType>(args...);
            context_stack.push_back(ctx);
            return ctx;
        }

        void end_context() {
            context_stack.pop_back();
        }

        ParserError error(const string &msg, const std::shared_ptr<Token> &token);
        ParserError error(const string &msg);

        void parse_term(bool strict = true);

        ElpInfo parse_assembly();

        ModuleInfo parse_module();
        GlobalInfo parse_global();

        ClassInfo parse_class();
        FieldInfo parse_field();

        MethodInfo parse_method();
        ArgInfo parse_arg();
        LocalInfo parse_local();
        ExceptionContext parse_exception();
        void parse_line();

        ValueContext parse_value();
        vector<ValueContext> parse_array();

        string parse_name();

        string parse_signature();
        string parse_sign_atom();
        string parse_sign_param();

      public:
        /**
         * Construct a new Parser object
         * @param file_path the path to the file
         * @param lexer pointer to the token source
         */
        explicit Parser(Lexer lexer);

        Parser(const Parser &other) = default;
        Parser(Parser &&other) noexcept = default;
        Parser &operator=(const Parser &other) = default;
        Parser &operator=(Parser &&other) noexcept = default;
        ~Parser() = default;

        ElpInfo parse();
    };
}    // namespace spasm