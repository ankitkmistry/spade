#include "verifier.hpp"

namespace spade
{
    void Verifier::verify() {
        const auto magic = elp.magic;
        if (magic != 0xC0FFEEDE && magic != 0xDEADCAFE)
            throw corrupt();
        for (const auto &module: elp.modules) check_module(module);
    }

    void Verifier::check_module(const ModuleInfo &module) {
        for (int i = 0; i < module.globals_count; i++) {
            check_global(module.globals[i], module.constant_pool_count);
        }
    }

    void Verifier::check_class(const ClassInfo &klass, const uint16_t cp_count) {
        if (klass.kind > 0x03)
            throw corrupt();
        check_range(klass.name, cp_count);
        check_range(klass.supers, cp_count);
        for (const auto &field: klass.fields) check_field(field, cp_count);
        for (const auto &method: klass.methods) check_method(method, cp_count);
    }

    void Verifier::check_field(const FieldInfo &field, const uint16_t cp_count) {
        if (field.kind > 0x01)
            throw corrupt();
        check_range(field.name, cp_count);
        check_range(field.type, cp_count);
    }

    void Verifier::check_method(const MethodInfo &method, const uint16_t cp_count) {
        if (method.kind > 0x02)
            throw corrupt();
        for (const auto &type_param: method.type_params) check_range(type_param.name, cp_count);
        for (const auto &arg: method.args) check_arg(arg, cp_count);
        for (const auto &local: method.locals) check_local(local, cp_count);
        for (const auto &exception_table: method.exception_table) check_exception(exception_table, cp_count);
        const uint32_t code_count = method.code_count;
        check_line(method.line_info, code_count);
        for (const auto &match: method.matches) check_match(match, code_count, cp_count);
    }

    void Verifier::check_match(const MatchInfo &info, const uint32_t code_count, const uint16_t cp_count) {
        for (int i = 0; i < info.case_count; ++i) {
            auto kase = info.cases[i];
            check_range(kase.value, cp_count);
            check_range(kase.location, code_count);
        }
        check_range(info.default_location, code_count);
    }

    void Verifier::check_arg(const ArgInfo &arg, const uint16_t cp_count) {
        if (arg.kind > 0x01)
            throw corrupt();
        check_range(arg.type, cp_count);
    }

    void Verifier::check_local(const LocalInfo &local, const uint16_t cp_count) {
        if (local.kind > 0x01)
            throw corrupt();
        check_range(local.type, cp_count);
    }

    void Verifier::check_exception(const ExceptionTableInfo &exception, const uint16_t cp_count) {
        check_range(exception.exception, cp_count);
    }

    void Verifier::check_line(const LineInfo &line, const uint32_t code_count) {
        uint32_t total_byte_lines = 0;
        for (const auto &number: line.numbers) total_byte_lines += number.times;
        if (total_byte_lines > code_count)
            throw corrupt();
    }

    void Verifier::check_global(const GlobalInfo &global, const uint16_t cp_count) {
        if (global.kind > 0x01)
            throw corrupt();
        check_range(global.name, cp_count);
        check_range(global.type, cp_count);
    }

    void Verifier::check_range(const uint32_t i, const uint32_t count) {
        if (i >= count)
            throw corrupt();
    }

    void Verifier::check_cp(const CpInfo &info) {
        if (info.tag > 0x07)
            throw corrupt();
        if (info.tag == 0x07)
            for (const auto &item: std::get<_Container>(info.value).items) check_cp(item);
    }
}    // namespace spade
