#include <cassert>
#include <concepts>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <variant>

#include "parser.hpp"
#include "context.hpp"
#include "elpops/elpdef.hpp"
#include "context.hpp"
#include "lexer/token.hpp"
#include "spinfo/opcode.hpp"
#include "spinfo/sign.hpp"
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
        const auto &str = token->get_text();
        try {
            if constexpr (std::same_as<int64_t, long long>)
                return static_cast<int64_t>(std::stoll(str));
            else if constexpr (std::same_as<int64_t, long>)
                return static_cast<int64_t>(std::stol(str));
            else
                return static_cast<int64_t>(std::stoi(str));
        } catch (const std::out_of_range &) {
            throw error("number is out of range", current());
        } catch (const std::invalid_argument &) {
            throw error("number is invalid", current());
        }
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

        ModuleContext ctx(file_path);
        const bool excecutable = !entry_point.empty();
        ElpInfo elp;
        elp.magic = excecutable ? 0xC0FFEEDE : 0xDEADCAFE;
        elp.major_version = 1;
        elp.minor_version = 0;

        elp.entry = ctx.get_constant(excecutable ? entry_point.to_string() : "");
        elp.imports = ctx.get_constant(vector<ValueContext>());

        const auto constants = ctx.get_constants();
        vector<CpInfo> constant_pool;
        constant_pool.reserve(constants.size());
        for (const auto &constant: constants) constant_pool.push_back(constant);
        if (const auto max = std::numeric_limits<decltype(elp.constant_pool_count)>::max(); constant_pool.size() >= max)
            throw ParserError{std::format("constant_pool_count cannot be >= {}", max), file_path, -1, -1, -1, -1};
        else {
            elp.constant_pool_count = constant_pool.size() & max;
            elp.constant_pool = constant_pool;
        }

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
        global.type = get_current_module()->get_constant(type);
        return global;
    }

    ClassInfo Parser::parse_class() {
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
        while (match(TokenType::FIELD)) fields.push_back(parse_field());

        vector<MethodInfo> methods;
        while (peek()->get_type() == TokenType::METHOD) methods.push_back(parse_method());

        expect(TokenType::END);
        parse_term(false);

        ClassInfo klass;
        if (auto value = std::get_if<int64_t>(&properties["@kind"].value))
            klass.kind = static_cast<uint8_t>(*value);
        klass.name = get_current_module()->get_constant(name);
        if (auto value = std::get_if<vector<ValueContext>>(&properties["@supers"].value))
            klass.supers = get_current_module()->get_constant(*value);

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
        field.type = get_current_module()->get_constant(type);
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
                {"@closure_start", -1},
                {"@stack_max",     32},
        };
        while (match(TokenType::PROPERTY)) {
            const auto property_name = current()->get_text();
            if (property_name == "@closure_start") {
                static constinit auto max = std::numeric_limits<decltype(std::declval<MethodInfo>().closure_start)>::max();
                auto value = str2int(expect(TokenType::INTEGER));
                if (value >= max || value < 0)
                    throw error(std::format("'{}' cannot be >= {} or < 0", property_name, max), current());
                properties[property_name] = value;
            } else if (property_name == "@stack_max") {
                static constinit auto max = std::numeric_limits<decltype(std::declval<MethodInfo>().stack_max)>::max();
                auto value = str2int(expect(TokenType::INTEGER));
                if (value >= max)
                    throw error(std::format("'{}' cannot be >= {}", property_name, max), current());
                properties[property_name] = value;
            } else
                throw error(std::format("unknown property: '{}'", property_name), current());
            parse_term();
        }

        vector<ArgInfo> args;
        while (match(TokenType::ARG)) args.push_back(parse_arg());

        vector<LocalInfo> locals;
        while (match(TokenType::LOCAL)) locals.push_back(parse_local());

        vector<ExceptionContext> exceptions;
        while (match(TokenType::EXCEPTION)) exceptions.push_back(parse_exception());

        while (peek()->get_type() != TokenType::END) {
            if (peek()->get_type() == TokenType::END_OF_FILE)
                throw error(std::format("expected {}", make_expected_string(TokenType::END)));
            parse_line();
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
        if (const auto max = std::numeric_limits<decltype(method.args_count)>::max(); args.size() >= max)
            errors.error(error(std::format("args_count cannot be >= {}", max), start));
        else {
            method.args_count = args.size() & max;
            method.args = args;
        }
        if (const auto max = std::numeric_limits<decltype(method.locals_count)>::max(); locals.size() >= max)
            errors.error(error(std::format("locals_count cannot be >= {}", max), start));
        else {
            method.locals_count = locals.size() & max;
            using T = decltype(method.closure_start);
            auto property_value = std::get<int64_t>(properties["@closure_start"].value);
            if (property_value < 0)
                method.closure_start = method.locals_count;
            else if (property_value > method.locals_count)
                throw error(std::format("@closure_start cannot be > locals_count (locals_count={})", method.locals_count), start);
            else
                method.closure_start = static_cast<T>(property_value & std::numeric_limits<T>::max());
            method.locals = locals;
        }
        if (const auto max = std::numeric_limits<decltype(method.exception_table_count)>::max(); exceptions.size() >= max)
            errors.error(error(std::format("exception_table_count cannot be >= {}", max), start));
        else {
            method.exception_table_count = exceptions.size() & max;
            method.exception_table = vector<ExceptionTableInfo>(method.exception_table_count);
            for (size_t i = 0; i < exceptions.size(); i++) {
                const auto exception = exceptions[i];
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
        method.line_info = ctx->get_line_info();
        {
            using T = decltype(method.stack_max);
            method.stack_max = static_cast<T>(std::get<int64_t>(properties["@stack_max"].value) & std::numeric_limits<T>::max());
        }
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

    ArgInfo Parser::parse_arg() {
        const auto ctx = cast<MethodContext>(get_current_context());
        const auto property = match(TokenType::PROPERTY);
        const auto name = parse_name();
        const auto name_tok = current();
        expect(TokenType::COLON);
        const auto type = parse_signature().to_string();
        parse_term();

        if (!ctx->add_arg(name))
            throw error(std::format("redefinition of local '{}'", name), name_tok);

        ArgInfo arg;
        if (property->get_text() == "@var") {
            arg.kind = 0;
        } else if (property->get_text() == "@const") {
            arg.kind = 1;
        } else
            throw error("expected '@var', '@const'", property);
        arg.name = get_current_module()->get_constant(name);
        arg.type = get_current_module()->get_constant(type);
        return arg;
    }

    LocalInfo Parser::parse_local() {
        const auto ctx = cast<MethodContext>(get_current_context());
        const auto property = expect(TokenType::PROPERTY);
        const auto name = parse_name();
        const auto name_tok = current();
        expect(TokenType::COLON);
        const auto type = parse_signature().to_string();
        parse_term();

        if (!ctx->add_local(name))
            throw error(std::format("redefinition of local '{}'", name), name_tok);

        LocalInfo local;
        if (property->get_text() == "@var") {
            local.kind = 0;
        } else if (property->get_text() == "@const") {
            local.kind = 1;
        } else
            throw error("expected '@var', '@const'", property);
        local.name = get_current_module()->get_constant(name);
        local.type = get_current_module()->get_constant(type);
        return local;
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
                    if (value >= uint16_max) {
                        ErrorGroup<ParserError> errors;
                        errors.error(error(
                                std::format("opcode '{}' cannot accept a value more than {} (value={})", opcode_token->get_text(), uint16_max, value),
                                opcode_token));
                        if (opcode != OpcodeInfo::alternate(opcode))
                            errors.note(error(std::format("use '{}' instead", OpcodeInfo::to_string(OpcodeInfo::alternate(opcode))), opcode_token));
                        throw errors;
                    }
                    if constexpr (std::same_as<decltype(value), const uint8_t>)
                        ctx->emit(0);
                    else
                        ctx->emit((value >> 8) & uint8_max);
                    ctx->emit(value & uint8_max);
                default:
                    break;
            }
        };

        if (OpcodeInfo::params_count(opcode) == 0) {
            parse_term();
            return;
        }
        if (OpcodeInfo::take_from_const_pool(opcode)) {
            emit_value(module->get_constant(parse_value()));
        } else {
            switch (opcode) {
                case Opcode::LLOAD:
                case Opcode::LFLOAD:
                case Opcode::LSTORE:
                case Opcode::LFSTORE:
                case Opcode::PLSTORE:
                case Opcode::PLFSTORE: {
                    const auto name = parse_name();
                    if (const auto idx = ctx->get_local(name)) {
                        emit_value(*idx);
                    } else
                        throw error("undefined local", current());
                    break;
                }
                case Opcode::ALOAD:
                case Opcode::ASTORE:
                case Opcode::PASTORE: {
                    const auto name = parse_name();
                    if (const auto idx = ctx->get_arg(name)) {
                        emit_value(*idx);
                    } else
                        throw error("undefined arg", current());
                    break;
                }
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
                default: {
                    const auto value_token = expect(TokenType::INTEGER);
                    const auto value = std::stoi(value_token->get_text());
                    if (value < 0)
                        throw error("value cannot be negative", value_token);
                    emit_value(static_cast<unsigned int>(value));
                    break;
                }
            }
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
        if (match(TokenType::LBRACKET)) {
            const auto name = expect(TokenType::IDENTIFIER)->get_text();
            elements.emplace_back(name, Sign::Kind::TYPE_PARAM);
            expect(TokenType::RBRACKET);
        } else if (match(TokenType::IDENTIFIER)) {
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
        vector<string> type_params;
        if (match(TokenType::LBRACKET)) {
            do {
                type_params.push_back(expect(TokenType::IDENTIFIER)->get_text());
            } while (match(TokenType::COMMA));
            expect(TokenType::RBRACKET);
        }
        if (match(TokenType::LPAREN)) {
            vector<SignParam> params;
            if (!match(TokenType::RPAREN)) {
                do {
                    params.push_back(parse_sign_param());
                } while (match(TokenType::COMMA));
                expect(TokenType::RPAREN);
            }
            return SignElement(name, Sign::Kind::METHOD, type_params, params);
        }
        return SignElement(name, Sign::Kind::CLASS, type_params);
    }

    SignElement Parser::parse_sign_class() {
        const auto name = expect(TokenType::IDENTIFIER)->get_text();
        vector<string> type_params;
        if (match(TokenType::LBRACKET)) {
            do {
                type_params.push_back(expect(TokenType::IDENTIFIER)->get_text());
            } while (match(TokenType::COMMA));
            expect(TokenType::RBRACKET);
        }
        return SignElement(name, Sign::Kind::CLASS, type_params);
    }

    SignElement Parser::parse_sign_method() {
        const auto name = expect(TokenType::IDENTIFIER)->get_text();
        vector<string> type_params;
        if (match(TokenType::LBRACKET)) {
            do {
                type_params.push_back(expect(TokenType::IDENTIFIER)->get_text());
            } while (match(TokenType::COMMA));
            expect(TokenType::RBRACKET);
        }
        vector<SignParam> params;
        expect(TokenType::LPAREN);
        if (!match(TokenType::RPAREN)) {
            do {
                params.push_back(parse_sign_param());
            } while (match(TokenType::COMMA));
            expect(TokenType::RPAREN);
        }
        return SignElement(name, Sign::Kind::METHOD, type_params, params);
    }

    SignParam Parser::parse_sign_param() {
        if (match(TokenType::LBRACKET)) {
            const auto name = expect(TokenType::IDENTIFIER)->get_text();
            expect(TokenType::RBRACKET);
            return SignParam(SignParam::Kind::TYPE_PARAM, name);
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