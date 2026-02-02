#include <cassert>
#include <charconv>
#include <concepts>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <variant>

#include "parser.hpp"
#include "context.hpp"
#include "lexer/token.hpp"
#include "utils/error.hpp"

namespace spasm
{
    static string destringify(const string &str) {
        if (str == "\"\"")
            return "";
        return str.substr(1, str.size() - 2);
    }

    Parser::Parser(Lexer lexer) : file_path(lexer.get_file_path()) {
        std::shared_ptr<Token> token;
        do tokens.push_back(token = lexer.next_token());
        while (token->get_type() != spasm::TokenType::END_OF_FILE);
    }

    std::shared_ptr<Context> Parser::get_current_context() const {
        return context_stack.empty() ? null : context_stack.back();
    }

    std::shared_ptr<ModuleContext> Parser::get_current_module() const {
        for (auto it = context_stack.rbegin(); it != context_stack.rend(); it++) {
            auto context = *it;
            if (context->get_kind() == ContextType::MODULE)
                return cast<ModuleContext>(context);
        }
        return null;
    }

    int64_t Parser::str2int(const string &str) {
        try {
            if constexpr (std::same_as<int64_t, long long>)
                return static_cast<int64_t>(std::stoll(str));
            else if constexpr (std::same_as<int64_t, long>)
                return static_cast<int64_t>(std::stol(str));
            else
                return static_cast<int64_t>(std::stoi(str));
        } catch (const std::out_of_range &) {
            throw error("number is out of range", peek());
        } catch (const std::invalid_argument &) {
            throw error("number is invalid", peek());
        }
    }

    int64_t Parser::str2int(const std::shared_ptr<Token> &token) {
        int sign = 1;
        string str = token->get_text();
        int base = 10;

        if (str.size() >= 1 && str[0] == '-') {
            str = str.substr(1);
            sign = -1;
        }

        if (str.size() >= 2 && str.starts_with("0x")) {
            str = str.substr(2);
            base = 16;
        } else if (str.size() > 1 && str[0] == '0') {
            str = str.substr(1);
            base = 8;
        } else if (str.size() > 2 && str.starts_with("0b")) {
            str = str.substr(2);
            base = 2;
        }

        int64_t value = 0;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value, base);
        if (ec == std::errc())
            return value * sign;
        else if (ec == std::errc::invalid_argument)
            throw error("number is invalid", token);
        else if (ec == std::errc::result_out_of_range)
            throw error("number is out of range", token);
        throw Unreachable();
    }

    std::shared_ptr<Token> Parser::current() {
        if (index >= tokens.size())
            return tokens.back();    // EOF
        return tokens[index - 1];
    }

    std::shared_ptr<Token> Parser::peek(int i) {
        auto idx = index + i;
        if (idx >= tokens.size())
            return tokens.back();    // EOF
        return tokens[idx];
    }

    std::shared_ptr<Token> Parser::advance() {
        if (index >= tokens.size())
            return tokens.back();    // EOF
        return tokens[index++];
    }

    ParserError Parser::error(const string &msg, const std::shared_ptr<Token> &token) {
        return {msg, file_path, token->get_line_start(), token->get_col_start(), token->get_line_end(), token->get_col_end()};
    }

    ParserError Parser::error(const string &msg) {
        return error(msg, peek());
    }

    void Parser::parse_term(bool strict) {
        if (strict)
            expect(TokenType::NEWLINE);
        while (match(TokenType::NEWLINE));
    }

    ElpInfo Parser::parse_assembly() {
        vector<std::shared_ptr<Token>> imports;
        while (match(TokenType::IMPORT)) {
            imports.push_back(expect(TokenType::STRING));
            parse_term();
        }

        vector<ModuleInfo> modules;
        while (peek()->get_type() != TokenType::END_OF_FILE) {
            modules.push_back(parse_module());
        }

        const bool excecutable = !entry_point.empty();
        ElpInfo elp;
        elp.magic = excecutable ? 0xC0FFEEDE : 0xDEADCAFE;
        elp.major_version = 1;
        elp.minor_version = 0;

        // elp.entry = ctx.get_constant(excecutable ? entry_point.to_string() : "");
        // elp.imports = ctx.get_constant(vector<ValueContext>());
        elp.entry = std::get<_UTF8>(CpInfo::from_string(entry_point.to_string()).value);
        elp.imports_count = 0;
        elp.imports = {};

        if (const auto max = std::numeric_limits<decltype(elp.modules_count)>::max(); modules.size() >= max)
            throw ParserError{std::format("modules_count cannot be >= {}", max), file_path, -1, -1, -1, -1};
        else {
            elp.modules_count = modules.size() & max;
            elp.modules = modules;
        }
        return elp;
    }

    ModuleInfo Parser::parse_module() {
        const auto ctx = begin_context<ModuleContext>(file_path);

        auto start = expect(TokenType::MODULE);
        auto name = expect(TokenType::IDENTIFIER)->get_text();
        current_sign |= SignElement(name, Sign::Kind::MODULE);
        parse_term();

        vector<GlobalInfo> globals;
        while (match(TokenType::GLOBAL)) globals.push_back(parse_global());

        vector<MethodInfo> methods;
        vector<ClassInfo> classes;
        vector<ModuleInfo> modules;
        bool end = false;
        while (!end) {
            switch (peek()->get_type()) {
            case TokenType::METHOD:
                methods.push_back(parse_method());
                break;
            case TokenType::CLASS:
                classes.push_back(parse_class());
                break;
            case TokenType::MODULE:
                modules.push_back(parse_module());
                break;
            default:
                end = true;
                break;
            }
        }
        expect(TokenType::END);
        parse_term(false);

        ModuleInfo module;
        module.compiled_from = ctx->get_constant(file_path.string());
        module.name = ctx->get_constant(name);
        module.init = ctx->get_constant(ctx->get_init());

        if (const auto max = std::numeric_limits<decltype(module.globals_count)>::max(); globals.size() >= max)
            throw error(std::format("globals_count cannot be >= {}", max), start);
        else {
            module.globals_count = globals.size() & max;
            module.globals = globals;
        }
        if (const auto max = std::numeric_limits<decltype(module.methods_count)>::max(); methods.size() >= max)
            throw error(std::format("methods_count cannot be >= {}", max), start);
        else {
            module.methods_count = methods.size() & max;
            module.methods = methods;
        }
        if (const auto max = std::numeric_limits<decltype(module.classes_count)>::max(); classes.size() >= max)
            throw error(std::format("classes_count cannot be >= {}", max), start);
        else {
            module.classes_count = classes.size() & max;
            module.classes = classes;
        }
        if (const auto max = std::numeric_limits<decltype(module.modules_count)>::max(); modules.size() >= max)
            throw error(std::format("modules_count cannot be >= {}", max), start);
        else {
            module.modules_count = modules.size() & max;
            module.modules = modules;
        }

        const auto constants = ctx->get_constants();
        vector<CpInfo> constant_pool;
        constant_pool.reserve(constants.size());
        for (const auto &constant: constants) constant_pool.push_back(constant);
        if (const auto max = std::numeric_limits<decltype(module.constant_pool_count)>::max(); constant_pool.size() >= max)
            throw error(std::format("constant_pool_count cannot be >= {}", max), start);
        else {
            module.constant_pool_count = constant_pool.size() & max;
            module.constant_pool = constant_pool;
        }

        end_context();
        return module;
    }

    GlobalInfo Parser::parse_global() {
        const auto property = expect(TokenType::PROPERTY);
        const auto name = parse_name();
        expect(TokenType::COLON);
        const auto type = parse_signature().to_string();
        parse_term();

        GlobalInfo global;
        if (property->get_text() == "@var") {
            global.kind = 0;
        } else if (property->get_text() == "@const") {
            global.kind = 1;
        } else
            throw error("expected '@var', '@const'", property);
        global.name = get_current_module()->get_constant(name);
        return global;
    }

    ClassInfo Parser::parse_class() {
        const auto ctx = begin_context<ClassContext>();

        const auto start = expect(TokenType::CLASS);
        const auto sign = parse_sign_class();
        const auto name = sign.to_string();
        current_sign |= sign;
        parse_term();

        std::unordered_map<string, ValueContext> properties{
                {"@kind",   0                     },
                {"@supers", vector<ValueContext>()},
        };
        while (match(TokenType::PROPERTY)) {
            const auto property_name = current()->get_text();
            if (property_name == "@kind") {
                const auto value = expect(TokenType::IDENTIFIER);
                if (value->get_text() == "class")
                    properties[property_name] = 0;
                else if (value->get_text() == "interface")
                    properties[property_name] = 1;
                else if (value->get_text() == "annotation")
                    properties[property_name] = 2;
                else if (value->get_text() == "enum")
                    properties[property_name] = 3;
                else
                    throw error(std::format("possible values for '{}' are 'class', 'interface', 'annotation', 'enum'", property_name), current());
            } else if (property_name == "@supers") {
                auto value = parse_array();
                for (const auto &item: value) {
                    if (!std::holds_alternative<string>(item.value))
                        throw error(std::format("value for '{}' should be an array of strings or signatures", property_name), current());
                }
                properties[property_name] = value;
            } else
                throw error(std::format("unknown property: '{}'", current()->get_text()), current());
            parse_term();
        }

        vector<FieldInfo> fields;
        vector<MethodInfo> methods;

        while (peek()->get_type() != TokenType::END) {
            if (peek()->get_type() == TokenType::END_OF_FILE)
                throw error(std::format("expected {}", make_expected_string(TokenType::END)));
            switch (peek()->get_type()) {
            case TokenType::FIELD:
                advance();
                fields.push_back(parse_field());
                break;
            case TokenType::METHOD:
                methods.push_back(parse_method());
                break;
            default:
                break;
            }
        }

        expect(TokenType::END);
        parse_term(false);

        ClassInfo klass;
        klass.kind = static_cast<uint8_t>(std::get<int64_t>(properties["@kind"].value));
        klass.name = get_current_module()->get_constant(name);
        klass.supers = get_current_module()->get_constant(std::get<vector<ValueContext>>(properties["@supers"].value));

        if (const auto max = std::numeric_limits<decltype(klass.fields_count)>::max(); fields.size() >= max)
            throw error(std::format("fields_count cannot be >= {}", max), start);
        else {
            klass.fields_count = fields.size() & max;
            klass.fields = fields;
        }
        if (const auto max = std::numeric_limits<decltype(klass.methods_count)>::max(); methods.size() >= max)
            throw error(std::format("methods_count cannot be >= {}", max), start);
        else {
            klass.methods_count = methods.size() & max;
            klass.methods = methods;
        }
        end_context();
        return klass;
    }

    FieldInfo Parser::parse_field() {
        const auto property = expect(TokenType::PROPERTY);
        const auto name = parse_name();
        expect(TokenType::COLON);
        const auto type = parse_signature().to_string();
        parse_term();

        FieldInfo field;
        if (property->get_text() == "@var") {
            field.kind = 0;
        } else if (property->get_text() == "@const") {
            field.kind = 1;
        } else
            throw error("expected '@var', '@const'", property);
        field.name = get_current_module()->get_constant(name);
        return field;
    }

    MethodInfo Parser::parse_method() {
        const auto ctx = begin_context<MethodContext>();

        const auto start = expect(TokenType::METHOD);
        const auto property = match(TokenType::PROPERTY);
        const auto sign = parse_sign_method();
        const auto name = sign.to_string();
        current_sign |= sign;
        parse_term();

        std::unordered_map<string, ValueContext> properties{
                {"@stack_max", 8},
        };
        while (match(TokenType::PROPERTY)) {
            const auto property_name = current()->get_text();
            if (property_name == "@stack_max") {
                static constinit auto max = std::numeric_limits<decltype(std::declval<MethodInfo>().stack_max)>::max();
                auto value = str2int(expect(TokenType::INTEGER));
                if (value >= max)
                    throw error(std::format("'{}' cannot be >= {}", property_name, max), current());
                properties[property_name] = value;
            } else
                throw error(std::format("unknown property: '{}'", property_name), current());
            parse_term();
        }

        size_t args_count = 0;
        size_t locals_count = 0;
        vector<ExceptionContext> exceptions;
        if (context_stack[context_stack.size() - 2]->get_kind() == ContextType::CLASS) {
            locals_count++;
            ctx->add_local("self");
        }

        while (peek()->get_type() != TokenType::END) {
            if (peek()->get_type() == TokenType::END_OF_FILE)
                throw error(std::format("expected {}", make_expected_string(TokenType::END)));
            switch (peek()->get_type()) {
            case TokenType::ARG:
                advance();
                parse_arg();
                args_count++;
                break;
            case TokenType::LOCAL:
                advance();
                parse_local();
                locals_count++;
                break;
            case TokenType::EXCEPTION:
                advance();
                exceptions.push_back(parse_exception());
                break;
            case TokenType::MATCH: {
                advance();
                const auto name = parse_name();
                const auto name_tok = current();
                parse_term();
                const auto match_ctx = std::make_shared<MatchContext>();
                if (!ctx->add_match(name, match_ctx))
                    throw error(std::format("redefinition of match '{}'", name), name_tok);
                while (peek()->get_type() != TokenType::END) {
                    if (peek()->get_type() == TokenType::END_OF_FILE)
                        throw error(std::format("expected {}", make_expected_string(TokenType::END)));
                    if (match(TokenType::UNDERSCORE)) {
                        if (match_ctx->get_default_label())
                            throw ErrorGroup<ParserError>()
                                    .error(error("redefinition of default case", current()))
                                    .note(error("already declared here", match_ctx->get_default_label()));
                        expect(TokenType::ARROW);
                        match_ctx->set_default_label(expect(TokenType::LABEL));
                    } else {
                        const auto tok = peek();
                        const auto value = parse_value();
                        expect(TokenType::ARROW);
                        const auto label = expect(TokenType::LABEL);
                        if (!match_ctx->add_case(get_current_module()->get_constant(value), label))
                            throw error("redefinition of case", tok);
                    }
                    parse_term();
                }
                if (!match_ctx->get_default_label())
                    throw error("match must have a default label", name_tok);
                expect(TokenType::END);
                parse_term();
                break;
            }
            default:
                goto outside;
            }
        }

outside:
        while (peek()->get_type() != TokenType::END) {
            if (peek()->get_type() == TokenType::END_OF_FILE)
                throw error(std::format("expected {}", make_expected_string(TokenType::END)));
            switch (peek()->get_type()) {
            case TokenType::ARG:
                parse_arg();
                args_count++;
                break;
            case TokenType::LOCAL:
                parse_local();
                locals_count++;
                break;
            case TokenType::EXCEPTION:
                exceptions.push_back(parse_exception());
                break;
            default:
                parse_line();
            }
        }

        expect(TokenType::END);
        parse_term(false);

        if (property) {
            if (property->get_text() == "@entry") {
                entry_point = current_sign;
            } else if (property->get_text() == "@init") {
                get_current_module()->set_init(name);
            }
        }

        ErrorGroup<ParserError> errors;
        const auto undefined = ctx->resolve_labels();
        for (const auto &token: undefined) {
            errors.error(error(std::format("undefined reference to label '{}'", token->get_text()), token));
        }

        MethodInfo method;
        method.kind = context_stack[context_stack.size() - 2]->get_kind() == ContextType::MODULE ? 0x00 : 0x01;
        method.name = get_current_module()->get_constant(name);

        if (const auto max = std::numeric_limits<decltype(method.args_count)>::max(); args_count >= max)
            errors.error(error(std::format("args_count cannot be >= {}", max), start));
        else {
            method.args_count = args_count & max;
        }
        if (const auto max = std::numeric_limits<decltype(method.locals_count)>::max(); locals_count >= max)
            errors.error(error(std::format("locals_count cannot be >= {}", max), start));
        else {
            method.locals_count = locals_count & max;
        }
        if (const auto max = std::numeric_limits<decltype(method.exception_table_count)>::max(); exceptions.size() >= max)
            errors.error(error(std::format("exception_table_count cannot be >= {}", max), start));
        else {
            method.exception_table_count = exceptions.size() & max;
            method.exception_table = vector<ExceptionTableInfo>(method.exception_table_count);
            for (size_t i = 0; i < exceptions.size(); i++) {
                const auto &exception = exceptions[i];
                if (const auto pos = ctx->get_label_pos(exception.from_label->get_text()))
                    method.exception_table[i].start_pc = *pos;
                else
                    errors.error(error(std::format("undefined reference to label '{}'", exception.from_label->get_text()), exception.from_label));
                if (const auto pos = ctx->get_label_pos(exception.to_label->get_text()))
                    method.exception_table[i].end_pc = *pos;
                else
                    errors.error(error(std::format("undefined reference to label '{}'", exception.to_label->get_text()), exception.to_label));
                if (const auto pos = ctx->get_label_pos(exception.dest_label->get_text()))
                    method.exception_table[i].target_pc = *pos;
                else
                    errors.error(error(std::format("undefined reference to label '{}'", exception.dest_label->get_text()), exception.dest_label));
                method.exception_table[i].exception = get_current_module()->get_constant(exception.type);
            }
        }
        {    // Set the matches
            const auto matches_list = ctx->get_matches();
            if (const auto max = std::numeric_limits<decltype(method.match_count)>::max(); matches_list.size() >= max)
                errors.error(error(std::format("match_count cannot be >= {}", max), start));
            else {
                method.match_count = matches_list.size() & max;
                method.matches.resize(matches_list.size());
                for (size_t match_idx = 0; const auto &[name, match]: matches_list) {
                    MatchInfo info;
                    // Set the default location
                    if (const auto label = match->get_default_label(); const auto pos = ctx->get_label_pos(label->get_text()))
                        info.default_location = *pos;
                    else
                        errors.error(error(std::format("undefined reference to label '{}'", label->get_text()), label));
                    // Set the cases accordingly
                    if (const auto max = std::numeric_limits<decltype(info.case_count)>::max(); match->get_cases().size() >= max) {
                        errors.error(error(std::format("case_count cannot be >= {}", max), start));
                        break;
                    } else {
                        info.case_count = match->get_cases().size() & max;
                        info.cases.resize(match->get_cases().size());
                        for (size_t case_idx = 0; const auto &[value, case_label]: match->get_cases()) {
                            if (const auto pos = ctx->get_label_pos(case_label->get_text()))
                                info.cases[case_idx] = CaseInfo{.value = value, .location = *pos};
                            else
                                errors.error(error(std::format("undefined reference to label '{}'", case_label->get_text()), case_label));
                            case_idx++;
                        }
                        // Put it in the method
                        method.matches[match_idx] = std::move(info);
                        match_idx++;
                    }
                }
            }
        }

        method.line_info = ctx->get_line_info();
        {    // Set the stack max
            using T = decltype(method.stack_max);
            method.stack_max = static_cast<T>(std::get<int64_t>(properties["@stack_max"].value) & std::numeric_limits<T>::max());
        }
        // Set the code
        const auto code = ctx->get_code();
        if (const auto max = std::numeric_limits<decltype(method.code_count)>::max(); code.size() >= max)
            errors.error(error(std::format("code_count cannot be >= {}", max), start));
        else {
            method.code_count = code.size() & max;
            method.code = code;
        }

        end_context();
        if (!errors.get_errors().empty())
            throw errors;
        return method;
    }

    void Parser::parse_arg() {
        const auto ctx = cast<MethodContext>(get_current_context());
        const auto property = match(TokenType::PROPERTY);
        const auto name = parse_name();
        const auto name_tok = current();
        expect(TokenType::COLON);
        const auto type = parse_signature().to_string();
        parse_term();

        if (!ctx->add_arg(name))
            throw error(std::format("redefinition of arg '{}'", name), name_tok);

        if (property->get_text() != "@var" && property->get_text() != "@const")
            throw error("expected '@var', '@const'", property);
    }

    void Parser::parse_local() {
        const auto ctx = cast<MethodContext>(get_current_context());
        const auto property = expect(TokenType::PROPERTY);
        const auto name = parse_name();
        const auto name_tok = current();
        expect(TokenType::COLON);
        const auto type = parse_signature().to_string();
        parse_term();

        if (!ctx->add_local(name))
            throw error(std::format("redefinition of local '{}'", name), name_tok);

        if (property->get_text() != "@var" && property->get_text() != "@const")
            throw error("expected '@var', '@const'", property);
    }

    ExceptionContext Parser::parse_exception() {
        ExceptionContext exception;
        exception.from_label = expect(TokenType::LABEL);
        expect(TokenType::DASH);
        exception.to_label = expect(TokenType::LABEL);
        expect(TokenType::ARROW);
        exception.dest_label = expect(TokenType::LABEL);
        expect(TokenType::COLON);
        exception.type = parse_signature().to_string();
        parse_term();
        return exception;
    }

    void Parser::parse_line() {
        static constinit const auto uint8_max = std::numeric_limits<uint8_t>::max();
        static constinit const auto uint16_max = std::numeric_limits<uint16_t>::max();
        const auto module = get_current_module();
        const auto klass = context_stack[context_stack.size() - 2]->get_kind() == ContextType::CLASS
                                   ? cast<ClassContext>(context_stack[context_stack.size() - 2])
                                   : null;
        const auto ctx = cast<MethodContext>(get_current_context());

        if (match(TokenType::LABEL)) {
            if (!ctx->define_label(current()->get_text()))
                throw error(std::format("redeclaration of label '{}'", current()->get_text()), current());
            expect(TokenType::COLON);
            if (peek()->get_type() == TokenType::NEWLINE)
                parse_term();
        }

        const auto opcode_token = expect(TokenType::IDENTIFIER);
        Opcode opcode;
        if (const auto opt = OpcodeInfo::from_string(opcode_token->get_text()))
            opcode = *opt;
        else
            throw error(std::format("invalid opcode: '{}'", opcode_token->get_text()), opcode_token);
        ctx->set_line(opcode_token->get_line_start());
        ctx->emit(opcode);

        const auto emit_value = [&](std::unsigned_integral auto const value) {
            switch (OpcodeInfo::params_count(opcode)) {
            case 0:
                break;
            case 1: {
                if (value >= uint8_max) {
                    ErrorGroup<ParserError> errors;
                    errors.error(error(
                            std::format("opcode '{}' cannot accept a value more than {} (value={})", opcode_token->get_text(), uint8_max, value),
                            opcode_token));
                    if (opcode != OpcodeInfo::alternate(opcode))
                        errors.note(error(std::format("use '{}' instead", OpcodeInfo::to_string(OpcodeInfo::alternate(opcode))), opcode_token));
                    throw errors;
                }
                ctx->emit(value & uint8_max);
                break;
            }
            case 2:
                if (value >= uint16_max)
                    throw error(std::format("opcode '{}' cannot accept a value more than {} (value={})", opcode_token->get_text(), uint16_max, value),
                                opcode_token);
                if constexpr (std::same_as<decltype(value), const uint8_t>) {
                    ctx->emit(0);
                    ctx->emit(value);
                } else {
                    ctx->emit((value >> 8) & uint8_max);
                    ctx->emit(value & uint8_max);
                }
                break;
            default:
                if (value >= uint16_max)
                    throw error(std::format("opcode '{}' cannot accept a value more than {} (value={})", opcode_token->get_text(), uint16_max, value),
                                opcode_token);
                if constexpr (std::same_as<decltype(value), const uint8_t>) {
                    ctx->emit(0);
                    ctx->emit(value);
                } else {
                    ctx->emit((value >> 8) & uint8_max);
                    ctx->emit(value & uint8_max);
                }
                break;
            }
        };

        if (OpcodeInfo::params_count(opcode) == 0) {
            parse_term();
            return;
        }

        switch (opcode) {
        case Opcode::CONST:
        case Opcode::CONSTL:
            emit_value(module->get_constant(parse_value()));
            break;
        case Opcode::NPOP:
        case Opcode::NDUP:
            emit_value(static_cast<uint64_t>(str2int(expect(TokenType::INTEGER))));
            break;
        case Opcode::GLOAD:
        case Opcode::GFLOAD:
        case Opcode::GSTORE:
        case Opcode::GFSTORE:
        case Opcode::PGSTORE:
        case Opcode::PGFSTORE:
        case Opcode::GINVOKE:
        case Opcode::GFINVOKE:
            emit_value(module->get_constant(parse_signature().to_string()));
            break;
        case Opcode::LLOAD:
        case Opcode::LFLOAD:
        case Opcode::LSTORE:
        case Opcode::LFSTORE:
        case Opcode::PLSTORE:
        case Opcode::PLFSTORE:
        case Opcode::LINVOKE:
        case Opcode::LFINVOKE: {
            if (match(TokenType::INTEGER)) {
                const auto value = str2int(current());
                if (value >= uint16_max)
                    throw error(std::format("value cannot be greater than {}", uint16_max), current());
                emit_value(static_cast<uint16_t>(value & uint16_max));
            } else {
                const auto name = parse_name();
                if (const auto idx = ctx->get_local(name)) {
                    emit_value(*idx);
                } else
                    throw error("undefined local", current());
            }
            break;
        }
        case Opcode::ALOAD:
        case Opcode::ASTORE:
        case Opcode::PASTORE:
        case Opcode::AINVOKE: {
            if (match(TokenType::INTEGER)) {
                const auto value = str2int(current());
                if (value >= uint8_max)
                    throw error(std::format("value cannot be greater than {}", uint8_max), current());
                emit_value(static_cast<uint8_t>(value & uint8_max));
            } else {
                const auto name = parse_name();
                if (const auto idx = ctx->get_arg(name)) {
                    emit_value(*idx);
                } else
                    throw error("undefined arg", current());
            }
            break;
        }
        case Opcode::MLOAD:
        case Opcode::MFLOAD:
        case Opcode::MSTORE:
        case Opcode::MFSTORE:
        case Opcode::PMSTORE:
        case Opcode::PMFSTORE:
            emit_value(module->get_constant(parse_signature().to_string()));
            break;
        case Opcode::ARRBUILD:
        case Opcode::ARRFBUILD:
            emit_value(static_cast<uint64_t>(str2int(expect(TokenType::INTEGER))));
            break;
        case Opcode::INVOKE:
            emit_value(static_cast<uint64_t>(str2int(expect(TokenType::INTEGER))));
            break;
        case Opcode::VINVOKE:
        case Opcode::VFINVOKE:
        case Opcode::SPINVOKE:
        case Opcode::SPFINVOKE:
            emit_value(module->get_constant(parse_signature().to_string()));
            break;
        case Opcode::JMP:
        case Opcode::JT:
        case Opcode::JF:
        case Opcode::JLT:
        case Opcode::JLE:
        case Opcode::JEQ:
        case Opcode::JNE:
        case Opcode::JGE:
        case Opcode::JGT: {
            const auto label = expect(TokenType::LABEL);
            const auto jmp_val = ctx->patch_jump_to(label);
            ctx->emit((jmp_val >> 8) & uint8_max);
            ctx->emit(jmp_val & uint8_max);
            break;
        }
        case Opcode::MTPERF:
        case Opcode::MTFPERF: {
            const auto name = parse_name();
            if (const auto idx = ctx->get_match(name))
                emit_value(*idx);
            else
                throw error("undefined match", current());
            break;
        }
        case Opcode::CLOSURELOAD: {
            parse_term();
            size_t capture_count_loc = ctx->get_code().size();
            ctx->emit(0);

            size_t capture_count = 0;
            while (!match(TokenType::END)) {
                if (peek()->get_type() == TokenType::END_OF_FILE)
                    expect(TokenType::END);

                if (match(TokenType::INTEGER)) {
                    const auto value = str2int(current());
                    if (value >= uint16_max)
                        throw error(std::format("value cannot be greater than {}", uint16_max), current());
                    emit_value(static_cast<uint16_t>(value & uint16_max));
                }

                expect(TokenType::ARROW);
                if (match(TokenType::LOCAL)) {
                    ctx->emit(1);
                    if (match(TokenType::INTEGER)) {
                        const auto value = str2int(current());
                        if (value >= uint16_max)
                            throw error(std::format("value cannot be greater than {}", uint16_max), current());
                        emit_value(static_cast<uint16_t>(value & uint16_max));
                    } else {
                        const auto name = parse_name();
                        if (const auto idx = ctx->get_local(name)) {
                            emit_value(*idx);
                        } else
                            throw error("undefined local", current());
                    }
                } else if (match(TokenType::ARG)) {
                    ctx->emit(0);
                    if (match(TokenType::INTEGER)) {
                        const auto value = str2int(current());
                        if (value >= uint8_max)
                            throw error(std::format("value cannot be greater than {}", uint8_max), current());
                        emit_value(static_cast<uint8_t>(value & uint8_max));
                    } else {
                        const auto name = parse_name();
                        if (const auto idx = ctx->get_arg(name)) {
                            emit_value(*idx);
                        } else
                            throw error("undefined arg", current());
                    }
                } else {
                    const auto name = parse_name();
                    const auto arg_idx = ctx->get_arg(name);
                    const auto local_idx = ctx->get_local(name);

                    if (arg_idx && local_idx) {
                        throw error("cannot resolve name, specify 'local' or 'arg'", current());
                    } else if (arg_idx) {
                        ctx->emit(0);
                        emit_value(*arg_idx);
                    } else if (local_idx) {
                        ctx->emit(1);
                        emit_value(*local_idx);
                    } else
                        throw error("undefined arg or local", current());
                }
                capture_count++;
                parse_term();
            }
            ctx->get_code()[capture_count_loc] = capture_count;
            break;
        }
        default:
            break;
        }
        parse_term();
    }

    ValueContext Parser::parse_value() {
        switch (peek()->get_type()) {
        case TokenType::INTEGER:
            return str2int(advance());
        case TokenType::FLOAT:
            try {
                return std::stod(advance()->get_text());
            } catch (const std::out_of_range &) {
                throw error("number is out of range", current());
            } catch (const std::invalid_argument &) {
                throw error("number is invalid", current());
            }
        case TokenType::STRING:
            return destringify(advance()->get_text());
        case TokenType::CSTRING:
            return advance()->get_text()[1];
        case TokenType::LBRACKET:
            return parse_array();
        case TokenType::IDENTIFIER:
            return parse_signature().to_string();
        default:
            throw error(std::format("expected {}, array, signature",
                                    make_expected_string(TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING, TokenType::CSTRING)));
        }
    }

    vector<ValueContext> Parser::parse_array() {
        vector<ValueContext> array;
        expect(TokenType::LBRACKET);
        do {
            array.push_back(parse_value());
        } while (match(TokenType::COMMA));
        expect(TokenType::RBRACKET);
        return array;
    }

    string Parser::parse_name() {
        switch (peek()->get_type()) {
        case TokenType::IDENTIFIER:
            return advance()->get_text();
        case TokenType::STRING:
            return destringify(advance()->get_text());
        default:
            throw error(std::format("expected {}", make_expected_string(TokenType::IDENTIFIER, TokenType::STRING)));
        }
    }

    Sign Parser::parse_signature() {
        vector<SignElement> elements;
        if (match(TokenType::IDENTIFIER)) {
            elements.emplace_back(current()->get_text(), Sign::Kind::MODULE);
            while (match(TokenType::COLON)) {
                expect(TokenType::COLON);
                const auto name = expect(TokenType::IDENTIFIER)->get_text();
                elements.emplace_back(name, Sign::Kind::MODULE);
            }
            while (match(TokenType::DOT)) {
                elements.push_back(parse_sign_class_or_method());
            }
        } else {
            throw error("expected signature");
        }
        return Sign(elements);
    }

    SignElement Parser::parse_sign_class_or_method() {
        const auto name = expect(TokenType::IDENTIFIER)->get_text();
        if (match(TokenType::LPAREN)) {
            vector<SignParam> params;
            if (!match(TokenType::RPAREN)) {
                do {
                    params.push_back(parse_sign_param());
                } while (match(TokenType::COMMA));
                expect(TokenType::RPAREN);
            }
            return SignElement(name, Sign::Kind::METHOD, params);
        }
        return SignElement(name, Sign::Kind::CLASS);
    }

    SignElement Parser::parse_sign_class() {
        const auto name = expect(TokenType::IDENTIFIER)->get_text();
        return SignElement(name, Sign::Kind::CLASS);
    }

    SignElement Parser::parse_sign_method() {
        const auto name = expect(TokenType::IDENTIFIER)->get_text();
        vector<SignParam> params;
        expect(TokenType::LPAREN);
        if (!match(TokenType::RPAREN)) {
            do {
                params.push_back(parse_sign_param());
            } while (match(TokenType::COMMA));
            expect(TokenType::RPAREN);
        }
        return SignElement(name, Sign::Kind::METHOD, params);
    }

    SignParam Parser::parse_sign_param() {
        if (match(TokenType::LBRACKET)) {
            const auto name = expect(TokenType::IDENTIFIER)->get_text();
            expect(TokenType::RBRACKET);
            return SignParam(SignParam::Kind::CLASS, "[" + name + "]");
        }
        vector<SignElement> elements;

        expect(TokenType::IDENTIFIER);
        elements.emplace_back(current()->get_text(), Sign::Kind::MODULE);

        while (match(TokenType::COLON)) {
            expect(TokenType::COLON);
            const auto name = expect(TokenType::IDENTIFIER)->get_text();
            elements.emplace_back(name, Sign::Kind::MODULE);
        }

        expect(TokenType::DOT);
        elements.push_back(parse_sign_class());
        while (match(TokenType::DOT)) {
            elements.push_back(parse_sign_class());
        }
        if (match(TokenType::LPAREN)) {
            vector<SignParam> params;
            if (!match(TokenType::RPAREN)) {
                do {
                    params.push_back(parse_sign_param());
                } while (match(TokenType::COMMA));
                expect(TokenType::RPAREN);
            }
            return SignParam(SignParam::Kind::CALLBACK, Sign(elements), params);
        }
        return SignParam(SignParam::Kind::CLASS, Sign(elements));
    }

    string Parser::parse_sign_atom() {
        string result;
        auto token = expect(TokenType::IDENTIFIER);
        result += token->get_text();
        while (match(TokenType::DOT)) {
            token = expect(TokenType::IDENTIFIER);
            result += '.' + token->get_text();
        }
        return result;
    }

    ElpInfo Parser::parse() {
        return parse_assembly();
    }
}    // namespace spasm
