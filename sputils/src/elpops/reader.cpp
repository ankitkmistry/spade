#include "reader.hpp"
#include "elpdef.hpp"
#include <cstdint>

namespace spade
{
    ElpInfo ElpReader::read() {
        ElpInfo elp;
        elp.magic = read_int();
        elp.minor_version = read_int();
        elp.major_version = read_int();

        elp.entry = read_short();
        elp.imports = read_short();

        elp.constant_pool_count = read_short();
        elp.constant_pool = vector<CpInfo>(elp.constant_pool_count);
        for (int i = 0; i < elp.constant_pool_count; ++i) {
            elp.constant_pool[i] = read_cp_info();
        }

        elp.modules_count = read_short();
        elp.modules = vector<ModuleInfo>(elp.modules_count);
        for (int i = 0; i < elp.modules_count; ++i) {
            elp.modules[i] = read_module_info();
        }

        elp.meta = read_meta_info();
        // Reset the index to zero so that the file can be read again
        index = 0;
        return elp;
    }

    ModuleInfo ElpReader::read_module_info() {
        ModuleInfo module;
        module.kind = read_byte();
        module.compiled_from = read_short();
        module.name = read_short();
        module.init = read_short();

        module.globals_count = read_short();
        module.globals = vector<GlobalInfo>(module.globals_count);
        for (int i = 0; i < module.globals_count; ++i) {
            module.globals[i] = read_global_info();
        }

        module.methods_count = read_short();
        module.methods = vector<MethodInfo>(module.methods_count);
        for (int i = 0; i < module.methods_count; ++i) {
            module.methods[i] = read_method_info();
        }

        module.classes_count = read_short();
        module.classes = vector<ClassInfo>(module.classes_count);
        for (int i = 0; i < module.classes_count; ++i) {
            module.classes[i] = read_class_info();
        }

        module.constant_pool_count = read_short();
        module.constant_pool = vector<CpInfo>(module.constant_pool_count);
        for (int i = 0; i < module.constant_pool_count; ++i) {
            module.constant_pool[i] = read_cp_info();
        }

        module.modules_count = read_short();
        module.modules = vector<ModuleInfo>(module.modules_count);
        for (int i = 0; i < module.modules_count; ++i) {
            module.modules[i] = read_module_info();
        }

        module.meta = read_meta_info();
        return module;
    }

    ClassInfo ElpReader::read_class_info() {
        ClassInfo klass;
        klass.kind = read_byte();
        klass.access_flags = read_short();
        klass.name = read_short();
        klass.supers = read_short();

        klass.type_params_count = read_byte();
        klass.type_params = vector<TypeParamInfo>(klass.type_params_count);
        for (int i = 0; i < klass.type_params_count; ++i) {
            klass.type_params[i] = read_type_param_info();
        }

        klass.fields_count = read_short();
        klass.fields = vector<FieldInfo>(klass.fields_count);
        for (int i = 0; i < klass.fields_count; ++i) {
            klass.fields[i] = read_field_info();
        }

        klass.methods_count = read_short();
        klass.methods = vector<MethodInfo>(klass.methods_count);
        for (int i = 0; i < klass.methods_count; ++i) {
            klass.methods[i] = read_method_info();
        }

        klass.meta = read_meta_info();
        return klass;
    }

    FieldInfo ElpReader::read_field_info() {
        FieldInfo field;
        field.kind = read_byte();
        field.access_flags = read_short();
        field.name = read_short();
        field.type = read_short();
        field.meta = read_meta_info();
        return field;
    }

    MethodInfo ElpReader::read_method_info() {
        MethodInfo method;
        method.kind = read_byte();
        method.access_flags = read_short();
        method.name = read_short();

        method.type_params_count = read_byte();
        method.type_params = vector<TypeParamInfo>(method.type_params_count);
        for (int i = 0; i < method.type_params_count; ++i) {
            method.type_params[i] = read_type_param_info();
        }

        method.args_count = read_byte();
        method.args = vector<ArgInfo>(method.args_count);
        for (int i = 0; i < method.args_count; i++) {
            method.args[i] = read_arg_info();
        }

        method.locals_count = read_short();
        method.closure_start = read_short();
        method.locals = vector<LocalInfo>(method.locals_count);
        for (int i = 0; i < method.locals_count; i++) {
            method.locals[i] = read_local_info();
        }

        method.stack_max = read_int();
        method.code_count = read_int();
        method.code = vector<uint8_t>(method.code_count);
        for (uint32_t i = 0; i < method.code_count; i++) {
            method.code[i] = read_byte();
        }

        method.exception_table_count = read_short();
        method.exception_table = vector<ExceptionTableInfo>(method.exception_table_count);
        for (int i = 0; i < method.exception_table_count; i++) {
            method.exception_table[i] = read_exception_info();
        }

        method.line_info = read_line_info();

        method.match_count = read_short();
        method.matches = vector<MatchInfo>(method.match_count);
        for (int i = 0; i < method.match_count; i++) {
            method.matches[i] = read_match_info();
        }

        method.meta = read_meta_info();
        return method;
    }

    MatchInfo ElpReader::read_match_info() {
        MatchInfo match;
        match.case_count = read_short();
        match.cases = vector<CaseInfo>(match.case_count);
        for (int i = 0; i < match.case_count; i++) {
            CaseInfo kase;
            kase.value = read_short();
            kase.location = read_int();
            match.cases[i] = kase;
        }
        match.default_location = read_int();
        match.meta = read_meta_info();
        return match;
    }

    LineInfo ElpReader::read_line_info() {
        LineInfo line;
        line.number_count = read_short();
        line.numbers = vector<NumberInfo>(line.number_count);
        for (int i = 0; i < line.number_count; ++i) {
            NumberInfo number;
            number.times = read_byte();
            number.lineno = read_int();
            line.numbers[i] = number;
        }
        return line;
    }

    ExceptionTableInfo ElpReader::read_exception_info() {
        ExceptionTableInfo exception;
        exception.start_pc = read_int();
        exception.end_pc = read_int();
        exception.target_pc = read_int();
        exception.exception = read_short();
        exception.meta = read_meta_info();
        return exception;
    }

    LocalInfo ElpReader::read_local_info() {
        LocalInfo local;
        local.kind = read_short();
        local.name = read_short();
        local.type = read_short();
        local.meta = read_meta_info();
        return local;
    }

    ArgInfo ElpReader::read_arg_info() {
        ArgInfo arg;
        arg.kind = read_short();
        arg.name = read_short();
        arg.type = read_short();
        arg.meta = read_meta_info();
        return arg;
    }

    TypeParamInfo ElpReader::read_type_param_info() {
        TypeParamInfo typeParam;
        typeParam.name = read_short();
        return typeParam;
    }

    GlobalInfo ElpReader::read_global_info() {
        GlobalInfo global;
        global.kind = read_short();
        global.access_flags = read_short();
        global.name = read_short();
        global.type = read_short();
        global.meta = read_meta_info();
        return global;
    }

    MetaInfo ElpReader::read_meta_info() {
        MetaInfo meta;
        meta.len = read_short();
        meta.table = vector<MetaInfo::_Meta>(meta.len);
        for (int i = 0; i < meta.len; ++i) {
            MetaInfo::_Meta entry;
            entry.key = read_utf8();
            entry.value = read_utf8();
            meta.table[i] = entry;
        }
        return meta;
    }

    CpInfo ElpReader::read_cp_info() {
        CpInfo cp;
        cp.tag = read_byte();
        switch (cp.tag) {
            case 0x03:
                cp._char = read_int();
                break;
            case 0x04:
                cp._int = read_long();
                break;
            case 0x05:
                cp._float = read_long();
                break;
            case 0x06:
                cp._string = read_utf8();
                break;
            case 0x07:
                cp._array = read_container();
                break;
            default:
                corrupt_file_error();
        }
        return cp;
    }

    _Container ElpReader::read_container() {
        _Container container;
        container.len = read_short();
        container.items = vector<CpInfo>(container.len);
        for (int i = 0; i < container.len; ++i) {
            container.items[i] = read_cp_info();
        }
        return container;
    }

    _UTF8 ElpReader::read_utf8() {
        _UTF8 utf8;
        utf8.len = read_short();
        utf8.bytes = vector<uint8_t>(utf8.len);
        for (int i = 0; i < utf8.len; ++i) {
            utf8.bytes[i] = read_byte();
        }
        return utf8;
    }
}    // namespace spade
